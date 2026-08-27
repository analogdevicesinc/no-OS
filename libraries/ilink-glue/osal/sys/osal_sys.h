/***************************************************************************//**
 *   @file   osal_sys.h
 *   @brief  OSAL type definitions — maps rt-labs OSAL types to FreeRTOS
 *           handles.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2017 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef OSAL_SYS_H
#define OSAL_SYS_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

typedef struct QueueDefinition   os_mutex_t;
typedef struct EventGroupDef_t  os_event_t;
typedef struct tskTaskControlBlock os_thread_t;
typedef struct QueueDefinition   os_mbox_t;
typedef struct tmrTimerControl   os_timer_t;

#endif /* OSAL_SYS_H */
