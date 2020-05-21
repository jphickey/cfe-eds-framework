/*******************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: ci_lab_app.c
**
** Purpose:
**   This file contains the source code for the Command Ingest task.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "ci_lab_app.h"
#include "ci_lab_perfids.h"
#include "ci_lab_msgids.h"
#include "ci_lab_events.h"
#include "ci_lab_version.h"

#include "cfe_sb_eds.h"

#include "ci_lab_eds_dictionary.h"
#include "ci_lab_eds_dispatcher.h"


/*
** CI global data...
*/

/*
 * Declaring the CI_LAB_IngestBuffer as a union
 * ensures it is aligned appropriately to
 * store a CFE_SB_Msg_t type.
 */
typedef union
{
    CFE_SB_Msg_t MsgHdr;
    uint8        bytes[CI_LAB_MAX_INGEST];
    uint16       hwords[2];
} CI_LAB_IngestBuffer_t;

typedef union
{
    CFE_SB_Msg_t   MsgHdr;
    CI_LAB_HkTlm_t HkTlm;
} CI_LAB_HkTlm_Buffer_t;

typedef struct
{
    bool            SocketConnected;
    CFE_SB_PipeId_t CommandPipe;
    CFE_SB_MsgPtr_t MsgPtr;
    uint32          SocketID;
    OS_SockAddr_t   SocketAddress;

    CI_LAB_HkTlm_Buffer_t HkBuffer;
    CI_LAB_IngestBuffer_t IngestBuffer;
    CI_LAB_IngestBuffer_t NetworkBuffer;
} CI_LAB_GlobalData_t;

CI_LAB_GlobalData_t CI_LAB_Global;

static CFE_EVS_BinFilter_t CI_LAB_EventFilters[] =
    {/* Event ID    mask */
     {CI_LAB_SOCKETCREATE_ERR_EID, 0x0000}, {CI_LAB_SOCKETBIND_ERR_EID, 0x0000}, {CI_LAB_STARTUP_INF_EID, 0x0000},
     {CI_LAB_COMMAND_ERR_EID, 0x0000},      {CI_LAB_COMMANDNOP_INF_EID, 0x0000}, {CI_LAB_COMMANDRST_INF_EID, 0x0000},
     {CI_LAB_INGEST_INF_EID, 0x0000},       {CI_LAB_INGEST_ERR_EID, 0x0000}};

/*
 * Individual message handler function prototypes
 *
 * Per the recommended code pattern, these should accept a const pointer
 * to a structure type which matches the message, and return an int32
 * where CFE_SUCCESS (0) indicates successful handling of the message.
 */
int32 CI_LAB_Noop(const CI_LAB_Noop_t *data);
int32 CI_LAB_ResetCounters(const CI_LAB_ResetCounters_t *data);
int32 CI_LAB_ReportHousekeeping(const CCSDS_CommandPacket_t *data);

/*
 * Define a lookup table for CI lab command codes
 */
static const CI_LAB_Application_Component_Telecommand_DispatchTable_t CI_LAB_TC_DISPATCH_TABLE =
{
        .CMD =
        {
                .Noop_indication            = CI_LAB_Noop,
                .ResetCounters_indication   = CI_LAB_ResetCounters,

        },
        .SEND_HK = {.indication = CI_LAB_ReportHousekeeping}
};

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* CI_Lab_AppMain() -- Application entry point and main process loop          */
/* Purpose: This is the Main task event loop for the Command Ingest Task      */
/*            The task handles all interfaces to the data system through      */
/*            the software bus. There is one pipeline into this task          */
/*            The task is sceduled by input into this pipeline.               */
/*            It can receive Commands over this pipeline                      */
/*            and acts accordingly to process them.                           */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void CI_Lab_AppMain(void)
{
    int32  status;
    uint32 RunStatus = CFE_ES_RunStatus_APP_RUN;

    CFE_ES_PerfLogEntry(CI_LAB_MAIN_TASK_PERF_ID);

    CI_LAB_TaskInit();

    /*
    ** CI Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == true)
    {
        CFE_ES_PerfLogExit(CI_LAB_MAIN_TASK_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&CI_LAB_Global.MsgPtr, CI_LAB_Global.CommandPipe, 500);

        CFE_ES_PerfLogEntry(CI_LAB_MAIN_TASK_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            CI_LAB_Application_Component_Telecommand_Dispatch(
                    CFE_SB_Telecommand_indication_Command_ID, CI_LAB_Global.MsgPtr, &CI_LAB_TC_DISPATCH_TABLE);
        }

        /* Regardless of packet vs timeout, always process uplink queue      */
        if (CI_LAB_Global.SocketConnected)
        {
            CI_LAB_ReadUpLink();
        }
    }

    CFE_ES_ExitApp(RunStatus);

} /* End of CI_Lab_AppMain() */

/*
** CI delete callback function.
** This function will be called in the event that the CI app is killed.
** It will close the network socket for CI
*/
void CI_LAB_delete_callback(void)
{
    OS_printf("CI delete callback -- Closing CI Network socket.\n");
    OS_close(CI_LAB_Global.SocketID);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* CI_LAB_TaskInit() -- CI initialization                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_LAB_TaskInit(void)
{
    int32  status;
    uint16 DefaultListenPort;

    memset(&CI_LAB_Global, 0, sizeof(CI_LAB_Global));

    CFE_ES_RegisterApp();

    CFE_EVS_Register(CI_LAB_EventFilters, sizeof(CI_LAB_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_EventFilter_BINARY);

    CFE_SB_EDS_RegisterSelf(&CI_LAB_DATATYPE_DB);

    CFE_SB_CreatePipe(&CI_LAB_Global.CommandPipe, CI_LAB_PIPE_DEPTH, "CI_LAB_CMD_PIPE");
    CFE_SB_Subscribe(CI_LAB_CMD_MID, CI_LAB_Global.CommandPipe);
    CFE_SB_Subscribe(CI_LAB_SEND_HK_MID, CI_LAB_Global.CommandPipe);

    status = OS_SocketOpen(&CI_LAB_Global.SocketID, OS_SocketDomain_INET, OS_SocketType_DATAGRAM);
    if (status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(CI_LAB_SOCKETCREATE_ERR_EID, CFE_EVS_EventType_ERROR, "CI: create socket failed = %d",
                          (int)status);
    }
    else
    {
        OS_SocketAddrInit(&CI_LAB_Global.SocketAddress, OS_SocketDomain_INET);
        DefaultListenPort = CI_LAB_BASE_UDP_PORT + CFE_PSP_GetProcessorId() - 1;
        OS_SocketAddrSetPort(&CI_LAB_Global.SocketAddress, DefaultListenPort);

        status = OS_SocketBind(CI_LAB_Global.SocketID, &CI_LAB_Global.SocketAddress);

        if (status != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CI_LAB_SOCKETBIND_ERR_EID, CFE_EVS_EventType_ERROR, "CI: bind socket failed = %d",
                              (int)status);
        }
        else
        {
            CI_LAB_Global.SocketConnected = true;
            CFE_ES_WriteToSysLog("CI_LAB listening on UDP port: %u\n", (unsigned int)DefaultListenPort);
        }
    }

    CI_LAB_ResetCounters_Internal();

    /*
    ** Install the delete handler
    */
    OS_TaskInstallDeleteHandler(&CI_LAB_delete_callback);

    CFE_SB_InitMsg(&CI_LAB_Global.HkBuffer.MsgHdr, CI_LAB_HK_TLM_MID, sizeof(CI_LAB_Global.HkBuffer.HkTlm), true);

    CFE_EVS_SendEvent(CI_LAB_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "CI Lab Initialized.  Version %d.%d.%d.%d",
                      CI_LAB_MAJOR_VERSION, CI_LAB_MINOR_VERSION, CI_LAB_REVISION, CI_LAB_MISSION_REV);

} /* End of CI_LAB_TaskInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*  Name:  CI_LAB_Noop                                                         */
/*                                                                             */
/*  Purpose:                                                                   */
/*     Handle NOOP command packets                                             */
/*                                                                             */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CI_LAB_Noop(const CI_LAB_Noop_t *data)
{
    /* Does everything the name implies */
    CI_LAB_Global.HkBuffer.HkTlm.Payload.CommandCounter++;

    CFE_EVS_SendEvent(CI_LAB_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "CI: NOOP command");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*  Name:  CI_LAB_ResetCounters                                                */
/*                                                                             */
/*  Purpose:                                                                   */
/*     Handle ResetCounters command packets                                    */
/*                                                                             */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CI_LAB_ResetCounters(const CI_LAB_ResetCounters_t *data)
{
    CFE_EVS_SendEvent(CI_LAB_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "CI: RESET command");
    CI_LAB_ResetCounters_Internal();
    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  CI_LAB_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the CI task  */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 CI_LAB_ReportHousekeeping(const CCSDS_CommandPacket_t *data)
{
    CI_LAB_Global.HkBuffer.HkTlm.Payload.SocketConnected = CI_LAB_Global.SocketConnected;
    CFE_SB_TimeStampMsg(&CI_LAB_Global.HkBuffer.MsgHdr);
    CFE_SB_SendMsg(&CI_LAB_Global.HkBuffer.MsgHdr);
    return CFE_SUCCESS;

} /* End of CI_LAB_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  CI_LAB_ResetCounters_Internal                                      */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void CI_LAB_ResetCounters_Internal(void)
{
    /* Status of commands processed by CI task */
    CI_LAB_Global.HkBuffer.HkTlm.Payload.CommandCounter      = 0;
    CI_LAB_Global.HkBuffer.HkTlm.Payload.CommandErrorCounter = 0;

    /* Status of packets ingested by CI task */
    CI_LAB_Global.HkBuffer.HkTlm.Payload.IngestPackets = 0;
    CI_LAB_Global.HkBuffer.HkTlm.Payload.IngestErrors  = 0;

    return;

} /* End of CI_LAB_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* CI_LAB_ReadUpLink() --                                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void CI_LAB_ReadUpLink(void)
{
    int    i;
    int32  status;
    uint32 DataSize;

    for (i = 0; i <= 10; i++)
    {
        status = OS_SocketRecvFrom(CI_LAB_Global.SocketID, CI_LAB_Global.NetworkBuffer.bytes,
                sizeof(CI_LAB_Global.NetworkBuffer), &CI_LAB_Global.SocketAddress, OS_CHECK);
        if (status > CI_LAB_MAX_INGEST)
        {
            CI_LAB_Global.HkBuffer.HkTlm.Payload.IngestErrors++;
            CFE_EVS_SendEvent(CI_LAB_INGEST_ERR_EID, CFE_EVS_EventType_ERROR,
                    "CI: L%d, cmd %0x %0x dropped, too long\n", __LINE__,
                    CI_LAB_Global.NetworkBuffer.hwords[0], CI_LAB_Global.NetworkBuffer.hwords[1]);
        }
        else if (status > 0)
        {
            /* Packet is in external wire-format byte order - unpack it and copy */
            DataSize = sizeof(CI_LAB_Global.IngestBuffer);
            status   = CFE_SB_EDS_UnpackInputMessage(CFE_SB_Telecommand_Interface_ID, &CI_LAB_Global.IngestBuffer.MsgHdr,
                    CI_LAB_Global.NetworkBuffer.bytes, &DataSize, status);

            if (status != CFE_SUCCESS)
            {
                CI_LAB_Global.HkBuffer.HkTlm.Payload.IngestErrors++;
                CFE_EVS_SendEvent(CI_LAB_INGEST_ERR_EID, CFE_EVS_EventType_ERROR,
                        "CI: L%d, cmd %0x %0x dropped, undefined payload?\n", __LINE__,
                        CI_LAB_Global.NetworkBuffer.hwords[0], CI_LAB_Global.NetworkBuffer.hwords[1]);
            }
            else
            {
                CFE_ES_PerfLogEntry(CI_LAB_SOCKET_RCV_PERF_ID);
                CI_LAB_Global.HkBuffer.HkTlm.Payload.IngestPackets++;
                status = CFE_SB_SendMsg(&CI_LAB_Global.IngestBuffer.MsgHdr);
                CFE_ES_PerfLogExit(CI_LAB_SOCKET_RCV_PERF_ID);
            }
        }
        else
        {
            break; /* no (more) messages */
        }
    }

    return;

} /* End of CI_LAB_ReadUpLink() */


