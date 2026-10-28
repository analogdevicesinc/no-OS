/***************************************************************************//**
 *   @file   maxim_core.h
 *   @brief  MAX78000 ARM (CPU0) core helpers.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CORE_H_
#define MAXIM_CORE_H_

#include <stdbool.h>
#include "max78000.h"

#define MAXIM_TIME_TIMER_ID -1
#define MAXIM_IRQ_HANDLER

static inline uint32_t maxim_irq_save(void)
{
	uint32_t state = __get_PRIMASK();

	__disable_irq();
	return state;
}

static inline void maxim_irq_restore(uint32_t state)
{
	__set_PRIMASK(state);
}

static inline bool maxim_timer_available(uint32_t id)
{
	return true;
}

static inline void maxim_ipc_barrier(void)
{
	__DMB();
}

#endif /* MAXIM_CORE_H_ */
