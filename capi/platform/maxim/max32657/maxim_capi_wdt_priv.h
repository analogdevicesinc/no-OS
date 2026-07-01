/*******************************************************************************
 *   @file   maxim_capi_wdt_priv.h
 *   @brief  Header file for the WDT private handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_WDT_PRIV_H_
#define MAIXM_CAPI_WDT_PRIV_H_

#include "maxim_capi_wdt.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

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
	/** WDT IRQ connected and enabled at the NVIC */
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
