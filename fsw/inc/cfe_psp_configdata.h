/**
 * \file cfe_psp_config.h
 *
 *  Created on: Dec 31, 2014
 *      Author: joseph.p.hickey@nasa.gov
 *
 *  Copyright (c) 2004-2014, United States government as represented by the 
 *  administrator of the National Aeronautics Space Administration.  
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 *
 *  This is governed by the NASA Open Source Agreement and may be used, 
 *  distributed and modified only according to the terms of that agreement.
 */

#ifndef CFE_PSP_CONFIG_H_
#define CFE_PSP_CONFIG_H_

/* osapi.h is required for the definition of OS_VolumeInto_t */
#include <osapi.h>
/* cfe_psp.h is required for the definition of CFE_PSP_MemTable_t */
#include <cfe_psp.h>

typedef const struct CFE_PSP_IoDriverApi CFE_PSP_IoDriverApi_t;

typedef const struct
{
   const char *Name;
   CFE_PSP_IoDriverApi_t *Api;
} CFE_PSP_IoDriverLoadEntry_t;

/**
 * PSP/Hardware configuration parameters
 * This structure should be instantiated by the PSP according
 * such that other modules do not need to directly include
 * the PSP configuration at compile time.
 */
typedef const struct
{
   uint32 PSP_WatchdogMin;                /**< PSP Minimum watchdog in milliseconds */
   uint32 PSP_WatchdogMax;                /**< PSP Maximum watchdog in milliseconds */
   CFE_PSP_IoDriverLoadEntry_t *PSP_DriverList; /**< List of drivers (API structures) statically linked into PSP */
   uint32 PSP_MemTableSize;               /**< Size of PSP memory table */
   CFE_PSP_MemTable_t *PSP_MemoryTable;   /**< Pointer to PSP memory table (forward reference) */

   uint32 OS_VolumeTableSize;             /**< Size of OS volume table */
   OS_VolumeInfo_t *OS_VolumeTable;       /**< Pointer to OS volume table (forward reference) */

   /**
    * Processor Context type.
    * This is needed to determine the size of the context entry in the ER log.
    * It is a placeholder as the implementation to use it is not merged in yet.
    */
   uint32 OS_CpuContextSize;

   /**
    * Number of EEPROM banks on this platform
    */
   uint32 HW_NumEepromBanks;

} Target_PspConfigData;

/**
 * Extern reference to psp config struct.
 * Allows the actual instantiation to be done outside this module
 */
extern Target_PspConfigData GLOBAL_PSP_CONFIGDATA;

/**
 * Extern reference to driver preload list.
 * Allows the actual instantiation to be done outside this module
 */
extern CFE_PSP_IoDriverLoadEntry_t TARGET_IODRIVER_PRELOADLISTDATA[];


#endif /* CFE_PSP_CONFIG_H_ */
