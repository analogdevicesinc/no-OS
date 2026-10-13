/* SPDX-License-Identifier: BSD-3-Clause */

#include "maxim_core.h"
#include "mxc_errors.h"
#include "mxc_lock.h"

/** @brief SDK software lock within CPU1; cross-core locks use SEMA. */
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

void MXC_FreeLock(uint32_t *lock)
{
	maxim_ipc_barrier();
	*lock = 0;
}
