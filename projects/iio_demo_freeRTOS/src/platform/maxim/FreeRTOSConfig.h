/***************************************************************************//**
 *   @file   FreeRTOSConfig.h
 *   @brief  Defines useded for configuring freeRTOS in iio_demo_freeRTOS project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_DEMO_FREE_RTOS_CONFIG
#define IIO_DEMO_FREE_RTOS_CONFIG

#include <stdint.h>
#define configCPU_CLOCK_HZ ((uint32_t)100000000)
#define configTICK_RATE_HZ ((portTickType)1000)
#define configRTC_TICK_RATE_HZ (32768)
#define configTOTAL_HEAP_SIZE ((size_t)(26 * 1024))
#define configMINIMAL_STACK_SIZE ((uint16_t)512)
#define configIIO_APP_STACK_SIZE ((uint16_t)2048)
// #define configSUPPORT_STATIC_ALLOCATION 1
#define configMAX_PRIORITIES 5
#define configUSE_PREEMPTION 1
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configUSE_CO_ROUTINES 0
#define configUSE_16_BIT_TICKS 0
#define configUSE_MUTEXES 1

#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configHEAP_CLEAR_MEMORY_ON_FREE 1
#define configUSE_TRACE_FACILITY 1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet 0
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_uxTaskPriorityGet 0
#define INCLUDE_vTaskDelay 1
/* # of priority bits (configured in hardware) is provided by CMSIS */
#define configPRIO_BITS 3
/* Priority 7, or 255 as only the top three bits are implemented.  This is the lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY ((unsigned char)7 << (8 - configPRIO_BITS))
/* Priority 5, or 160 as only the top three bits are implemented. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ((unsigned char)5 << (8 - configPRIO_BITS))
/* Alias the default handler names to match CMSIS weak symbols */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* FreeRTOS+CLI requires this size to be defined, but we do not use it */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE 1
#define INCLUDE_xTaskGetIdleTaskHandle 1
#define INCLUDE_pxTaskGetStackStart  1

#endif // IIO_DEMO_FREE_RTOS_CONFIG
