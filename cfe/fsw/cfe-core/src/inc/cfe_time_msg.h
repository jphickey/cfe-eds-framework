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
** File: cfe_time_msg.h
**
** Purpose:  cFE Time Services (TIME) SB message definitions header file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_time_msg_
#define _cfe_time_msg_

/*
** Required header files...
*/
#include "cfe_time_extern_typedefs.h"


/*
** Clock "state flag" definitions (state data converted to flags)...
*/
/** \defgroup CFETIMEClkStates cFE Clock State Flag Defines
 * \{
 */
#define CFE_TIME_FLAG_CLKSET     (0x8000 >> CFE_TIME_FlagBit_CLKSET)   /**< \brief The spacecraft time has been set */
#define CFE_TIME_FLAG_FLYING     (0x8000 >> CFE_TIME_FlagBit_FLYING)   /**< \brief This instance of Time Services is flywheeling */
#define CFE_TIME_FLAG_SRCINT     (0x8000 >> CFE_TIME_FlagBit_SRCINT)   /**< \brief The clock source is set to "internal" */
#define CFE_TIME_FLAG_SIGPRI     (0x8000 >> CFE_TIME_FlagBit_SIGPRI)   /**< \brief The clock signal is set to "primary" */
#define CFE_TIME_FLAG_SRVFLY     (0x8000 >> CFE_TIME_FlagBit_SRVFLY)   /**< \brief The Time Server is in flywheel mode */
#define CFE_TIME_FLAG_CMDFLY     (0x8000 >> CFE_TIME_FlagBit_CMDFLY)   /**< \brief This instance of Time Services was commanded into flywheel mode */
#define CFE_TIME_FLAG_ADDADJ     (0x8000 >> CFE_TIME_FlagBit_ADDADJ)   /**< \brief One time STCF Adjustment is to be done in positive direction */
#define CFE_TIME_FLAG_ADD1HZ     (0x8000 >> CFE_TIME_FlagBit_ADD1HZ)   /**< \brief 1 Hz STCF Adjustment is to be done in a positive direction */
#define CFE_TIME_FLAG_ADDTCL     (0x8000 >> CFE_TIME_FlagBit_ADDTCL)   /**< \brief Time Client Latency is applied in a positive direction */
#define CFE_TIME_FLAG_SERVER     (0x8000 >> CFE_TIME_FlagBit_SERVER)   /**< \brief This instance of Time Services is a Time Server */
#define CFE_TIME_FLAG_GDTONE     (0x8000 >> CFE_TIME_FlagBit_GDTONE)   /**< \brief The tone received is good compared to the last tone received */
/** \} */


/*************************************************************************/

#endif /* _cfe_time_msg_ */

/************************/
/*  End of File Comment */
/************************/
