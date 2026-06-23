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

#include "capi_irq.h"

// For stub implementation, we directly call the stub functions
#ifdef CAPI_STUB_IMPLEMENTATION
#include "capi_irq_stub_driver.h"

int capi_irq_init(struct capi_irq_config *config)
{
	return capi_irq_stub_init(config);
}

int capi_irq_deinit(void)
{
	return capi_irq_stub_deinit();
}

int capi_irq_global_enable(void)
{
	return capi_irq_stub_global_enable();
}

int capi_irq_global_disable(void)
{
	return capi_irq_stub_global_disable();
}

int capi_irq_enable(uint32_t irq)
{
	return capi_irq_stub_enable(irq);
}

int capi_irq_disable(uint32_t irq)
{
	return capi_irq_stub_disable(irq);
}

int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	return capi_irq_stub_connect(irq, isr, arg);
}

int capi_irq_clear_pending(uint32_t irq)
{
	return capi_irq_stub_clear_pending(irq);
}

int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	return capi_irq_stub_get_status(irq, pactive);
}

int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	return capi_irq_stub_set_priority(irq, priority);
}

int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	return capi_irq_stub_get_priority(irq, priority);
}

int capi_irq_set_level_edge_trigger(uint32_t irq,
				    enum capi_irq_trig_level trigger)
{
	return capi_irq_stub_set_level_edge_trigger(irq, trigger);
}

#endif /* CAPI_STUB_IMPLEMENTATION */
