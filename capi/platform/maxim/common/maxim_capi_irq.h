/*******************************************************************************
 *   @file   maxim_capi_irq.h
 *   @brief  Header file for the MAX32650/MAX32690 CAPI IRQ functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_IRQ_H_
#define MAXIM_CAPI_IRQ_H_

#include <stdbool.h>
#include <stdint.h>
#include "capi_irq.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_irq_extra_config
 * @brief Platform specific IRQ controller configuration
 */
struct max_capi_irq_extra_config {
	/** Priority applied when an IRQ without one is enabled */
	uint32_t default_priority;
};

/**
 * @struct max_capi_irq_entry
 * @brief IRQ dispatch table entry
 */
struct max_capi_irq_entry {
	capi_isr_callback_t callback;
	void *arg;
	/** Vector installed before capi_irq_connect(), restored on deinit */
	void (*prev_vector)(void);
	bool connected;
	bool enabled;
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_IRQ_H_ */
