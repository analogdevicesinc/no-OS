/***************************************************************************//**
 *   @file   FreeRTOSConfig.h
 *   @brief  FreeRTOS configuration for the ADIN1140 project (MAX78000).
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef ADIN1140_FREE_RTOS_CONFIG
#define ADIN1140_FREE_RTOS_CONFIG

#include <stdint.h>

#define configCPU_CLOCK_HZ          ((uint32_t)250000000)
#define configTICK_RATE_HZ          ((portTickType)1000)
#define configRTC_TICK_RATE_HZ      (32768)
#define configTOTAL_HEAP_SIZE       ((size_t)(64 * 1024))
#define configMINIMAL_STACK_SIZE    ((uint16_t)512)
#define configIIO_APP_STACK_SIZE    ((uint16_t)4096)
#define configMAX_PRIORITIES        5
#define configUSE_PREEMPTION        1
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#define configUSE_CO_ROUTINES       0
#define configUSE_16_BIT_TICKS      0
#define configUSE_MUTEXES           1
#define configUSE_COUNTING_SEMAPHORES 1
#define configUSE_RECURSIVE_MUTEXES 1

#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configHEAP_CLEAR_MEMORY_ON_FREE  1
#define configUSE_TRACE_FACILITY         1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

#define INCLUDE_vTaskPrioritySet    0
#define INCLUDE_vTaskDelete         1
#define INCLUDE_vTaskSuspend        1
#define INCLUDE_vTaskDelayUntil     1
#define INCLUDE_uxTaskPriorityGet   0
#define INCLUDE_vTaskDelay          1

#define configPRIO_BITS             4
#define configKERNEL_INTERRUPT_PRIORITY \
	((unsigned char)7 << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
	((unsigned char)5 << (8 - configPRIO_BITS))

#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#define configCOMMAND_INT_MAX_OUTPUT_SIZE 1
#define INCLUDE_xTaskGetIdleTaskHandle   1
#define INCLUDE_pxTaskGetStackStart      1

#endif /* ADIN1140_FREE_RTOS_CONFIG */
