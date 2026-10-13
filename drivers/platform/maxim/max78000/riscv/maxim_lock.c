/***************************************************************************//**
 *   @file   maxim_lock.c
 *   @brief  MAX78000 RISC-V (CPU1) software lock primitives.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "maxim_core.h"
#include "mxc_errors.h"
#include "mxc_lock.h"

/**
 * @brief Acquire an SDK software lock within CPU1; cross-core locks use SEMA.
 * @param lock - Lock variable.
 * @param value - Non-zero value stored in the lock while it is held.
 * @return E_NO_ERROR if the lock was acquired, E_BUSY otherwise.
 */
int MXC_GetLock(uint32_t *lock, uint32_t value)
{
	uint32_t state = maxim_irq_save();
	int ret = E_BUSY;

	if (!*lock) {
		*lock = value;
		maxim_ipc_barrier();
		ret = E_NO_ERROR;
	}
	maxim_irq_restore(state);
	return ret;
}

/**
 * @brief Release an SDK software lock.
 * @param lock - Lock variable.
 */
void MXC_FreeLock(uint32_t *lock)
{
	maxim_ipc_barrier();
	*lock = 0;
}
