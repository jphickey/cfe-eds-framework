/*
 *  Copyright (c) 2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 */

/**
 * \file iodriver_impl.h
 *
 *  Created on: Oct 5, 2015
 *  Created by: joseph.p.hickey@nasa.gov
 *
 */

#ifndef _IODRIVER_IMPL_H_
#define _IODRIVER_IMPL_H_

#ifndef _CFE_PSP_
#error "Do not include this file from outside the PSP"
#endif

#include "cfe_psp_module.h"
#include "iodriver_base.h"

/**
 * Macro to declare the global object for an IO device driver
 */
#define CFE_PSP_MODULE_DECLARE_IODEVICEDRIVER(name)                                     \
    static void name##_Init(uint32 PspModuleId);                                        \
    CFE_PSP_ModuleApi_t CFE_PSP_##name##_API =                                          \
    {                                                                                   \
        .ModuleType = CFE_PSP_MODULE_TYPE_DEVICEDRIVER,                                 \
        .OperationFlags = 0,                                                            \
        .Init = name##_Init,                                                            \
        .ExtendedApi = &name##_DevApi,                                                  \
    }


/**
 * Prototype for a basic device command function
 * Implemented as a single API call with an extendible command code for device-specific ops.  This allows
 * a common API to be used while still allowing full freedom to handle many different device types.
 */
typedef int32 (*IODriver_ApiFunc_t)(uint32 CommandCode, uint16 Instance, uint16 SubChannel, IODriver_Arg_t arg);

typedef const struct
{
    IODriver_ApiFunc_t DeviceCommand;
    IODriver_ApiFunc_t DeviceMutex;
} IODriver_API_t;


uint32 IODriver_GetMutex(uint32 PspModuleId, int32 DeviceHash);
int32 IODriver_HashMutex(int32 StartHash, int32 Datum);

#endif      /* _IODRIVER_IMPL_H_ */

