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
** File: cfe_sb_eds_db.c
**
** Purpose:
**      This file is part of the CFE Electronic Data Sheets (EDS) integration
**      It instantiates an EDS database object for the runtime code.  It is
**      a global resource but mainly utilized by the software code (SB).
**
**
******************************************************************************/

/*
** Include Files
*/
#include <stdlib.h>
#include <string.h>

#include <target_config.h>
#include <cfe.h>
#include "cfe_sb_priv.h"
#include "cfe_sb_eds_db.h"
#include "edslib_api_types.h"
#include "edslib_datatypedb.h"
#include "cfe_mission_eds_parameters.h"
#include "cfe_mission_eds_interface_parameters.h"

#define CFE_SB_EDS_APP_MARKER   0x00100000
#define CFE_SB_EDS_LIB_MARKER   0x00200000

/**
 * Instantiate an EDS mission object that brings all elements together
 *
 * Contains the global structures required to interact with EdsLib
 *
 * Also contains registration table mapping CFE AppIds to EDS structures -
 * This is primarily used for unregistration/cleanup, so we can probably unregister
 * the EDS structures associated with an AppID when the app exits.
 */
static struct
{
    uint32 RegAppId[EDS_MAX_DATASHEETS];
} CFE_EDS_Global;



/******************************************************************************
**  Function:  CFE_SB_GetEds()
**
**  Purpose:
**    Returns a pointer to the runtime EDS object.  This can be used
**    by other entities that need to make EdsLib API calls.
**
**  Arguments:
**    None
**
**  Return:
**    Pointer to the runtime EDS database object
*/
CFE_SB_EDS_GlobalDbPtr_t CFE_SB_GetEds(void)
{
    return GLOBAL_CONFIGDATA.EdsDb;
}

/******************************************************************************
**  Function:  CFE_SB_EDS_DoRegister()
**
**  Purpose:
**    Register a CFE Application DB object into the global database
**
**  Arguments:
**    RegTag - Unique Identifier to associate with the entry
**    AppDictionary - the loaded application dictionary
**
**  Return:
**    None
*/
static void CFE_SB_EDS_DoRegister(uint32 RegTag, CFE_SB_EDS_AppDbPtr_t AppDictionary)
{
    uint16 DictionaryIdx;
    int32_t Status;

    /* Obtain the static (compile-time) EDS ID value from the DB object.
     * Note this is NOT the same as the CFE ES AppID */
    DictionaryIdx = EdsLib_DataTypeDB_GetAppIdx(AppDictionary);

    CFE_SB_LockSharedData(__func__, __LINE__);

    if (CFE_EDS_Global.RegAppId[DictionaryIdx] != 0)
    {
        /*
         * only the first registration counts, subsequent
         * registrations are ignored (but this could be a bug)
         */
        Status = EDSLIB_FAILURE;
    }
    else if (GLOBAL_CONFIGDATA.DynamicEdsDb != NULL)
    {
        Status = EdsLib_DataTypeDB_Register(GLOBAL_CONFIGDATA.DynamicEdsDb, AppDictionary);
    }
    else
    {
        /*
         * no registration needed, but still track the
         * app as the "owner" of this EDS object.
         */
        Status = EDSLIB_SUCCESS;
    }

    if (Status == EDSLIB_SUCCESS)
    {
        CFE_EDS_Global.RegAppId[DictionaryIdx] = RegTag;
    }

    CFE_SB_UnlockSharedData(__func__,__LINE__);
}

/******************************************************************************
**  Function:  CFE_SB_EDS_DoUnregister()
**
**  Purpose:
**    Remove a CFE Application message dictionary from the global structure
**
**  Arguments:
**    RegTag - Identifier from previous registration call
**
**  Return:
**    None
*/
static void CFE_SB_EDS_DoUnregister(uint32 RegTag)
{
    uint16 DictionaryIdx;
    int32_t Status;

    CFE_SB_LockSharedData(__func__, __LINE__);

    for (DictionaryIdx = 0; DictionaryIdx < EDS_MAX_DATASHEETS; ++DictionaryIdx)
    {
        if (CFE_EDS_Global.RegAppId[DictionaryIdx] != RegTag)
        {
            /* object not associated with this app/lib */
            Status = EDSLIB_FAILURE;
        }
        else if (GLOBAL_CONFIGDATA.DynamicEdsDb != NULL)
        {
            /* do actual de-registration */
            Status = EdsLib_DataTypeDB_Unregister(GLOBAL_CONFIGDATA.DynamicEdsDb, DictionaryIdx);
        }
        else
        {
            /* not dynamically linked */
            Status = EDSLIB_SUCCESS;
        }

        if (Status == EDSLIB_SUCCESS)
        {
            CFE_EDS_Global.RegAppId[DictionaryIdx] = 0;
        }
    }

    CFE_SB_UnlockSharedData(__func__,__LINE__);
}

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterSelf()
**
**  Purpose:
**    Register a CFE Application message dictionary into the global structure
**
**  Arguments:
**    AppDictionary - the loaded application dictionary
**
**  Return:
**    None
*/
void CFE_SB_EDS_RegisterSelf(CFE_SB_EDS_AppDbPtr_t AppEds)
{
    uint32 AppId;

    if (CFE_ES_GetAppID(&AppId) == CFE_SUCCESS)
    {
        CFE_SB_EDS_RegisterApp(AppId, AppEds);
    }
}

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterApp()
**
**  Purpose:
**    Register a CFE Application message dictionary into the global structure
**
**  Arguments:
**    AppDictionary - the loaded application dictionary
**
**  Return:
**    None
*/
void CFE_SB_EDS_RegisterApp(uint32 AppId, CFE_SB_EDS_AppDbPtr_t AppEds)
{
   if (AppId >= 0 && AppId < CFE_SB_EDS_APP_MARKER)
   {
       CFE_SB_EDS_DoRegister(AppId | CFE_SB_EDS_APP_MARKER, AppEds);
   }
}

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterLib()
**
**  Purpose:
**    Register a CFE Library message dictionary into the global structure
**
**  Arguments:
**    AppDictionary - the loaded application dictionary
**
**  Return:
**    None
*/
void CFE_SB_EDS_RegisterLib(uint32 LibId, CFE_SB_EDS_AppDbPtr_t AppEds)
{
   if (LibId >= 0 && LibId < CFE_SB_EDS_LIB_MARKER)
   {
       CFE_SB_EDS_DoRegister(LibId | CFE_SB_EDS_LIB_MARKER, AppEds);
   }
}

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterSelf()
**
**  Purpose:
**    Remove a CFE Application message dictionary from the global structure
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_EDS_UnregisterApp(uint32 AppId)
{
    if (AppId >= 0 && AppId < CFE_SB_EDS_APP_MARKER)
    {
        CFE_SB_EDS_DoUnregister(AppId | CFE_SB_EDS_APP_MARKER);
    }
}

/******************************************************************************
**  Function:  CFE_SB_EDS_UnregisterLib()
**
**  Purpose:
**    Remove a CFE Library message dictionary from the global structure
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_EDS_UnregisterLib(uint32 LibId)
{
    if (LibId >= 0 && LibId < CFE_SB_EDS_LIB_MARKER)
    {
        CFE_SB_EDS_DoUnregister(LibId | CFE_SB_EDS_LIB_MARKER);
    }
}

/******************************************************************************
**  Function:  CFE_SB_EDS_RegisterSelf()
**
**  Purpose:
**    Remove a CFE Application message dictionary from the global structure
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_EDS_UnregisterSelf(void)
{
    uint32 AppId;

    if (CFE_ES_GetAppID(&AppId) == CFE_SUCCESS)
    {
        CFE_SB_EDS_UnregisterApp(AppId);
    }
}

/*
 * Global EDS object registration.
 * These objects cannot be unregistered.
 */
void CFE_SB_EDS_RegisterGlobal(CFE_SB_EDS_AppDbPtr_t AppEds)
{
    CFE_SB_EDS_DoRegister(0xFFFFFFFF, AppEds);
}

/*****************************************************************************/
