/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_irq.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the IRQ driver via its ops pointer structure. Note, it does not
 *        contain the actual driver implementation.
 */

#include "no_os_error.h"
#include "no_os_mutex.h"
#include "max32655.h"
#include "capi_irq.h"
#include "maxim_irq.h"

struct capi_isr_table isr_table[MXC_IRQ_EXT_COUNT];
static void *nvic_lock;

int capi_irq_init(struct capi_irq_config *config)
{
	no_os_mutex_init(&nvic_lock);

	return 0;
}

int capi_irq_deinit(void)
{
	no_os_mutex_remove(nvic_lock);

	return 0;
}

int capi_irq_global_enable(void)
{
	no_os_mutex_lock(nvic_lock);
	__enable_irq();
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_global_disable(void)
{
	no_os_mutex_lock(nvic_lock);
	__disable_irq();
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_enable(uint32_t irq)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_EnableIRQ(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_disable(uint32_t irq)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_DisableIRQ(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	uint8_t enabled;

	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	enabled = NVIC_GetEnableIRQ(irq);
	NVIC_DisableIRQ(irq);
	isr_table[irq].isr = isr;
	isr_table[irq].arg = arg;

	if (enabled)
		NVIC_EnableIRQ(irq);

	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_clear_pending(uint32_t irq)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_ClearPendingIRQ(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	*pactive  = NVIC_GetActive(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_SetPriority(irq, priority);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	*priority = NVIC_GetPriority(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_set_level_edge_trigger(uint32_t irq, enum capi_irq_trig_level trigger)
{
	return -ENOSYS;
}
