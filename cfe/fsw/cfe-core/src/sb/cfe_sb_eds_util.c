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
** File: cfe_sb_eds_util.c
**
** Purpose:
**      This file contains utility functions for validating and dispatching
**      generic messages based on the Electronic Data Sheets (EDS)
**
**
******************************************************************************/

/*
** Include Files
*/
#include <stdlib.h>
#include <string.h>

#include "edslib_datatypedb.h"
#include "common_types.h"
#include "cfe_sb.h"
#include "cfe_sb_priv.h"
#include "cfe_sb_eds.h"
#include "private/cfe_private.h"
#include "cfe_error.h"

#include "cfe_mission_eds_parameters.h"
#include "cfe_mission_eds_interface_parameters.h"
#include "cfe_missionlib_api.h"
#include "cfe_missionlib_runtime.h"

/*
 * Confirm that the "memaddr_Atom_t" (type used for address in messages) is equal to
 * or greater than the size of the local CPU address size.  If this assertion
 * fails, then it indicates a mission configuration issue.
 */
CompileTimeAssert(sizeof(BASE_TYPES_CpuAddress_Atom_t) >= sizeof(cpuaddr), MsgAddrSizeError);



int32 CFE_SB_EDS_UnpackInputMessage(uint16 InterfaceId, CFE_SB_Msg_t *DestBuffer, const void *SourceBuffer, uint32 *DestBufferSize, uint32 SourceBufferSize)
{
    const EdsLib_DatabaseObject_t *GD = CFE_SB_GetEds();
    EdsLib_Id_t EdsId;
    EdsLib_DataTypeDB_TypeInfo_t TypeInfo;
    CFE_SB_SoftwareBus_PubSub_Interface_t PubSubParams;
    uint16 TopicId;
    int32 Status;

    if (InterfaceId == CFE_SB_Telecommand_Interface_ID)
    {
        EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CCSDS_SPACEPACKET), CCSDS_CommandPacket_DATADICTIONARY);
    }
    else if (InterfaceId == CFE_SB_Telemetry_Interface_ID)
    {
        EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CCSDS_SPACEPACKET), CCSDS_TelemetryPacket_DATADICTIONARY);
    }
    else
    {
        EdsId = EDSLIB_ID_INVALID;
    }

    Status = EdsLib_DataTypeDB_GetTypeInfo(GD, EdsId, &TypeInfo);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    Status = EdsLib_DataTypeDB_UnpackPartialObject(GD, &EdsId, DestBuffer, SourceBuffer,
            *DestBufferSize, 8 * SourceBufferSize, 0);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    CFE_SB_Get_PubSub_Parameters(&PubSubParams, &DestBuffer->SpacePacket);
    if (InterfaceId == CFE_SB_Telecommand_Interface_ID)
    {
        CFE_SB_Listener_Component_t ListenerParams;
        CFE_SB_UnmapListenerComponent(&ListenerParams, &PubSubParams);
        TopicId = ListenerParams.Telecommand.TopicId;
    }
    else if (InterfaceId == CFE_SB_Telemetry_Interface_ID)
    {
        CFE_SB_Publisher_Component_t PublisherParams;
        CFE_SB_UnmapPublisherComponent(&PublisherParams, &PubSubParams);
        TopicId = PublisherParams.Telemetry.TopicId;
    }
    else
    {
        TopicId = 0;
    }

    Status = CFE_MissionLib_GetArgumentType(&CFE_SOFTWAREBUS_INTERFACE,
            InterfaceId, TopicId, 1, 1, &EdsId);
    if (Status != CFE_MISSIONLIB_SUCCESS)
    {
        return CFE_STATUS_UNKNOWN_MSG_ID;
    }

    Status = EdsLib_DataTypeDB_UnpackPartialObject(GD, &EdsId, DestBuffer, SourceBuffer,
            *DestBufferSize, 8 * SourceBufferSize, TypeInfo.Size.Bytes);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    Status = EdsLib_DataTypeDB_GetTypeInfo(GD, EdsId, &TypeInfo);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    /*
     * For CFS it is helpful to recompute at least the Length fields to be based on
     * the native size rather than the encoded size.  For instance the
     * CFE_SB_GetUserDataLength() function depends on having the internal
     * length set according to its native structure size or else applications
     * will get unexpected values from this.
     */
    Status = EdsLib_DataTypeDB_VerifyUnpackedObject(GD, EdsId, DestBuffer, 
        SourceBuffer, EDSLIB_DATATYPEDB_RECOMPUTE_LENGTH);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_BUFFER_INVALID;
    }

    *DestBufferSize = TypeInfo.Size.Bytes;
    return CFE_SUCCESS;
}

int32 CFE_SB_EDS_PackOutputMessage(uint16 InterfaceId, void *DestBuffer, const CFE_SB_Msg_t *SourceBuffer, uint32 *DestBufferSize, uint32 SourceBufferSize)
{
    const EdsLib_DatabaseObject_t *GD = CFE_SB_GetEds();
    EdsLib_Id_t EdsId;
    EdsLib_DataTypeDB_TypeInfo_t TypeInfo;
    CFE_SB_SoftwareBus_PubSub_Interface_t PubSubParams;
    uint16 TopicId;
    int32 Status;

    if (InterfaceId == CFE_SB_Telecommand_Interface_ID)
    {
        EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CCSDS_SPACEPACKET), CCSDS_CommandPacket_DATADICTIONARY);
    }
    else if (InterfaceId == CFE_SB_Telemetry_Interface_ID)
    {
        EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CCSDS_SPACEPACKET), CCSDS_TelemetryPacket_DATADICTIONARY);
    }
    else
    {
        EdsId = EDSLIB_ID_INVALID;
    }

    Status = EdsLib_DataTypeDB_GetTypeInfo(GD, EdsId, &TypeInfo);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    CFE_SB_Get_PubSub_Parameters(&PubSubParams, &SourceBuffer->SpacePacket);
    if (InterfaceId == CFE_SB_Telemetry_Interface_ID)
    {
        CFE_SB_Publisher_Component_t PublisherParams;
        CFE_SB_UnmapPublisherComponent(&PublisherParams, &PubSubParams);
        TopicId = PublisherParams.Telemetry.TopicId;
    }
    else if (InterfaceId == CFE_SB_Telecommand_Interface_ID)
    {
        CFE_SB_Listener_Component_t ListenerParams;
        CFE_SB_UnmapListenerComponent(&ListenerParams, &PubSubParams);
        TopicId = ListenerParams.Telecommand.TopicId;
    }
    else
    {
        TopicId = 0;
    }

    Status = CFE_MissionLib_GetArgumentType(&CFE_SOFTWAREBUS_INTERFACE,
            InterfaceId, TopicId, 1, 1, &EdsId);
    if (Status != CFE_MISSIONLIB_SUCCESS)
    {
        return CFE_STATUS_UNKNOWN_MSG_ID;
    }

    Status = EdsLib_DataTypeDB_GetTypeInfo(GD, EdsId, &TypeInfo);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    Status = EdsLib_DataTypeDB_PackCompleteObject(GD, &EdsId, DestBuffer, SourceBuffer,
            8 * *DestBufferSize, SourceBufferSize);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    *DestBufferSize = (TypeInfo.Size.Bits + 7) / 8;
    return CFE_SUCCESS;
}

int32 CFE_SB_EDS_Dispatch(
        uint16 InterfaceId,
        uint16 IndicationIndex,
        uint16 DispatchTableID,
        const CFE_SB_Msg_t *Message,
        const void* DispatchTable
)
{
    const EdsLib_DatabaseObject_t *GD = CFE_SB_GetEds();
    CFE_MissionLib_TopicInfo_t TopicInfo;
    CFE_MissionLib_IndicationInfo_t IndicationInfo;
    EdsLib_DataTypeDB_TypeInfo_t TypeInfo;
    CFE_SB_SoftwareBus_PubSub_Interface_t PubSubParams;
    EdsLib_Id_t ArgumentType;
    int32_t Status;
    uint16_t TopicId;
    uint16_t DispatchOffset;
    union
    {
        cpuaddr MemAddr;
        const void* GenericPtr;
        int32 (**DispatchFunc)(const CFE_SB_Msg_t *);
    } HandlerPtr;

    HandlerPtr.GenericPtr = DispatchTable;
    DispatchOffset = 0;
    CFE_SB_Get_PubSub_Parameters(&PubSubParams, &Message->SpacePacket);

    switch(InterfaceId)
    {
    case CFE_SB_Telemetry_Interface_ID:
    {
        CFE_SB_Publisher_Component_t PublisherParams;
        CFE_SB_UnmapPublisherComponent(&PublisherParams, &PubSubParams);
        TopicId = PublisherParams.Telemetry.TopicId;
        break;
    }
    case CFE_SB_Telecommand_Interface_ID:
    {
        CFE_SB_Listener_Component_t ListenerParams;
        CFE_SB_UnmapListenerComponent(&ListenerParams, &PubSubParams);
        TopicId = ListenerParams.Telecommand.TopicId;
        break;
    }
    default:
        TopicId = 0;
        break;
    }

    Status = CFE_MissionLib_GetTopicInfo(&CFE_SOFTWAREBUS_INTERFACE, InterfaceId, TopicId, &TopicInfo);
    if (Status != CFE_MISSIONLIB_SUCCESS)
    {
        return CFE_STATUS_UNKNOWN_MSG_ID;
    }

    if (TopicInfo.DispatchTableId != DispatchTableID)
    {
        return CFE_SB_NO_SUBSCRIBERS;
    }

    Status = CFE_MissionLib_GetIndicationInfo(&CFE_SOFTWAREBUS_INTERFACE, InterfaceId, TopicId, IndicationIndex, &IndicationInfo);
    if (Status != CFE_MISSIONLIB_SUCCESS || IndicationInfo.NumArguments != 1)
    {
        /*
         * This dispatch function only handles single-argument commands defined in EDS.
         * This is really a programmer/EDS error and not a runtime error if this occurs
         */
        return CFE_SB_NOT_IMPLEMENTED;
    }

    Status = CFE_MissionLib_GetArgumentType(&CFE_SOFTWAREBUS_INTERFACE, InterfaceId, TopicId, IndicationIndex, 1, &ArgumentType);
    if (Status != CFE_MISSIONLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    if (IndicationInfo.SubcommandArgumentId == 1 && IndicationInfo.NumSubcommands > 0)
    {
        /* Derived command case -- the indication corresponds to a multiple entries in the dispatch table.
         * The actual type of the argument must be determined to figure out which one to invoke. */
        EdsLib_DataTypeDB_DerivativeObjectInfo_t DerivObjInfo;

        Status = EdsLib_DataTypeDB_IdentifyBuffer(GD, ArgumentType, Message->Byte, &DerivObjInfo);
        if (Status != EDSLIB_SUCCESS)
        {
            return CFE_STATUS_UNKNOWN_MSG_ID;
        }

        /* NOTE: The "IdentifyBuffer" outputs a 0-based index, but the Subcommand is a 1-based index */
        Status = CFE_MissionLib_GetSubcommandOffset(&CFE_SOFTWAREBUS_INTERFACE, InterfaceId, TopicId, IndicationIndex,
                1+ DerivObjInfo.DerivativeTableIndex, &DispatchOffset);
        if (Status != EDSLIB_SUCCESS)
        {
            return CFE_STATUS_BAD_COMMAND_CODE;
        }

        ArgumentType = DerivObjInfo.EdsId;
    }

    Status = EdsLib_DataTypeDB_GetTypeInfo(GD, ArgumentType, &TypeInfo);
    if (Status != EDSLIB_SUCCESS)
    {
        return CFE_SB_INTERNAL_ERR;
    }

    if (TypeInfo.Size.Bytes > CFE_SB_GetTotalMsgLength((CFE_SB_MsgPtr_t)Message))
    {
        return CFE_STATUS_WRONG_MSG_LENGTH;
    }

    HandlerPtr.MemAddr += TopicInfo.DispatchStartOffset;
    HandlerPtr.MemAddr += DispatchOffset;
    if (*HandlerPtr.DispatchFunc == NULL)
    {
        return CFE_SB_NO_SUBSCRIBERS;
    }

    return (*HandlerPtr.DispatchFunc)(Message);
}

/******************************************************************************
**  Function:  CFE_SB_MsgId_From_TopicId()
**
**  Purpose:
**    Convert a portable offset into the final CCSDS APID (CPU-specific) message ID
**
**  Arguments:
**    StreamIdOffset - message ID to convert
**
**  Return:
**    Message ID for message payload (cpu-specific)
*/
CFE_SB_MsgId_t CFE_SB_MsgId_From_TopicId(uint16 TopicId)
{
    CFE_SB_SoftwareBus_PubSub_Interface_t Result;
    uint16 LocalInstance = CFE_PSP_GetProcessorId();

    if (TopicId >= CFE_MISSION_TELECOMMAND_BASE_TOPICID &&
            TopicId < CFE_MISSION_TELECOMMAND_MAX_TOPICID)
    {
        CFE_SB_Listener_Component_t Params;
        Params.Telecommand.InstanceNumber = LocalInstance;
        Params.Telecommand.TopicId = TopicId;
        CFE_SB_MapListenerComponent(&Result, &Params);
    }
    else if (TopicId >= CFE_MISSION_TELEMETRY_BASE_TOPICID &&
            TopicId < CFE_MISSION_TELEMETRY_MAX_TOPICID)
    {
        CFE_SB_Publisher_Component_t Params;
        Params.Telemetry.InstanceNumber = LocalInstance;
        Params.Telemetry.TopicId = TopicId;
        CFE_SB_MapPublisherComponent(&Result, &Params);
    }
    else
    {
        memset(&Result, 0, sizeof(Result));
    }

    return Result;
} /* end CFE_SB_MsgId_From_TopicId */

/******************************************************************************
**  Function:  CFE_SB_MsgId_To_TopicId()
**
**  Purpose:
**    Convert a CCSDS APID (CPU-specific) message ID into a portable offset
**
**  Arguments:
**    MsgId - message ID from packet payload
**
**  Return:
**    Portable Message ID Table Offset (non-cpu-specific) or zero if invalid
*/
uint16 CFE_SB_MsgId_To_TopicId(CFE_SB_MsgId_t MsgId)
{
    uint16 TopicID;

    TopicID = 0;

    /*
     * JPHFIX:
     * This needs some sort of lookup function to positively identify
     * whether the message is CMD or TLM.  This needs to come from EDS without
     * relying on an assumption of CCSDS primary header bits.
     *
     * For now, just try both.  A mismatch should produce a topic ID of zero
     * which is invalid.  This is less efficient of course but it will work
     * for now and it has no reliance on CCSDS framing bits.
     */
    if (CFE_SB_PubSub_IsListenerComponent(&MsgId))
    {
        CFE_SB_Listener_Component_t Result;
        CFE_SB_UnmapListenerComponent(&Result, &MsgId);
        TopicID = Result.Telecommand.TopicId;
    }
    if (CFE_SB_PubSub_IsPublisherComponent(&MsgId))
    {
        CFE_SB_Publisher_Component_t Result;
        CFE_SB_UnmapPublisherComponent(&Result, &MsgId);
        TopicID = Result.Telemetry.TopicId;
    }

    return TopicID;
} /* end CFE_SB_MsgId_To_TopicId */



/*****************************************************************************/
