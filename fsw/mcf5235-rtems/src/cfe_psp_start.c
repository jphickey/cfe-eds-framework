/******************************************************************************
** File:  cfe_psp_start.c
**
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
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
#include <rtems.h>

/*
** cFE includes 
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

#include "cfe_psp.h" 
#include "cfe_psp_memory.h"           

/*
**  External Declarations
*/
extern void CFE_TIME_Local1HzISR(void);

/*
** Global variables
*/

rtems_id          RtemsTimerId;


/*
** 1 HZ Timer "ISR"
*/
int timer_count = 0;

rtems_timer_service_routine CFE_PSP_1hzTimer( rtems_id timer_id, void *user_data)
{
   int status;

   timer_count++;
  
   /* printk("T:%d\n",timer_count);  */
   status = rtems_timer_reset(timer_id);
   CFE_TIME_Local1HzISR(); 
}


/*
** A simple entry point to start from the loader
*/
void goCFE(void)
{
   CFE_PSP_Main(1, "/tftp/cfe_es_startup.scr");
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
   rtems_status_code RtemsStatus;
   rtems_name        RtemsTimerName;
   
   /*
   ** Initialize the OS API
   */
   OS_API_Init();

   /* 
   ** Create an interval timer for the 1hz
   */
   RtemsTimerName = rtems_build_name('1',' ','H','Z');
   RtemsStatus = rtems_timer_create(RtemsTimerName, &RtemsTimerId);
   if ( RtemsStatus != RTEMS_SUCCESSFUL )
   {
      printf("CFE_PSP: Error: Cannot create RTEMS 1hz interval timer\n");
   }
   
   /*
   ** Allocate memory for the cFE memory. Note that this is malloced on
   ** the COTS board, but will be a static location in the ETU. 
   */
   printf("Sizeof BSP reserved memory = %d bytes\n",sizeof(CFE_PSP_ReservedMemory_t));

   CFE_PSP_ReservedMemoryPtr = malloc(sizeof(CFE_PSP_ReservedMemory_t));

   if ( CFE_PSP_ReservedMemoryPtr == NULL )
   {
      printf("CFE_PSP: Error: Cannot malloc BSP reserved memory!\n");
   }
   else
   {
      printf("CFE_PSP: Allocated %d bytes for PSP reserved memory at: 0x%08X\n",
              sizeof(CFE_PSP_ReservedMemory_t), (int)CFE_PSP_ReservedMemoryPtr);
   }
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
   CFE_ES_Main(reset_type,reset_subtype, 1, StartupFilePath);

   /*
   ** Setup the timer to fire at 1hz 
   */  
   
   /* CLOCK TICK */ 
   RtemsStatus = rtems_timer_fire_after(RtemsTimerId, 100, CFE_PSP_1hzTimer, NULL );
   if ( RtemsStatus != RTEMS_SUCCESSFUL )
   {
      printf("CFE_PSP: Error: Cannot setup interval timer to fire at 1hz\n");
   }
      
   /*
   ** Return to the shell/monitor
   */
   return; 
   
}

