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
** File: sch_lab_sched_tab.h
**
** Purpose: 
**  This file contains the schedule tables for the SCH Lab app.
**  It is intended to go in the platform include directory so the SCH_LAB
**  app source code does not have to be modified.
**
** Notes:
**
*************************************************************************/


#include "sch_lab_eds_typedefs.h"
#include "cfe_msgids.h"

/*
** Include headers for message IDs here
*/
#include "cfe_mission_eds_parameters.h"



/*
** Defines
*/
#define SCH_LAB_END_OF_TABLE          0
#define SCH_LAB_MAX_SCHEDULE_ENTRIES  CFE_MISSION_SCH_LAB_MAX_SCHEDULE_ENTRIES
#define SCH_TBL_DEFAULT_FILE          "/cf/sch_lab_table.tbl"


