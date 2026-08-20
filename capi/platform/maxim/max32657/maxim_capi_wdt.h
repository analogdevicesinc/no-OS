/*******************************************************************************
 *   @file   maxim_capi_wdt.h
 *   @brief  Header file for WDT functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
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
#include <stdbool.h>

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
 * @note PCLK = 25 MHz, IBRO = 7.3728 MHz below
 */
enum max_capi_wdt_period {
	/** 2^31 ticks. PCLK ~ 85.90 s, IBRO ~ 291.3 s  */
	MAX_CAPI_WDT_PERIOD_2_31 = MXC_WDT_PERIOD_2_31,
	/** 2^30 ticks. PCLK ~ 42.95 s, IBRO ~ 145.6 s*/
	MAX_CAPI_WDT_PERIOD_2_30 = MXC_WDT_PERIOD_2_30,
	/** 2^29 ticks. PCLK ~ 21.47 s, IBRO ~ 72.82 s */
	MAX_CAPI_WDT_PERIOD_2_29 = MXC_WDT_PERIOD_2_29,
	/** 2^28 ticks. PCLK ~ 10.74 s, IBRO ~ 36.41 s */
	MAX_CAPI_WDT_PERIOD_2_28 = MXC_WDT_PERIOD_2_28,
	/** 2^27 ticks. PCLK ~ 5.349 s, IBRO ~ 18.20 s */
	MAX_CAPI_WDT_PERIOD_2_27 = MXC_WDT_PERIOD_2_27,
	/** 2^26 ticks. PCLK ~ 2.684 s, IBRO ~ 9.102 s */
	MAX_CAPI_WDT_PERIOD_2_26 = MXC_WDT_PERIOD_2_26,
	/** 2^25 ticks. PCLK ~ 1.342 s, IBRO ~ 4.551 s */
	MAX_CAPI_WDT_PERIOD_2_25 = MXC_WDT_PERIOD_2_25,
	/** 2^24 ticks. PCLK ~ 671.1 ms, IBRO ~ 2.276 s */
	MAX_CAPI_WDT_PERIOD_2_24 = MXC_WDT_PERIOD_2_24,
	/** 2^23 ticks. PCLK ~ 335.5 ms, IBRO ~ 1.138 s */
	MAX_CAPI_WDT_PERIOD_2_23 = MXC_WDT_PERIOD_2_23,
	/** 2^22 ticks. PCLK ~ 167.8 ms, IBRO ~ 568.9 ms */
	MAX_CAPI_WDT_PERIOD_2_22 = MXC_WDT_PERIOD_2_22,
	/** 2^21 ticks. PCLK ~ 83.89 ms, IBRO ~ 284.4 ms */
	MAX_CAPI_WDT_PERIOD_2_21 = MXC_WDT_PERIOD_2_21,
	/** 2^20 ticks. PCLK ~ 41.94 ms, IBRO ~ 142.2 ms */
	MAX_CAPI_WDT_PERIOD_2_20 = MXC_WDT_PERIOD_2_20,
	/** 2^19 ticks. PCLK ~ 20.97 ms, IBRO ~ 71.11 ms */
	MAX_CAPI_WDT_PERIOD_2_19 = MXC_WDT_PERIOD_2_19,
	/** 2^18 ticks. PCLK ~ 10.49 ms, IBRO ~ 35.56 ms */
	MAX_CAPI_WDT_PERIOD_2_18 = MXC_WDT_PERIOD_2_18,
	/** 2^17 ticks. PCLK ~ 5.243 ms, IBRO ~ 17.78 ms */
	MAX_CAPI_WDT_PERIOD_2_17 = MXC_WDT_PERIOD_2_17,
	/** 2^16 ticks. PCLK ~ 2.621 ms, IBRO ~ 8.8890 ms  */
	MAX_CAPI_WDT_PERIOD_2_16 = MXC_WDT_PERIOD_2_16,
};

/**
 * @enum max_capi_wdt_clock
 * @brief Clock selection
 */
enum max_capi_wdt_clock {
	/** Peripheral Clock (25 MHz) */
	MAX_CAPI_WDT_CLOCK_PCLK = MXC_WDT_PCLK,
	/** Internal Baud Rate Oscillator (7.3728 MHz) */
	MAX_CAPI_WDT_CLOCK_IBRO = MXC_WDT_IBRO_CLK,
};

/**
 * @enum max_capi_wdt_mode
 * @brief Watchdog timer mode
 */
enum max_capi_wdt_mode {
	/** Compatbility mode (late interrupts/resets only) */
	MAX_CAPI_WDT_MODE_COMPATIBILITY = MXC_WDT_COMPATIBILITY,
	/** Windowed mode (includes early interrupts/resets) */
	MAX_CAPI_WDT_MODE_WINDOWED = MXC_WDT_WINDOWED,
};

/**
 * @struct max_capi_wdt_extra
 * @brief MAX32657 platform-specific extra configuration
 */
struct max_capi_wdt_extra {
	/** Clock source */
	enum max_capi_wdt_clock clock_source;
};

/**
 * @struct max_capi_wdt_chan_extra
 * @brief MAX32657 platform-specific WDT channel extra configuration
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
