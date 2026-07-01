/*******************************************************************************
 *   @file   maxim_capi_wdt.h
 *   @brief  Header file for WDT functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef MAXIM_CAPI_WDT_H_
#define MAXIM_CAPI_WDT_H_

#include "capi_wdt.h"
#include "wdt.h"
#include <stdbool.h>

enum max_capi_wdt_flag {
	MAX_CAPI_WDT_FLAG_INT_LATE =	(1 << 0),
	MAX_CAPI_WDT_FLAG_INT_EARLY =	(1 << 1),
	MAX_CAPI_WDT_FLAG_RST_LATE =	(1 << 2),
	MAX_CAPI_WDT_FLAG_RST_EARLY =	(1 << 3),
};

enum max_capi_wdt_period {
	MAX_CAPI_WDT_PERIOD_2_31 = MXC_WDT_PERIOD_2_31,
	MAX_CAPI_WDT_PERIOD_2_30 = MXC_WDT_PERIOD_2_30,
	MAX_CAPI_WDT_PERIOD_2_29 = MXC_WDT_PERIOD_2_29,
	MAX_CAPI_WDT_PERIOD_2_28 = MXC_WDT_PERIOD_2_28,
	MAX_CAPI_WDT_PERIOD_2_27 = MXC_WDT_PERIOD_2_27,
	MAX_CAPI_WDT_PERIOD_2_26 = MXC_WDT_PERIOD_2_26,
	MAX_CAPI_WDT_PERIOD_2_25 = MXC_WDT_PERIOD_2_25,
	MAX_CAPI_WDT_PERIOD_2_24 = MXC_WDT_PERIOD_2_24,
	MAX_CAPI_WDT_PERIOD_2_23 = MXC_WDT_PERIOD_2_23,
	MAX_CAPI_WDT_PERIOD_2_22 = MXC_WDT_PERIOD_2_22,
	MAX_CAPI_WDT_PERIOD_2_21 = MXC_WDT_PERIOD_2_21,
	MAX_CAPI_WDT_PERIOD_2_20 = MXC_WDT_PERIOD_2_20,
	MAX_CAPI_WDT_PERIOD_2_19 = MXC_WDT_PERIOD_2_19,
	MAX_CAPI_WDT_PERIOD_2_18 = MXC_WDT_PERIOD_2_18,
	MAX_CAPI_WDT_PERIOD_2_17 = MXC_WDT_PERIOD_2_17,
	MAX_CAPI_WDT_PERIOD_2_16 = MXC_WDT_PERIOD_2_16,
};

enum max_capi_wdt_clock {
	MAX_CAPI_WDT_CLOCK_PCLK = MXC_WDT_PCLK,
	MAX_CAPI_WDT_CLOCK_IBRO = MXC_WDT_IBRO_CLK,
};

enum max_capi_wdt_mode {
	MAX_CAPI_WDT_MODE_COMPATIBILITY = MXC_WDT_COMPATIBILITY,
	MAX_CAPI_WDT_MODE_WINDOWED = MXC_WDT_WINDOWED,
};

struct max_capi_wdt_extra {
	/** Clock source */
	enum max_capi_wdt_clock clock_source;
};

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
	/** Callback */
	capi_wdt_callback_t callback;
};

struct max_capi_wdt_chan_extra {
	/** Late interrupt event threshold */
	enum max_capi_wdt_period late_interrupt;
	/** Late reset event threshold */
	enum max_capi_wdt_period late_reset;
	/** Compatibility or windowed mode */
	enum max_capi_wdt_mode mode;
	/** Early interrupt event threshold (for windowed mode) */
	enum max_capi_wdt_period early_interrupt;
	/** Early reset event threshold (for windowed mode) */
	enum max_capi_wdt_period early_reset;
};

extern struct capi_wdt_ops max_capi_wdt_ops;

int max_capi_wdt_get_flags(struct capi_wdt_handle *handle, uint32_t *flags);
int max_capi_wdt_clear_flags(struct capi_wdt_handle *handle);

#endif /* MAXIM_CAPI_WDT_H_ */
