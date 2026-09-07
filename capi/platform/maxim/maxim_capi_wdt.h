/*******************************************************************************
 *   @file   maxim_capi_wdt.h
 *   @brief  Public interface for the common Maxim (MSDK) CAPI WDT backend
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_WDT_H_
#define MAXIM_CAPI_WDT_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "capi_wdt.h"
#include "wdt.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * The MSDK WDT public API (MXC_WDT_Init/SetIntPeriod/SetResetPeriod/Enable/...)
 * and the WDTn_CTRL field spellings are uniform across every part surveyed
 * (MAX32655/57/90), so this backend needs no signature or register-rev shim.
 *
 * The one portability boundary is instance selection: parts with a low-power
 * watchdog expose MXC_WDT0 (+ MXC_WDT1) and WDT0_IRQn/WDT1_IRQn, while a
 * single-instance part exposes only MXC_WDT and WDT_IRQn. That split is handled
 * in the .c via _max_wdt_regs()/_max_wdt_irq(), gated on MXC_WDT0.
 */

/**
 * @enum max_capi_wdt_flag
 * @brief Interrupt/reset flags
 */
enum max_capi_wdt_flag {
	/** Feed happened too late and generated an interrupt */
	MAX_CAPI_WDT_FLAG_INT_LATE =	(1 << 0),
	/** Feed happened too early and generated an interrupt */
	MAX_CAPI_WDT_FLAG_INT_EARLY =	(1 << 1),
	/** Feed happened too late and generated a reset */
	MAX_CAPI_WDT_FLAG_RST_LATE =	(1 << 2),
	/** Feed happened too early and generated a reset */
	MAX_CAPI_WDT_FLAG_RST_EARLY =	(1 << 3),
};

/**
 * @enum max_capi_wdt_period
 * @brief Timer period in WDT clock ticks
 */
enum max_capi_wdt_period {
	/** 2^31 ticks */
	MAX_CAPI_WDT_PERIOD_2_31 = MXC_WDT_PERIOD_2_31,
	/** 2^30 ticks */
	MAX_CAPI_WDT_PERIOD_2_30 = MXC_WDT_PERIOD_2_30,
	/** 2^29 ticks */
	MAX_CAPI_WDT_PERIOD_2_29 = MXC_WDT_PERIOD_2_29,
	/** 2^28 ticks */
	MAX_CAPI_WDT_PERIOD_2_28 = MXC_WDT_PERIOD_2_28,
	/** 2^27 ticks */
	MAX_CAPI_WDT_PERIOD_2_27 = MXC_WDT_PERIOD_2_27,
	/** 2^26 ticks */
	MAX_CAPI_WDT_PERIOD_2_26 = MXC_WDT_PERIOD_2_26,
	/** 2^25 ticks */
	MAX_CAPI_WDT_PERIOD_2_25 = MXC_WDT_PERIOD_2_25,
	/** 2^24 ticks */
	MAX_CAPI_WDT_PERIOD_2_24 = MXC_WDT_PERIOD_2_24,
	/** 2^23 ticks */
	MAX_CAPI_WDT_PERIOD_2_23 = MXC_WDT_PERIOD_2_23,
	/** 2^22 ticks */
	MAX_CAPI_WDT_PERIOD_2_22 = MXC_WDT_PERIOD_2_22,
	/** 2^21 ticks */
	MAX_CAPI_WDT_PERIOD_2_21 = MXC_WDT_PERIOD_2_21,
	/** 2^20 ticks */
	MAX_CAPI_WDT_PERIOD_2_20 = MXC_WDT_PERIOD_2_20,
	/** 2^19 ticks */
	MAX_CAPI_WDT_PERIOD_2_19 = MXC_WDT_PERIOD_2_19,
	/** 2^18 ticks */
	MAX_CAPI_WDT_PERIOD_2_18 = MXC_WDT_PERIOD_2_18,
	/** 2^17 ticks */
	MAX_CAPI_WDT_PERIOD_2_17 = MXC_WDT_PERIOD_2_17,
	/** 2^16 ticks */
	MAX_CAPI_WDT_PERIOD_2_16 = MXC_WDT_PERIOD_2_16,
};

/**
 * @enum max_capi_wdt_clock
 * @brief Clock selection
 */
enum max_capi_wdt_clock {
	/** Peripheral Clock */
	MAX_CAPI_WDT_CLOCK_PCLK = MXC_WDT_PCLK,
	/** Internal Baud Rate Oscillator */
	MAX_CAPI_WDT_CLOCK_IBRO = MXC_WDT_IBRO_CLK,
};

/**
 * @enum max_capi_wdt_mode
 * @brief Watchdog timer mode
 */
enum max_capi_wdt_mode {
	/** Compatibility mode (late interrupts/resets only) */
	MAX_CAPI_WDT_MODE_COMPATIBILITY = MXC_WDT_COMPATIBILITY,
	/** Windowed mode (includes early interrupts/resets) */
	MAX_CAPI_WDT_MODE_WINDOWED = MXC_WDT_WINDOWED,
};

/**
 * @struct max_capi_wdt_extra
 * @brief Maxim platform-specific extra configuration
 */
struct max_capi_wdt_extra {
	/** Clock source */
	enum max_capi_wdt_clock clock_source;
};

/**
 * @struct max_capi_wdt_chan_extra
 * @brief Maxim platform-specific WDT channel extra configuration
 */
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

/**
 * @brief Get the interrupt and reset flags from the WDT peripheral
 * @param handle The WDT handle
 * @param flags Where to store the flags
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_get_flags(struct capi_wdt_handle *handle, uint32_t *flags);

/**
 * @brief Clear the interrupt and reset flags from the WDT peripheral
 * @param handle The WDT handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_wdt_clear_flags(struct capi_wdt_handle *handle);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_WDT_H_ */
