/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/******************************************************************************
** File: cfe_sb_util.c
**
** Purpose:
**      This file contains 'access' macros and functions for reading and
**      writing message header fields.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/


/*
** Include Files
*/

#include "cfe_sb.h"
#include "ccsds.h"
#include "osapi.h"
#include "cfe_error.h"

#include "cfe_missionlib_runtime.h"
#include "cfe_mission_eds_interface_parameters.h"

#include <string.h>

/*
 * Helper function to obtain the header size from PubSub interface parameters
 */
static inline uint16 CFE_SB_GetMsgHdrSize_Impl(const CFE_SB_SoftwareBus_PubSub_Interface_t *Params)
{
    if (CFE_SB_PubSub_IsPublisherComponent(Params))
    {
        return CFE_SB_TLM_HDR_SIZE;
    }

    if (CFE_SB_PubSub_IsListenerComponent(Params))
    {
        return CFE_SB_CMD_HDR_SIZE;
    }

    /* fallback... */
    return sizeof(CCSDS_PriHdr_t);
}

/*
 * Function: CFE_SB_InitMsg - See API and header file for details
 */
void CFE_SB_InitMsg(void           *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16         Length,
                    bool        Clear )
{
    CFE_SB_Msg_t   *PktPtr = MsgPtr;
    uint16     SeqCount;

    /* Zero the entire packet if needed. */
    if (Clear)
    {
        memset(PktPtr->Byte, 0, Length);
    }
    else
    {
        /* Save the sequence count in case it must be preserved. */
        SeqCount = CCSDS_RD_SEQ(PktPtr->Hdr);
        CCSDS_CLR_PRI_HDR(PktPtr->Hdr);
    }

    /* Set the stream ID and length fields in the primary header. */
    CFE_SB_Set_PubSub_Parameters(&PktPtr->SpacePacket, &MsgId);
    CCSDS_WR_LEN(PktPtr->Hdr, Length);

    /* Restore the sequence count if needed. */
    if (!Clear)
    {
        CCSDS_WR_SEQ(PktPtr->Hdr, SeqCount);
    }
} /* end CFE_SB_InitMsg */


/******************************************************************************
**  Function:  CFE_SB_MsgHdrSize()
**
**  Purpose:
**    Get the size of a message header.
**
**  Arguments:
**    *MsgPtr - Pointer to a SB message 
**
**  Return:
**     Size of Message Header.
*/
uint16 CFE_SB_MsgHdrSize(const CFE_SB_Msg_t *MsgPtr)
{
    CFE_SB_SoftwareBus_PubSub_Interface_t Params;
    CFE_SB_Get_PubSub_Parameters(&Params, &MsgPtr->SpacePacket);

    return CFE_SB_GetMsgHdrSize_Impl(&Params);
}/* end CFE_SB_MsgHdrSize */


/*
 * Function: CFE_SB_GetUserData - See API and header file for details
 */
void *CFE_SB_GetUserData(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_SoftwareBus_PubSub_Interface_t  Params;
    CFE_SB_Get_PubSub_Parameters(&Params, &MsgPtr->SpacePacket);
    return &MsgPtr->Byte[CFE_SB_GetMsgHdrSize_Impl(&Params)];
}/* end CFE_SB_GetUserData */


/*
 * Function: CFE_SB_GetUserDataLength - See API and header file for details
 */
uint16 CFE_SB_GetUserDataLength(const CFE_SB_Msg_t *MsgPtr)
{
    CFE_SB_SoftwareBus_PubSub_Interface_t  Params;
    uint16 TotalMsgSize;
    uint16 HdrSize;

    CFE_SB_Get_PubSub_Parameters(&Params, &MsgPtr->SpacePacket);
    TotalMsgSize = CFE_SB_GetTotalMsgLength(MsgPtr);
    HdrSize = CFE_SB_GetMsgHdrSize_Impl(&Params);

    return (TotalMsgSize - HdrSize);
}/* end CFE_SB_GetUserDataLength */


/*
 * Function: CFE_SB_SetUserDataLength - See API and header file for details
 */
void CFE_SB_SetUserDataLength(CFE_SB_MsgPtr_t MsgPtr, uint16 DataLength)
{
    CFE_SB_SoftwareBus_PubSub_Interface_t  Params;
    uint16 TotalMsgSize;

    CFE_SB_Get_PubSub_Parameters(&Params, &MsgPtr->SpacePacket);
    TotalMsgSize = CFE_SB_GetMsgHdrSize_Impl(&Params) + DataLength;

    CCSDS_WR_LEN(MsgPtr->Hdr,TotalMsgSize);

}/* end CFE_SB_SetUserDataLength */


/*
 * Function: CFE_SB_GetTotalMsgLength - See API and header file for details
 */
uint16 CFE_SB_GetTotalMsgLength(const CFE_SB_Msg_t *MsgPtr)
{

    return CCSDS_RD_LEN(MsgPtr->Hdr);

}/* end CFE_SB_GetTotalMsgLength */


/*
 * Function: CFE_SB_SetTotalMsgLength - See API and header file for details
 */
void CFE_SB_SetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr,uint16 TotalLength)
{

    CCSDS_WR_LEN(MsgPtr->Hdr,TotalLength);

}/* end CFE_SB_SetTotalMsgLength */


/*
 * Function: CFE_SB_GetMsgTime - See API and header file for details
 */
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_TIME_SysTime_t TimeFromMsg;
    uint32 LocalSecs32 = 0;
    uint32 LocalSubs32 = 0;

    CFE_SB_TlmHdr_t *TlmHdrPtr;

    /* if msg type is a command or msg has no secondary hdr, time = 0 */
    if ((CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_CMD) && (CCSDS_RD_SHDR(MsgPtr->Hdr) != 0))
    {
        TlmHdrPtr = (CFE_SB_TlmHdr_t *)MsgPtr;

        LocalSecs32 = TlmHdrPtr->Sec.Seconds;

#if (CFE_MISSION_SB_PACKET_TIME_SUBSECONDS_UNITS == CFE_MISSION_SB_TIME_SUBSECS_BASE2)
         /*
          * When using base2 subseconds units,
          * Left-justify the subseconds value if less than 32 bits
          *
          * (note the comparison is based on two constants so when compiling
          * with optimization enabled this comparison should go away completely)
          */
        LocalSubs32 = TlmHdrPtr->Sec.Subseconds;
        if (sizeof(TlmHdrPtr->Sec.Subseconds) < sizeof(LocalSubs32))
        {
            LocalSubs32 <<= 8 * (sizeof(LocalSubs32) - sizeof(TlmHdrPtr->Sec.Subseconds));
        }
#elif (CFE_MISSION_SB_PACKET_TIME_SUBSECONDS_UNITS == CFE_MISSION_SB_TIME_SUBSECS_MICROSECONDS)
        /*
         * When using base10 subsecond units (millseconds, microseconds, etc)
         * then no shift is necessary, provided the value is read from the appropriately-sized
         * EDS definition.  Just use the appropriate conversion to bring it back to CFE subseconds.
         */
        LocalSubs32 = CFE_TIME_Micro2SubSecs(TlmHdrPtr->Sec.Subseconds.Micros);
#endif

    }

    /* return the packet time converted to CFE_TIME_SysTime_t format */
    TimeFromMsg.Seconds    = LocalSecs32;
    TimeFromMsg.Subseconds = LocalSubs32;

    return TimeFromMsg;

}/* end CFE_SB_GetMsgTime */


/*
 * Function: CFE_SB_SetMsgTime - See API and header file for details
 */
int32 CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr, CFE_TIME_SysTime_t NewTime)
{
    int32 Result = CFE_SB_WRONG_MSG_TYPE;

    CFE_SB_TlmHdr_t *TlmHdrPtr;

    /* cannot set time if msg type is a command or msg has no secondary hdr */
    if ((CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_CMD) && (CCSDS_RD_SHDR(MsgPtr->Hdr) != 0))
    {

        TlmHdrPtr = (CFE_SB_TlmHdr_t *) MsgPtr;
        /*
         * seconds field may be written directly (always)
         */
        TlmHdrPtr->Sec.Seconds = NewTime.Seconds;

#if (CFE_MISSION_SB_PACKET_TIME_SUBSECONDS_UNITS == CFE_MISSION_SB_TIME_SUBSECS_BASE2)
        uint32 LocalSubs32 = NewTime.Subseconds;
        if (sizeof(TlmHdrPtr->Sec.Subseconds) < sizeof(LocalSubs32))
        {
            LocalSubs32 >>= 8 * (sizeof(LocalSubs32) - sizeof(TlmHdrPtr->Sec.Subseconds));
        }
        TlmHdrPtr->Sec.Subseconds = LocalSubs32;
#elif (CFE_MISSION_SB_PACKET_TIME_SUBSECONDS_UNITS == CFE_MISSION_SB_TIME_SUBSECS_MICROSECONDS)
        /* convert time from CFE_TIME_SysTime_t format to packet format */
        TlmHdrPtr->Sec.Subseconds.Micros = CFE_TIME_Sub2MicroSecs(NewTime.Subseconds);
#endif

        Result = CFE_SUCCESS;
    }

    return Result;

}/* end CFE_SB_SetMsgTime */


/*
 * Function: CFE_SB_TimeStampMsg - See API and header file for details
 */
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_SetMsgTime(MsgPtr,CFE_TIME_GetTime());

}/* end CFE_SB_TimeStampMsg */


/*
 * Function: CFE_SB_GetCmdCode - See API and header file for details
 */
uint16 CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr, return 0 */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    /* Cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    return CCSDS_RD_FC(CmdHdrPtr->Sec);
}/* end CFE_SB_GetCmdCode */


/*
 * Function: CFE_SB_SetCmdCode - See API and header file for details
 */
int32 CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr,
                      uint16 CmdCode)
{
    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return CFE_SB_WRONG_MSG_TYPE;
    }/* end if */

    /* Cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    CCSDS_WR_FC(CmdHdrPtr->Sec,CmdCode);

    return CFE_SUCCESS;

}/* end CFE_SB_SetCmdCode */


/*
 * Function: CFE_SB_GetChecksum - See API and header file for details
 */
uint16 CFE_SB_GetChecksum(CFE_SB_MsgPtr_t MsgPtr)
{

    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    /* cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    return CCSDS_RD_CHECKSUM(CmdHdrPtr->Sec);

}/* end CFE_SB_GetChecksum */


/*
 * Function: CFE_SB_GenerateChecksum - See API and header file for details
 */
void CFE_SB_GenerateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{

    CCSDS_CommandPacket_t    *CmdPktPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return;
    }/* end if */

    CmdPktPtr = (CCSDS_CommandPacket_t *)MsgPtr;

    CCSDS_LoadCheckSum(CmdPktPtr);

}/* end CFE_SB_GenerateChecksum */


/*
 * Function: CFE_SB_ValidateChecksum - See API and header file for details
 */
bool CFE_SB_ValidateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{

    CCSDS_CommandPacket_t    *CmdPktPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return false;
    }/* end if */

    CmdPktPtr = (CCSDS_CommandPacket_t *)MsgPtr;

    return CCSDS_ValidCheckSum (CmdPktPtr);

}/* end CFE_SB_ValidateChecksum */


/*
 * Function: CFE_SB_MessageStringGet - See API and header file for details
 */
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    int32 Result;

    /*
     * Error in caller if DestMaxSize == 0.
     * Cannot terminate the string, since there is no place for the NUL
     * In this case, do nothing
     */
    if (DestMaxSize == 0)
    {
        Result = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        Result = 0;

        /*
         * Check if should use the default, which is if
         * the source string has zero length (first char is NUL).
         */
        if (DefaultString != NULL && (SourceMaxSize == 0 || *SourceStringPtr == 0))
        {
            SourceStringPtr = DefaultString;
            SourceMaxSize = DestMaxSize;
        }

        /* Reserve 1 character for the required NUL */
        --DestMaxSize;

        while (SourceMaxSize > 0 && *SourceStringPtr != 0 && DestMaxSize > 0)
        {
            *DestStringPtr = *SourceStringPtr;
            ++DestStringPtr;
            ++SourceStringPtr;
            --SourceMaxSize;
            --DestMaxSize;

            ++Result;
        }

        /* Put the NUL in the last character */
        *DestStringPtr = 0;
    }

    return Result;
}


/*
 * Function: CFE_SB_MessageStringSet - See API and header file for details
 */
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    int32 Result;

    Result = 0;

    while (SourceMaxSize > 0 && *SourceStringPtr != 0 && DestMaxSize > 0)
    {
        *DestStringPtr = *SourceStringPtr;
        ++DestStringPtr;
        ++SourceStringPtr;
        ++Result;
        --DestMaxSize;
        --SourceMaxSize;
    }

    /*
     * Pad the remaining space with NUL chars,
     * but this should NOT be included in the final size
     */
    while (DestMaxSize > 0)
    {
        /* Put the NUL in the last character */
        *DestStringPtr = 0;
        ++DestStringPtr;
        --DestMaxSize;
    }

    return Result;
}
