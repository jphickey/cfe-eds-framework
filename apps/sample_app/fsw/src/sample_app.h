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
** File: sample_app.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE application.
**
**
*******************************************************************************/

#ifndef _sample_app_h_
#define _sample_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "sample_app_perfids.h"
#include "sample_app_msgids.h"

/* Use the EDS generated type definitions */
#include "sample_app_eds_defines.h"
#include "sample_app_eds_typedefs.h"


/***********************************************************************/
#define SAMPLE_PIPE_DEPTH                     32 /* Depth of the Command Pipe for Application */

#define SAMPLE_NUMBER_OF_TABLES               1  /* Number of Table(s) */

/* Define filenames of default data images for tables */
#define SAMPLE_APP_TABLE_FILE                 "/cf/sample_app_tbl.tbl"

#define SAMPLE_APP_TABLE_OUT_OF_RANGE_ERR_CODE  -1

#define SAMPLE_APP_TBL_ELEMENT_1_MAX              10
/************************************************************************
** Type Definitions
*************************************************************************/

/*
 * Buffer to hold telemetry data prior to sending
 * Defined as a union to ensure proper alignment for a CFE_SB_Msg_t type
 */
typedef union
{
    CFE_SB_Msg_t        MsgHdr;
    SAMPLE_APP_HkTlm_t  HkTlm;
} SAMPLE_HkBuffer_t;

/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8                 CmdCounter;
    uint8                 ErrCounter;

    /*
    ** Housekeeping telemetry packet...
    */
    SAMPLE_HkBuffer_t     HkBuf;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t    CommandPipe;
    CFE_SB_MsgPtr_t    MsgPtr;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char     PipeName[16];
    uint16   PipeDepth;

    CFE_EVS_BinFilter_t  EventFilters[SAMPLE_EVENT_COUNTS];
    CFE_TBL_Handle_t     TblHandles[SAMPLE_NUMBER_OF_TABLES];

} SAMPLE_AppData_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (SAMPLE_AppMain), these
**       functions are not called from any other source module.
*/
void  SAMPLE_AppMain(void);
int32 SAMPLE_AppInit(void);
int32 SAMPLE_ReportHousekeeping(const SAMPLE_APP_SendHkCommand_t *Msg);
int32 SAMPLE_ResetCounters(const SAMPLE_APP_ResetCounters_t *Msg);
int32 SAMPLE_Process(const SAMPLE_APP_Process_t *Msg);
int32 SAMPLE_Noop(const SAMPLE_APP_Noop_t *Msg);
int32 SAMPLE_DoExample(const SAMPLE_APP_DoExample_t *Msg);
void  SAMPLE_GetCrc(const char *TableName);

int32 SAMPLE_TblValidationFunc(void *TblData);

#endif /* _sample_app_h_ */
