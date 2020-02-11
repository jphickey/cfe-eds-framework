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
** File   : cfe_sb_eds_db.h
**
** Author : Joe Hickey
**
** Purpose:
**      API for the Electronic Data Sheets (EDS) runtime database registration
**
**      Provides functions such that CFS apps may add/remove/query the EDS Database
**      maintained by the Software Bus core app.
*/

#ifndef _CFE_SB_EDS_DB_H
#define _CFE_SB_EDS_DB_H

#include <common_types.h>

/**
 * An abstract type for the Basic EDS for a single application.
 *
 * Any API calls defined in CFE SB will use this abstract type, so that
 * applications do not need to reference the real definition inside the EDS library.
 *
 * This helps make the compiler include path simpler for CFS apps.
 */
typedef const struct EdsLib_App_DataTypeDB *CFE_SB_EDS_AppDbPtr_t;

/**
 * An abstract type for the global EDS DB object (all applications).
 *
 * Any API calls defined in CFE SB will use this abstract type, so that
 * applications do not need to reference the real definition inside the EDS library.
 *
 * This helps make the compiler include path simpler for CFS apps.
 */
typedef const struct EdsLib_DatabaseObject *CFE_SB_EDS_GlobalDbPtr_t;


/******************************************************************************
**  Function:  CFE_SB_GetEds()
*/
/**
 * Obtain the global CFE EDS database object
 *
 * This accessor can be used by code that needs to make direct calls into EdsLib
 * against the active CFE EDS database.
 */
CFE_SB_EDS_GlobalDbPtr_t CFE_SB_GetEds(void);

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterSelf()
*/
/**
 * Registers an application with the software bus EDS registry.
 *
 * This allows OTHER applications running within the same CFE core to interpret
 * the messages defined by this application's EDS.
 *
 * Typically an application would have a global EDS object linked with the module
 * itself, so the EDS is loaded when the application is loaded.  The application
 * should call this function during initialization to register that app-specific
 * EDS object into the global runtime table so other applications can use it.
 *
 * Using this API, the EDS will be registered under the CFE ES AppID of the
 * calling application.
 *
 * \param AppEds        The Basic EDS object associated with this application.
 *
 * \sa CFE_SB_EDS_RegisterApp()
 */
void CFE_SB_EDS_RegisterSelf(CFE_SB_EDS_AppDbPtr_t AppEds);

/******************************************************************************
**  Function:  CFE_SB_EDS_UnregisterApp()
*/
/**
 * Unregisters the calling application with the software bus EDS registry.
 *
 * Inverse of CFE_SB_EDS_RegisterSelf()
 *
 * \sa CFE_SB_EDS_RegisterSelf()
 */
void CFE_SB_EDS_UnregisterSelf(void);

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterApp()
*/
/**
 * Registers an arbitrary application with the software bus EDS registry.
 *
 * This allows OTHER applications running within the same CFE core to interpret
 * the messages defined by another application's EDS.
 *
 * This function allows the caller to specify the CFE ES AppID associated with
 * the AppEds parameter, so it can be used by a "third party" application to load
 * an EDS object for a different application.  This may be required if the EDS
 * object is loaded separately from the main application module.
 *
 * \param AppId         The runtime AppID assigned by CFE ES for this application
 * \param AppEds        The Basic EDS object associated with that application.
 *
 * \sa CFE_SB_EDS_RegisterSelf()
 */
void CFE_SB_EDS_RegisterApp(uint32 AppId, CFE_SB_EDS_AppDbPtr_t AppEds);

/******************************************************************************
**  Function:  CFE_SB_EDS_UnregisterApp()
*/
/**
 * Unregisters the any application with the software bus EDS registry.
 *
 * Inverse of CFE_SB_EDS_RegisterApp()
 *
 * \param AppId         Numeric identifier from the CFE_SB_EDS_RegisterApp() call.
 *
 * \sa CFE_SB_EDS_RegisterApp()
 */
void CFE_SB_EDS_UnregisterApp(uint32 AppId);

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterLib()
*/
/**
 * Registers an EDS object contained within a CFE Library
 * The CFE Library ID is required in case the the library is unloaded in the future,
 * the EDS object must also be de-registered in this case.
 *
 * Inverse of CFE_SB_EDS_UnregisterLib()
 *
 * \param LibId     A unique numeric identifier.
 *        Should be the value passed by Executive Services (ES) to the library
 *        init function at the time the library was loaded.
 *
 * \param AppEds    Actual EDS object to register
 *
 *
 * \sa CFE_SB_EDS_RegisterApp()
 */
void CFE_SB_EDS_RegisterLib(uint32 LibId, CFE_SB_EDS_AppDbPtr_t AppEds);

/******************************************************************************
**  Function:  CFE_SB_EDS_UnregisterLib()
*/
/**
 * Unregisters an EDS object contained within a CFE Library
 * All EDS objects associated with the CFE library are detached from the global DB.
 *
 * Inverse of CFE_SB_EDS_RegisterLib()
 *
 * \param LibId         Numeric identifier from the CFE_SB_EDS_RegisterLib() call.
 *
 * \sa CFE_SB_EDS_RegisterLib()
 */
void CFE_SB_EDS_UnregisterLib(uint32 LibId);


/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterGlobal()
*/
/**
 * A registration function for system-wide EDS objects.
 *
 * This routine should only be called by core apps during system startup.
 * These objects cannot be unregistered, so they must not reside in dynamic objects.
 *
 * \sa CFE_SB_EDS_RegisterApp(), CFE_SB_EDS_RegisterLib()
 */
void CFE_SB_EDS_RegisterGlobal(CFE_SB_EDS_AppDbPtr_t AppEds);

#endif /* _CFE_SB_EDS_DB_H */

