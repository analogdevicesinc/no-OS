/*******************************************************************************
 *   @file   maxim_capi_wdt_priv.h
 *   @brief  Private handle for the common Maxim (MSDK) CAPI WDT backend
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_WDT_PRIV_H_
#define MAXIM_CAPI_WDT_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "maxim_capi_wdt.h"

struct max_capi_wdt_priv {
	/** WDT ID */
	uint32_t id;
	/** Clock source */
	enum max_capi_wdt_clock clock_source;
	/** Clock frequency in Hz */
	uint32_t clock_freq_hz;
	/** Configured or not */
	bool configured;
	/** Enabled or not */
	bool enabled;
	/** WDT IRQ has been connected/enabled */
	bool irq_connected;
	/** Callback */
	capi_wdt_callback_t callback;
};

#define CAPI_WDT_HANDLE_MAXIM_INIT()			\
	(&(struct capi_wdt_handle) {			\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_wdt_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif

#endif /* MAXIM_CAPI_WDT_PRIV_H_ */
