/*
    FreeRTOS V7.5.0 - Copyright (C) 2013 Real Time Engineers Ltd.

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <at91/boards/ISIS_OBC_G20/board.h>
#include <at91/utility/trace.h>
#include <at91/utility/assert.h>

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			0
#define configCPU_CLOCK_HZ				( ( unsigned long ) BOARD_PROCESSOR_CLOCK )
#define configTICK_RATE_HZ				( ( portTickType ) 1000 ) // Do not change this value
#define configMAX_PRIORITIES			( ( unsigned portBASE_TYPE ) 5 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 1024 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 40000 ) )
#define configMAX_TASK_NAME_LEN			( 32 )
// #define configUSE_TRACE_FACILITY		0
#define configUSE_TRACE_FACILITY		1
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			0
#define configUSE_MUTEXES				1
#define configCHECK_FOR_STACK_OVERFLOW	1
#define configUSE_RECURSIVE_MUTEXES		1
#define configQUEUE_REGISTRY_SIZE		32
#define configUSE_COUNTING_SEMAPHORES	1
#define configUSE_QUEUE_SETS			1

/* Hooks */
#define configUSE_IDLE_HOOK				1
#define configUSE_TICK_HOOK				0
#define configUSE_MALLOC_FAILED_HOOK	1

/* Enabling SW timers */
#define configUSE_TIMERS				1
#define configTIMER_TASK_PRIORITY		configMAX_PRIORITIES-1
#define configTIMER_QUEUE_LENGTH		10
#define configTIMER_TASK_STACK_DEPTH	4096

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/*
 * This is required when linking with newlib/newlib-nano if pre-emptive
 * Even though we use cooperative scheduling, context swicthing in ISR means we
 * are per def. pre-emptive
 */
#define configUSE_NEWLIB_REENTRANT	1

/* Define configASSERT() to call vAssertCalled() if the assertion fails.  The assertion
has failed if the value of the parameter passed into configASSERT() equals zero. */
#define configASSERT( x )  SANITY_CHECK( ( x ) )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_xTaskResumeFromISR			1
#define INCLUDE_xTaskGetCurrentTaskHandle	1
#define INCLUDE_eTaskGetState               1
#define INCLUDE_xTaskGetSchedulerState		1

#endif /* FREERTOS_CONFIG_H */
