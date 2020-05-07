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
** File:  ccsds.h
**
** Purpose:
**      Define typedefs and macros for CCSDS packet headers.
**
******************************************************************************/

#ifndef _ccsds_
#define _ccsds_

/*
** Include Files
*/

#include "common_types.h"
#include "cfe_mission_cfg.h"

/*
 * Actual instantiation of CCSDS types is done through EDS
 */
#include "ccsds_spacepacket_eds_typedefs.h"

/*
** Macro Definitions
*/

/**********************************************************************
** Constant values.
**********************************************************************/

/* Value of packet type for a telemetry packet. */
#define CCSDS_TLM  0
/* Value of packet type for a command packet. */
#define CCSDS_CMD  1

/* Value of secondary header flag if secondary header not present. */
#define CCSDS_NO_SEC_HDR   0
/* Value of secondary header flag if secondary header exists. */
#define CCSDS_HAS_SEC_HDR  1

#define NUM_CCSDS_APIDS        2048
#define NUM_CCSDS_PKT_TYPES    2


/**********************************************************************
** Initial values for CCSDS header fields.
**********************************************************************/

/* Initial value of the sequence count. */
#define CCSDS_INIT_SEQ      0
/* Initial value of the sequence flags. */
#define CCSDS_INIT_SEQFLG   3
/* Initial value of the command function code. */
#define CCSDS_INIT_FC       0
/* Initial value of the command checksum. */
#define CCSDS_INIT_CHECKSUM 0

/* Note: the stream ID and length are always explicitly set for a packet,
** so default values are not required.  */


/**********************************************************************
** Macros for reading and writing bit fields in a 16-bit integer.
** These are used to implement the read and write macros below.
**********************************************************************/

/* Read bits specified by 'mask' from 'word' and shift down by 'shift'. */
#define CCSDS_RD_BITS(word,mask,shift) \
   (((word) & mask) >> shift)

/* Shift 'value' up by 'shift' and write to those bits in 'word' that
** are specified by 'mask'.  Other bits in 'word' are unchanged.   */
#define CCSDS_WR_BITS(word,mask,shift,value) \
   ((word) = (uint16)(((word) & ~mask) | (((value) & (mask >> shift)) << shift)))


/**********************************************************************
** Macros for reading and writing the fields in a CCSDS header.  All
** of the macros are used in a similar way:
**
**   CCSDS_RD_xxx(header)        -- Read field xxx from header.
**   CCSDS_WR_xxx(header,value)  -- Write value to field xxx of header.
**
** Note that 'header' is a reference to the actual header structure,
** not to a pointer to the structure.  If using a pointer, one must
** refer to the structure as *pointer.
**
** The CCSDS_WR_xxx macros may refer to 'header' more than once; thus
** the expression for 'header' must NOT contain any side effects.
**********************************************************************/

/* Read application ID from primary header. */
#define CCSDS_RD_APID(phdr)         ((phdr).BaseHdr.AppId)
/* Write application ID to primary header. */
#define CCSDS_WR_APID(phdr,value)   ((phdr).BaseHdr.AppId = value)

/* Read secondary header flag from primary header. */
#define CCSDS_RD_SHDR(phdr)         CCSDS_RD_BITS((phdr).BaseHdr.SecHdrFlags, 0x01, 0)
/* Write secondary header flag to primary header. */
#define CCSDS_WR_SHDR(phdr,value)   CCSDS_WR_BITS((phdr).BaseHdr.SecHdrFlags, 0x01, 0, value)

/* Read packet type (0=TLM,1=CMD) from primary header. */
#define CCSDS_RD_TYPE(phdr)         CCSDS_RD_BITS((phdr).BaseHdr.SecHdrFlags, 0x02, 1)
/* Write packet type (0=TLM,1=CMD) to primary header. */
#define CCSDS_WR_TYPE(phdr,value)   CCSDS_WR_BITS((phdr).BaseHdr.SecHdrFlags, 0x02, 1, value)

/* Read CCSDS version from primary header. */
#define CCSDS_RD_VERS(phdr)         ((phdr).BaseHdr.VersionId)
/* Write CCSDS version to primary header. */
#define CCSDS_WR_VERS(phdr,value)   ((phdr).BaseHdr.VersionId = value)

/* Read sequence count from primary header. */
#define CCSDS_RD_SEQ(phdr)          ((phdr).BaseHdr.Sequence)
/* Write sequence count to primary header. */
#define CCSDS_WR_SEQ(phdr,value)    ((phdr).BaseHdr.Sequence = value)

/* Read sequence flags from primary header. */
#define CCSDS_RD_SEQFLG(phdr)       ((phdr).BaseHdr.SeqFlag)
/* Write sequence flags to primary header. */
#define CCSDS_WR_SEQFLG(phdr,value) ((phdr).BaseHdr.SeqFlag = value)

/* Read total packet length from primary header. */
#define CCSDS_RD_LEN(phdr)         ((phdr).BaseHdr.Length + 7)
/* Write total packet length to primary header. */
#define CCSDS_WR_LEN(phdr,value)   ((phdr).BaseHdr.Length = (value) - 7)

/* Read function code from command secondary header. */
#define CCSDS_RD_FC(shdr)           CCSDS_RD_BITS((shdr).Command, 0x7F, 0)
/* Write function code to command secondary header. */
#define CCSDS_WR_FC(shdr,value)     CCSDS_WR_BITS((shdr).Command, 0x7F, 0, value)

/* Read checksum from command secondary header. */
#define CCSDS_RD_CHECKSUM(shdr)     ((shdr).Checksum)
/* Write checksum to command secondary header. */
#define CCSDS_WR_CHECKSUM(shdr,val) ((shdr).Checksum = (val))

/* Define additional APID Qualifier macros. */

/* These macros will convert between local endianness and network endianness */
/* The packet headers are always in network byte order */
#define CCSDS_RD_EDS_VER(shdr)       ( ((shdr).APIDQSubsystem[0] & 0xF8) >> 3)
#define CCSDS_RD_ENDIAN(shdr)        ( ((shdr).APIDQSubsystem[0] & 0x04) >> 2)
#define CCSDS_RD_PLAYBACK(shdr)      ( ((shdr).APIDQSubsystem[0] & 0x02) >> 1)
#define CCSDS_RD_SUBSYSTEM_ID(shdr)  ( (((shdr).APIDQSubsystem[0] & 0x01) << 8) + ((shdr).APIDQSubsystem[1]))
#define CCSDS_RD_SYSTEM_ID(shdr)     ( ((shdr).APIDQSystemId[0] << 8) + ((shdr).APIDQSystemId[1]))

#define CCSDS_WR_EDS_VER(shdr,val)       ( (shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0x07) | (((val) & 0x1f) << 3) )
#define CCSDS_WR_ENDIAN(shdr,val)        ( (shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0xFB) | (((val) & 0x01) << 2) )
#define CCSDS_WR_PLAYBACK(shdr,val)      ( (shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0xFD) | (((val) & 0x01) << 1) )
                                  
#define CCSDS_WR_SUBSYSTEM_ID(shdr,val)  (((shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0xFE) | ((val & 0x0100) >> 8)) ,\
                                         ( (shdr).APIDQSubsystem[1] =  (val & 0x00ff)) )

#define CCSDS_WR_SYSTEM_ID(shdr,val)     (((shdr).APIDQSystemId[0] = ((val & 0xff00) >> 8)),\
                                         ( (shdr).APIDQSystemId[1] =  (val & 0x00ff)) )

/**********************************************************************
** Macros for clearing a CCSDS header to a standard initial state.  All
** of the macros are used in a similar way:
**   CCSDS_CLR_xxx_HDR(header)      -- Clear header of type xxx.
**********************************************************************/

/* Clear primary header. */
#define CCSDS_CLR_PRI_HDR(phdr)                     \
  (   (phdr).BaseHdr.VersionId = 0,                 \
      (phdr).BaseHdr.SecHdrFlags = 0,               \
      (phdr).BaseHdr.AppId = 0,                     \
      (phdr).BaseHdr.SeqFlag = CCSDS_INIT_SEQFLG,   \
      (phdr).BaseHdr.Sequence = CCSDS_INIT_SEQ,     \
      (phdr).BaseHdr.Length = 0                     )

#define CCSDS_CLR_SEC_APIDQ(shdr) \
  ( (shdr).APIDQSubsystem[0] = 0,\
    (shdr).APIDQSubsystem[1] = 0,\
    (shdr).APIDQSystemId[0] = 0,\
    (shdr).APIDQSystemId[1] = 0 )

/* Clear command secondary header. */
#define CCSDS_CLR_CMDSEC_HDR(shdr) \
  ( (shdr).FunctionCode = CCSDS_INIT_FC,\
    (shdr).Checksum = CCSDS_INIT_CHECKSUM )



#define CCSDS_WR_SEC_HDR_SEC(shdr, value)    ((shdr).Time.Seconds = value)
#define CCSDS_RD_SEC_HDR_SEC(shdr)           ((shdr).Time.Seconds)

/* Clear telemetry secondary header. */
#define CCSDS_CLR_TLMSEC_HDR(shdr) \
  ( (shdr).Time.Seconds = 0, (shdr).Time.Subseconds = 0 )

#define CCSDS_WR_SEC_HDR_SUBSEC(shdr, value) ((shdr).Time.Subseconds = value)

#define CCSDS_RD_SEC_HDR_SUBSEC(shdr)        ((shdr).Time.Subseconds)



/**********************************************************************
**   CCSDS_SID_xxx(sid)          -- Extract field xxx from sid. (REMOVED)
**
** NOTE in the EDS version these macros are obsolete.  Application code 
** should only ever deal with message IDs, for which there is an extraction
** function.  No assumption should be made about the presence of what
** was previously known as "stream ID" -- it never really existed anyway.
**
**********************************************************************/

/**********************************************************************
** Macros for frequently used combinations of operations.
**
**   CCSDS_INC_SEQ(phdr)           -- Increment sequence count.
**********************************************************************/

/* Increment sequence count in primary header by 1. */
#define CCSDS_INC_SEQ(phdr) \
   CCSDS_WR_SEQ(phdr, (CCSDS_RD_SEQ(phdr)+1))


/*********************************************************************/

/*
** Exported Functions
*/


/******************************************************************************
**  Function:  CCSDS_LoadCheckSum()
**
**  Purpose:
**    Compute and load a checksum for a CCSDS command packet that has a
**    secondary header.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.  The checksum field in the packet
**               will be modified.
**
**  Return:
**    (none)
*/

void CCSDS_LoadCheckSum (CCSDS_CommandPacket_t *PktPtr);

/******************************************************************************
**  Function:  CCSDS_ValidCheckSum()
**
**  Purpose:
**    Determine whether a checksum in a command packet is valid.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    true if checksum of packet is valid; false if not.
**    A valid checksum is 0.
*/

bool CCSDS_ValidCheckSum (CCSDS_CommandPacket_t *PktPtr);

/******************************************************************************
**  Function:  CCSDS_ComputeCheckSum()
**
**  Purpose:
**    Compute the checksum for a command packet.  The checksum is the XOR of
**    all bytes in the packet; a valid checksum is zero.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    true if checksum of packet is valid; false if not.
*/

uint8 CCSDS_ComputeCheckSum (CCSDS_CommandPacket_t *PktPtr);


#endif  /* _ccsds_ */
/*****************************************************************************/
