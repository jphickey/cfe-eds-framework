/******************************************************************************
** File:  cfe_psp_start.c
**
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASAs Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
** 
**
**
** Purpose:
**   cFE BSP main entry point.
**
** History:
**   2004/09/23  J.P. Swinski    | Initial version,
**   2004/10/01  P.Kutt          | Replaced OS API task delay with VxWorks functions
**                                 since OS API is initialized later.
**
******************************************************************************/
#define _USING_RTEMS_INCLUDES_

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <rtems.h>
#include <rtems/mkrootfs.h>
#include <rtems/bdbuf.h>
#include <rtems/blkdev.h>
#include <rtems/diskdevs.h>
#include <rtems/error.h>
#include <rtems/ramdisk.h>
#include <rtems/dosfs.h>
#include <rtems/fsmount.h>
#include <rtems/shell.h>


extern rtems_status_code rtems_ide_part_table_initialize (const char* );

/*
** cFE includes 
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_psp.h" 
#include "cfe_psp_memory.h"

#define RTEMS_NUMBER_OF_RAMDISKS 1


#ifdef _ENHANCED_BUILD_

/*
 * The preferred way to obtain the CFE tunable values at runtime is via
 * the dynamically generated configuration object.  This allows a single build
 * of the PSP to be completely CFE-independent.
 */
#include <target_config.h>

#define CFE_ES_MAIN_FUNCTION        (*GLOBAL_CONFIGDATA.CfeConfig->SystemMain)
#define CFE_TIME_1HZ_FUNCTION       (*GLOBAL_CONFIGDATA.CfeConfig->System1HzISR)
#define CFE_ES_NONVOL_STARTUP_FILE  (GLOBAL_CONFIGDATA.CfeConfig->NonvolStartupFile)
#define CFE_CPU_ID                  (GLOBAL_CONFIGDATA.Default_CpuId)
#define CFE_CPU_NAME                (GLOBAL_CONFIGDATA.Default_CpuName)
#define CFE_SPACECRAFT_ID           (GLOBAL_CONFIGDATA.Default_SpacecraftId)

#else

/*
 * cfe_platform_cfg.h needed for CFE_ES_NONVOL_STARTUP_FILE, CFE_CPU_ID/CPU_NAME/SPACECRAFT_ID
 *
 *  - this should NOT be included here -
 *
 * it is only for compatibility with the old makefiles.  Including this makes the PSP build
 * ONLY compatible with a CFE build using this exact same CFE platform config.
 */

#include "cfe_platform_cfg.h"

extern void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath );
extern void CFE_TIME_Local1HzISR(void);

#define CFE_ES_MAIN_FUNCTION     CFE_ES_Main
#define CFE_TIME_1HZ_FUNCTION    CFE_TIME_Local1HzISR

#endif

/*
** Global variables
*/

/*
 * The RAM Disk configuration.
 */
rtems_ramdisk_config rtems_ramdisk_configuration[RTEMS_NUMBER_OF_RAMDISKS];

/*
 * The number of RAM Disk configurations.
*/
size_t rtems_ramdisk_configuration_size = RTEMS_NUMBER_OF_RAMDISKS;

/*
** RAM Disk IO op table.
*/
rtems_driver_address_table rtems_ramdisk_io_ops =
{
  initialization_entry: ramdisk_initialize,
  open_entry:           rtems_blkdev_generic_open,
  close_entry:          rtems_blkdev_generic_close,
  read_entry:           rtems_blkdev_generic_read,
  write_entry:          rtems_blkdev_generic_write,
  control_entry:        rtems_blkdev_generic_ioctl
};

rtems_id          RtemsTimerId;

bool CFE_PSP_Login_Check(const char *user, const char *passphrase)
{
   return TRUE;
}

/*
** 1 HZ Timer "ISR"
*/
int timer_count = 0;

void CFE_PSP_Setup(void)
{
   int status;
   
   OS_printf( "\n\n*** RTEMS Info ***\n" );
   OS_printf("%s", _Copyright_Notice );
   OS_printf("%s\n\n", _RTEMS_version );
   OS_printf(" Stack size=%d\n", (int)Configuration.stack_space_size );
   OS_printf(" Workspace size=%d\n",   (int) Configuration.work_space_size );
   OS_printf("\n");
   OS_printf( "*** End RTEMS info ***\n\n" );

   /*
   ** Create the RTEMS Root file system
   */
   status = rtems_create_root_fs();
   if (status != RTEMS_SUCCESSFUL)
   {
       OS_printf("Creating Root file system failed: %s\n",rtems_status_text(status));
   }

   /*
   ** create the directory mountpoints
   */
   status = mkdir("/ram", S_IFDIR |S_IRWXU | S_IRWXG | S_IRWXO); /* For ramdisk mountpoint */
   if (status != RTEMS_SUCCESSFUL)
   {
       OS_printf("mkdir failed: %s\n", strerror (errno));
   }

   status = mkdir("/cf", S_IFDIR |S_IRWXU | S_IRWXG | S_IRWXO); /* For EEPROM mountpoint */
   if (status != RTEMS_SUCCESSFUL)
   {
       OS_printf("mkdir failed: %s\n", strerror (errno));
       return;
   }

   /*
    * Register the IDE partition table.
    */
   status = rtems_ide_part_table_initialize ("/dev/hda");
   if (status != RTEMS_SUCCESSFUL)
   {
     OS_printf ("error: ide partition table not found: %s / %s\n",
             rtems_status_text (status),strerror(errno));
   }

   status = mount("/dev/hda1", "/cf",
         RTEMS_FILESYSTEM_TYPE_DOSFS,
         RTEMS_FILESYSTEM_READ_WRITE,
         NULL);
   if (status < 0)
   {
     OS_printf ("mount failed: %s\n", strerror (errno));
   }

}

/*
** A simple entry point to start from the loader
*/
rtems_task Init(
  rtems_task_argument ignored
)
{
   CFE_PSP_Setup();

   /*
   ** Initialize the OS API
   */
   OS_API_Init();


   /*
   ** Run the PSP Main - this will return when init is complete
   */
   CFE_PSP_Main(1, "/cf/cfe_es_startup.scr");


   /*
   ** Run the RTEMS shell.
   */
   while (1)
   {
      if (rtems_shell_init("SHLL", RTEMS_MINIMUM_STACK_SIZE * 4, 100, "/dev/console", false, true, CFE_PSP_Login_Check) < 0)
      {
        OS_printf ("shell init failed: %s\n", strerror (errno));
        OS_TaskDelay(1000);
      }
   }
}

/******************************************************************************
**  Function:  CFE_PSP_1HzTimerTick
**
**  Purpose:
**    Simple wrapper function to call the CFE 1Hz ISR function
**
*/
void CFE_PSP_1HzTimerTick(uint32 timer_id, void *arg)
{
   CFE_TIME_1HZ_FUNCTION();
}



/******************************************************************************
**  Function:  CFE_PSP_SetupSystemTimer
**
**  Purpose:
**    BSP system time base and timer object setup.
**    This does the necessary work to start the 1Hz time tick required by CFE
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
**
** NOTE:
**      The handles to the timebase/timer objects are "start and forget"
**      as they are supposed to run forever as long as CFE runs.
**
**      If needed for e.g. additional timer creation, they can be recovered
**      using an OSAL GetIdByName() call.
**
**      This is preferred anyway -- far cleaner than trying to pass the uint32 value
**      up to the application somehow.
*/

void CFE_PSP_SetupSystemTimer(void)
{
    uint32 SystemTimebase;
    uint32 SystemTimer;
    int32  Status;

#if OSAL_API_VERSION >= 40200
    Status = OS_TimeBaseCreate(&SystemTimebase, "System Timebase", NULL);
    if (Status != OS_SUCCESS)
    {
        OS_printf("CFE_PSP: Error Creating System Timebase: %d\n", (int)Status);
        return;
    }

    Status = OS_TimeBaseSet(SystemTimebase, 250000, 250000);
    if (Status != OS_SUCCESS)
    {
       OS_printf("CFE_PSP: Error Setting System Timebase: %d\n", (int)Status);
       return;
    }

    Status = OS_TimerAdd(&SystemTimer, "System 1Hz", SystemTimebase, CFE_PSP_1HzTimerTick, NULL);
    if (Status != OS_SUCCESS)
    {
       OS_printf("CFE_PSP: Error Adding System Timer: %d\n", (int)Status);
       return;
    }

    Status = OS_TimerSet(SystemTimer, 500000, 1000000);
    if (Status != OS_SUCCESS)
    {
       OS_printf("CFE_PSP: Error Starting System Timer: %d\n", (int)Status);
       return;
    }
#endif


    OS_printf("CFE_PSP: OS Timer object created successfully\n");
}


/******************************************************************************
**  Function:  CFE_PSP_Main()
**
**  Purpose:
**    Application entry point.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/

void CFE_PSP_Main(  int ModeId, char *StartupFilePath )
{
   uint32            reset_type;
   uint32            reset_subtype;


   /*
   ** Determine Reset type by reading the hardware reset register.
   */
   reset_type = CFE_PSP_RST_TYPE_POWERON;
   reset_subtype = CFE_PSP_RST_SUBTYPE_POWER_CYCLE;

   /*
   ** Initialize the reserved memory 
   */
   CFE_PSP_InitProcessorReservedMemory(reset_type);

   /*
   ** Call cFE entry point. This will return when cFE startup
   ** is complete.
   */
   CFE_ES_MAIN_FUNCTION(reset_type,reset_subtype, 1, CFE_ES_NONVOL_STARTUP_FILE);

   /*
   ** Setup the timer to fire at 1hz 
   */  
   CFE_PSP_SetupSystemTimer();

}

/* configuration information */

/*
** RTEMS OS Configuration defintions
*/
#define TASK_INTLEVEL 0
#define CONFIGURE_INIT
#define CONFIGURE_INIT_TASK_ATTRIBUTES  (RTEMS_FLOATING_POINT | RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(TASK_INTLEVEL))
#define CONFIGURE_INIT_TASK_STACK_SIZE  (20*1024)
#define CONFIGURE_INIT_TASK_PRIORITY    120

#define CONFIGURE_MAXIMUM_TASKS                      64
#define CONFIGURE_MAXIMUM_TIMERS                     10
#define CONFIGURE_MAXIMUM_SEMAPHORES                 64
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES             64

#define CONFIGURE_EXECUTIVE_RAM_SIZE    (1024*1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS     100

#define CONFIGURE_FILESYSTEM_RFS
#define CONFIGURE_FILESYSTEM_IMFS
#define CONFIGURE_FILESYSTEM_DOSFS
#define CONFIGURE_FILESYSTEM_DEVFS

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MICROSECONDS_PER_TICK              10000

#define CONFIGURE_MAXIMUM_DRIVERS                   10

#define CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
#define CONFIGURE_ATA_DRIVER_TASK_PRIORITY         9

#define CONFIGURE_MAXIMUM_POSIX_KEYS               4

#include <rtems/confdefs.h>

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_MOUNT_MSDOS

#include <rtems/shellconfig.h>


