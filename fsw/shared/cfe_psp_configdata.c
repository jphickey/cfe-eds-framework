/**
 * \file cfe_psp_config.c
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

#include <cfe_psp.h>
#include <cfe_psp_configdata.h>

/*
 * Some of the referenced items are actually NOT in a header file and/or
 * the header file is private.  This is corrected after the refactoring
 * is merged in, however for the time being they are just declared
 * as "extern" right here.
 */
extern OS_VolumeInfo_t OS_VolumeTable[];
extern CFE_PSP_MemTable_t CFE_PSP_MemoryTable[];

/**
 * Instantiation of the PSP/HW configuration data
 *
 * This is done locally right now because this can access
 * the PSP configuration data directly, but may move to the
 * PSP common code when refactoring is done.
 */
Target_PspConfigData GLOBAL_PSP_CONFIGDATA =
{
      .PSP_WatchdogMin = 0,
      .PSP_WatchdogMax = 0xFFFFFFFF,
      .PSP_DriverList = TARGET_IODRIVER_PRELOADLISTDATA,
      .PSP_MemTableSize = CFE_PSP_MEM_TABLE_SIZE,
      .PSP_MemoryTable = CFE_PSP_MemoryTable,

      .OS_VolumeTableSize = NUM_TABLE_ENTRIES,
      .OS_VolumeTable = OS_VolumeTable,
      .OS_CpuContextSize = 0,

      .HW_NumEepromBanks = 1
};

