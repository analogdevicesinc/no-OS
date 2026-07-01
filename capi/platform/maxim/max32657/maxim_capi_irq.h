/***************************************************************************//**
 *   @file   maxim_capi_irq.h
 *   @brief  Header file for IRQ functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_IRQ_H_
#define MAXIM_CAPI_IRQ_H_

#include "uart.h"
#include "capi_irq.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_irq_entry
 * @brief IRQ entry struct
 */
struct max_capi_irq_entry {
	/** Callback function */
	capi_isr_callback_t callback;
	/** Callback argument */
	void *arg;
	/* IRQ enabled flag */
	bool enabled;
};

/**
 * @struct max_capi_irq_extra_config
 * @brief MAX32657 platform-specific IRQ extra config
 */
struct max_capi_irq_extra_config {
	uint32_t default_priority;
};

extern const struct capi_irq_ops max_capi_irq_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* MAXIM_CAPI_IRQ_H_ */
