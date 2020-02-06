/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
** File: cfe_msgids.h
**
** Purpose:
**   This header file contains the Message Id's for messages used by the
**   cFE core.
**
**
******************************************************************************/
#ifndef _cfe_msgids_
#define _cfe_msgids_

/*
** Includes
*/
#include <common_types.h>
#include "cfe_mission_cfg.h"
#include "cfe_mission_eds_parameters.h"

/*
** cFE Message Id macro definitions
**
** These definitions are for backward compatibility with code that subscribes to messages
** using a CPU-specific definition (i.e. XXX_MID).  Unfortunately, if one were to hard-code
** that macro definition with a value, the resulting code would only run on that specific CPU number,
** requiring a _different_ macro definition for each CPU.  Instead, this macro is now mapped to a lookup
** function that will resolve the proper message ID depending on the runtime CPU number.
*/

/*
** cFE CPU-specific command message Id's
*/
#define CFE_EVS_CMD_MID         CFE_SB_MsgId_From_TopicId(CFE_MISSION_EVS_CMD_TOPICID)
#define CFE_SB_CMD_MID          CFE_SB_MsgId_From_TopicId(CFE_MISSION_SB_CMD_TOPICID)
#define CFE_TBL_CMD_MID         CFE_SB_MsgId_From_TopicId(CFE_MISSION_TBL_CMD_TOPICID)
#define CFE_TIME_CMD_MID        CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_CMD_TOPICID)
#define CFE_ES_CMD_MID          CFE_SB_MsgId_From_TopicId(CFE_MISSION_ES_CMD_TOPICID)
#define CFE_ES_SEND_HK_MID      CFE_SB_MsgId_From_TopicId(CFE_MISSION_ES_SEND_HK_TOPICID)
#define CFE_EVS_SEND_HK_MID     CFE_SB_MsgId_From_TopicId(CFE_MISSION_EVS_SEND_HK_TOPICID)
#define CFE_SB_SEND_HK_MID      CFE_SB_MsgId_From_TopicId(CFE_MISSION_SB_SEND_HK_TOPICID)
#define CFE_TBL_SEND_HK_MID     CFE_SB_MsgId_From_TopicId(CFE_MISSION_TBL_SEND_HK_TOPICID)
#define CFE_TIME_SEND_HK_MID    CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_SEND_HK_TOPICID)
#define CFE_TIME_TONE_CMD_MID   CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_TONE_CMD_TOPICID)
#define CFE_TIME_1HZ_CMD_MID    CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_ONEHZ_CMD_TOPICID)

/*
** cFE Global Command Message Id's
*/
#define CFE_TIME_DATA_CMD_MID   CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_DATA_CMD_TOPICID)
#define CFE_TIME_SEND_CMD_MID   CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_SEND_CMD_TOPICID)

/*
** CFE Telemetry Message Id's
*/
#define CFE_ES_HK_TLM_MID       CFE_SB_MsgId_From_TopicId(CFE_MISSION_ES_HK_TLM_TOPICID)
#define CFE_EVS_HK_TLM_MID      CFE_SB_MsgId_From_TopicId(CFE_MISSION_EVS_HK_TLM_TOPICID)
#define CFE_SB_HK_TLM_MID       CFE_SB_MsgId_From_TopicId(CFE_MISSION_SB_HK_TLM_TOPICID)
#define CFE_TBL_HK_TLM_MID      CFE_SB_MsgId_From_TopicId(CFE_MISSION_TBL_HK_TLM_TOPICID)
#define CFE_TIME_HK_TLM_MID     CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_HK_TLM_TOPICID)
#define CFE_TIME_DIAG_TLM_MID   CFE_SB_MsgId_From_TopicId(CFE_MISSION_TIME_DIAG_TLM_TOPICID)
#define CFE_EVS_LONG_EVENT_MSG_MID  CFE_SB_MsgId_From_TopicId(CFE_MISSION_EVS_LONG_EVENT_MSG_TOPICID)
#define CFE_EVS_SHORT_EVENT_MSG_MID CFE_SB_MsgId_From_TopicId(CFE_MISSION_EVS_SHORT_EVENT_MSG_TOPICID)
#define CFE_SB_STATS_TLM_MID    CFE_SB_MsgId_From_TopicId(CFE_MISSION_SB_STATS_TLM_TOPICID)
#define CFE_ES_APP_TLM_MID      CFE_SB_MsgId_From_TopicId(CFE_MISSION_ES_APP_TLM_TOPICID)
#define CFE_TBL_REG_TLM_MID     CFE_SB_MsgId_From_TopicId(CFE_MISSION_TBL_REG_TLM_TOPICID)
#define CFE_SB_ALLSUBS_TLM_MID  CFE_SB_MsgId_From_TopicId(CFE_MISSION_SB_ALLSUBS_TLM_TOPICID)
#define CFE_SB_ONESUB_TLM_MID   CFE_SB_MsgId_From_TopicId(CFE_MISSION_SB_ONESUB_TLM_TOPICID)
#define CFE_ES_SHELL_TLM_MID    CFE_SB_MsgId_From_TopicId(CFE_MISSION_ES_SHELL_TLM_TOPICID)
#define CFE_ES_MEMSTATS_TLM_MID CFE_SB_MsgId_From_TopicId(CFE_MISSION_ES_MEMSTATS_TLM_TOPICID)


#endif
