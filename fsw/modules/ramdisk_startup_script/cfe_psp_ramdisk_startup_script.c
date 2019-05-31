/*
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
*/

/**
 * \file cfe_psp_ramdisk_startup_script.c
 *
 *  Created on: May 24, 2019
 *      Author: joseph.p.hickey@nasa.gov
 *
 * This is a simple module that installs a statically-linked CFE startup
 * script into the RAMDISK before starting CFE.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cfe_psp.h"
#include "cfe_psp_module.h"
#include "target_config.h"

CFE_PSP_MODULE_DECLARE_SIMPLE(ramdisk_startup_script);

extern const unsigned char StartupScript_DATA[];
extern const unsigned long StartupScript_SIZE;

void ramdisk_startup_script_Init(uint32 PspModuleId)
{
    int fd;

    fd = open(GLOBAL_CONFIGDATA.CfeConfig->NonvolStartupFile,
            O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd >= 0)
    {
        write(fd, StartupScript_DATA, StartupScript_SIZE);
        close(fd);
    }
    else
    {
        printf("CFE_PSP: %s(): ERROR: %s: %s\n", __func__,
                GLOBAL_CONFIGDATA.CfeConfig->NonvolStartupFile, strerror(errno));
    }
}


