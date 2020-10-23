/************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2002 United States Government as represented by
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
** File: to_lab_sub_table.c
**
** Purpose:
**  Define TO Lab CPU specific subscription table
**
** Notes:
**
*************************************************************************/

#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "to_lab_sub_table.h"
#include "to_lab_app.h"

/*
** Add the proper include file for the message IDs below
*/

/*
** Common CFS app includes below are commented out
*/
#include "to_lab_msgids.h"
#include "ci_lab_msgids.h"

#include "sample_app_msgids.h"

#if 0
#include "hs_msgids.h"
#include "fm_msgids.h"
#include "sc_msgids.h"
#include "ds_msgids.h"
#include "lc_msgids.h"
#endif

TO_LAB_Subs_t TO_LAB_Subs =
{
    .Subs =
    {
        /* CFS App Subscriptions */
        {CFE_MISSION_TO_LAB_HK_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_TO_LAB_DATA_TYPES_TOPICID, {0, 0}, 4},
        {CFE_MISSION_CI_LAB_HK_TLM_TOPICID, {0, 0}, 4},

        /* cFE Core subscriptions */
        {CFE_MISSION_ES_HK_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_EVS_HK_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_SB_HK_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_TBL_HK_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_TIME_HK_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_TIME_DIAG_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_SB_STATS_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_TBL_REG_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_EVS_LONG_EVENT_MSG_TOPICID, {0, 0}, 32},
        {CFE_MISSION_ES_APP_TLM_TOPICID, {0, 0}, 4},
        {CFE_MISSION_ES_MEMSTATS_TLM_TOPICID, {0, 0}, 4},

        /* TO_UNUSED entry to mark the end of valid MsgIds */
        {TO_UNUSED, {0, 0}, 0}
    }
};

CFE_TBL_FILEDEF(TO_LAB_Subs, "TO_LAB_APP.TO_LAB_Subs", "TO Lab Sub Tbl", "to_lab_sub.tbl", "TO_LAB/Subs")
