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

/*
** File   : cfe_sb_eds.h
**
** Author : Joe Hickey
**
** Purpose:
**      API for the Electronic Data Sheets (EDS) for software bus messages
**
**      Contains prototypes for the generic pack, unpack, and dispatch routines
**      that are part of the Software Bus (SB) application.  These are the EDS
**      API calls that actually work with instances of message objects, which
**      are all some form of CCSDS_SpacePacket_t (the base type of all SB messages).
**
**      This is a separate header file as these API calls are only available in
**      an EDS-enabled build.
**
**  Note - this public header file does not include any "EdsLib" headers directly...
**  The SB EDS implementation abstracts the relevant EdsLib data types so that
**  CFS applications are only dealing with the SB API directly, not going directly to
**  EdsLib to perform data operations.  This keeps the include paths simpler and
**  eases migration.
**
*/

#ifndef _CFE_SB_EDS_API_H
#define _CFE_SB_EDS_API_H

#include <common_types.h>
#include <cfe_sb.h>
#include <cfe_sb_eds_db.h>
#include <cfe_mission_eds_parameters.h>

/******************************************************************************
**  Function:  CFE_SB_EDS_Dispatch()
*/
/**
 * Dispatch an incoming message to its handler function, given a dispatch table.
 *
 * The incoming message will be identified and matched against the given
 * EDS-defined interface.
 *
 * If the message payload is defined by the application's EDS, the correct
 * handler function in the dispatch table will be called.
 *
 * \param Message           Pointer to the Software Bus message (with headers)
 * \returns If successfully dispatched, returns code from handler function, otherwise an error code.
 */
int32 CFE_SB_EDS_Dispatch(
        uint16 InterfaceID,
        uint16 IndicationIndex,
        uint16 DispatchTableID,
        const CFE_SB_Msg_t *Message,
        const void* DispatchTable
);


/******************************************************************************
**  Function:  CFE_SB_EDS_UnpackInputMessage()
**
**  Purpose:
**    Convert a structure from the packed (external) format into the native system
**    (C structure) representation.  Data will be re-encoded as it is unpacked if
**    the native system encoding differs from the external encoding.
**
**  Arguments:
**    DestBuffer - Local buffer to store the unpacked data
**    SourceBuffer - buffer / bit stream to read packed data from
**    DestBufferSize - input max size of destination (unpacked) data buffer, output actual size
**    SourceBufferSize - size of source (packed) data buffer
**
**  Return:
**    CFE_SUCCESS on success
*/
int32 CFE_SB_EDS_UnpackInputMessage(uint16 InterfaceId, CFE_SB_Msg_t *DestBuffer, const void *SourceBuffer, uint32 *DestBufferSize, uint32 SourceBufferSize);


/******************************************************************************
**  Function:  CFE_SB_EDS_PackOutputMessage()
**
**  Purpose:
**    Convert a structure from the C structure representation into the packed
**    external format. Data will be re-encoded as it is packed if the native
**    system encoding differs from the external encoding.
**
**  Arguments:
**    DestBuffer - buffer / bit stream to write packed data to
**    SourceBuffer - Local buffer holding the unpacked data
**    DestBufferSize - input max size of destination (packed) data buffer, output actual size
**    SourceBufferSize - size of source (unpacked) data buffer (dest buffer must be at least this size)
**
**  Return:
**    CFE_SUCCESS on success
*/
int32 CFE_SB_EDS_PackOutputMessage(uint16 InterfaceId, void *DestBuffer, const CFE_SB_Msg_t *SourceBuffer, uint32 *DestBufferSize, uint32 SourceBufferSize);


#endif /* _CFE_SB_EDS_API_H */

