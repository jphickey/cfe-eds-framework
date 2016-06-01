/*
 *  Copyright (c) 2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 */

/**
 * \file cfe_psp_iodriver.c
 *
 *  Created on: Sep 29, 2015
 *  Created by: joseph.p.hickey@nasa.gov
 *
 */

#include "cfe_psp_module.h"
#include "iodriver_base.h"
#include "iodriver_impl.h"

#define IODRIVER_LOCK_TABLE_SIZE        7

CFE_PSP_MODULE_DECLARE_SIMPLE(iodriver);

static uint32 IODriver_Mutex_Table[IODRIVER_LOCK_TABLE_SIZE];

const IODriver_API_t IODRIVER_DEFAULT_API =
{
        .DeviceCommand = NULL,
        .DeviceMutex = NULL
};

void iodriver_Init(uint32 PspModuleId)
{
    uint32 i;
    char TempName[OS_MAX_PATH_LEN];

    for (i = 0; i < IODRIVER_LOCK_TABLE_SIZE; ++i)
    {
        snprintf(TempName, sizeof(TempName), "DriverMutex-%02u", (unsigned int)(i + 1));
        OS_MutSemCreate(&IODriver_Mutex_Table[i], TempName, 0);
    }
}

IODriver_API_t *IODriver_GetAPI(uint32 PspModuleId)
{
    int32 Result;
    CFE_PSP_ModuleApi_t *API;
    IODriver_API_t *IODriver_API;

    Result = CFE_PSP_Module_GetAPIEntry(PspModuleId, &API);
    if (Result == CFE_PSP_SUCCESS && API->ModuleType == CFE_PSP_MODULE_TYPE_DEVICEDRIVER)
    {
        IODriver_API = (const IODriver_API_t *)API->ExtendedApi;
    }
    else
    {
        IODriver_API = &IODRIVER_DEFAULT_API;
    }

    return IODriver_API;
}

/**
 * Based on a board type and instance, determine which mutex needs to be acquired.
 * Allows some degree of parallelization by (most likely) not blocking requests to a different boards
 * that come in concurrently, but also ensuring that requests to the _same_ board will be serialized.
 * The computation here seemed to produce a decent-enough spread across the mutex table without
 * overlaps (at least with the current set of hardware)
 */
uint32 IODriver_GetMutex(uint32 PspModuleId, int32 DeviceHash)
{
    uint32 LookupId;

    if (DeviceHash < 0)
    {
        /* No locking required */
        LookupId = 0;
    }
    else
    {
        LookupId = (uint32)DeviceHash;
        if (PspModuleId != 0)
        {
            LookupId ^= PspModuleId;
        }
        LookupId = IODriver_Mutex_Table[LookupId % IODRIVER_LOCK_TABLE_SIZE];
    }

    return LookupId;
}

int32 IODriver_HashMutex(int32 StartHash, int32 Datum)
{
    Datum *= 0x10AB;
    return ((StartHash + Datum) & 0x7FFFFFFF);
}

int32 IODriver_Command(const IODriver_Location_t *Location, uint32 CommandCode, IODriver_Arg_t Arg)
{
    int32 Result;
    uint32 MutexId;
    IODriver_API_t *API;

    API = IODriver_GetAPI(Location->PspModuleId);
    if (API->DeviceCommand != NULL)
    {
        if (API->DeviceMutex != NULL)
        {
            MutexId = IODriver_GetMutex(Location->PspModuleId, API->DeviceMutex(CommandCode, Location->BoardInstance, Location->ChannelNumber, Arg));
        }
        else
        {
            MutexId = 0;
        }
        if (MutexId != 0)
        {
            OS_MutSemTake(MutexId);
        }
        Result = API->DeviceCommand(CommandCode, Location->BoardInstance, Location->ChannelNumber, Arg);
        if (MutexId != 0)
        {
            OS_MutSemGive(MutexId);
        }
    }
    else
    {
        /* No command function defined - this is a driver implementation error */
        Result = CFE_PSP_ERROR_NOT_IMPLEMENTED;
    }

    return Result;
}

int32 IODriver_FindByName(const char *DriverName, uint32 *PspModuleId)
{
    int32 Result;
    CFE_PSP_ModuleApi_t *API;

    Result = CFE_PSP_Module_FindByName(DriverName, PspModuleId);
    if (Result == CFE_PSP_SUCCESS)
    {
        /*
         * Check that the module is actually registered as a device driver,
         * if not then return CFE_PSP_INVALID_MODULE_NAME instead of SUCCESS
         */
        Result = CFE_PSP_Module_GetAPIEntry(*PspModuleId, &API);
        if (Result == CFE_PSP_SUCCESS && API->ModuleType != CFE_PSP_MODULE_TYPE_DEVICEDRIVER)
        {
            Result = CFE_PSP_INVALID_MODULE_NAME;
        }
    }

    return Result;
}



