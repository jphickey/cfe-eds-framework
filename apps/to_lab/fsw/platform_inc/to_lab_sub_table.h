/************************************************************************
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
** File: to_sub_table.h
**
** Purpose:
**  Define TO Lab CPU specific subscription table
**
** Notes:
**
*************************************************************************/

/*
** Add the proper include file for the message IDs below
*/
#include <common_types.h>
#include "cfe_mission_cfg.h"
#include "cfe_mission_eds_parameters.h"

typedef struct
{
    uint16       TopicId;
    CFE_SB_Qos_t Qos;
    uint16       BufLimit;
} TO_subscription_t;

static TO_subscription_t TO_SubTable[] = {
    /* CFS App Subscriptions */
    {CFE_MISSION_TO_LAB_HK_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_TO_LAB_DATA_TYPES_TOPICID, {0, 0}, 4},
    {CFE_MISSION_CI_LAB_HK_TLM_TOPICID, {0, 0}, 4},

    /* cFE Core subsciptions */
    {CFE_MISSION_ES_HK_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_EVS_HK_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_SB_HK_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_TBL_HK_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_TIME_HK_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_TIME_DIAG_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_SB_STATS_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_TBL_REG_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_EVS_LONG_EVENT_MSG_TOPICID, {0, 0}, 32},
    {CFE_MISSION_ES_SHELL_TLM_TOPICID, {0, 0}, 32},
    {CFE_MISSION_ES_APP_TLM_TOPICID, {0, 0}, 4},
    {CFE_MISSION_ES_MEMSTATS_TLM_TOPICID, {0, 0}, 4},

    {TO_UNUSED, {0, 0}, 0},
    {TO_UNUSED, {0, 0}, 0},
    {TO_UNUSED, {0, 0}, 0}};

/************************
** End of File Comment **
************************/
