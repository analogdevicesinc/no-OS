/*******************************************************************************
 *   @file   maxim_capi_timer.h
 *   @brief  Header file for Timer and PWM functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_TIMER_H_
#define MAXIM_CAPI_TIMER_H_

#include "capi_timer.h"
#include "tmr.h"
#include "maxim_capi_gpio.h"
#include "capi_alloc.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/**
 * @enum max_capi_timer_clock_source
 * @brief Clock source selection
 */
enum max_capi_timer_clock_source {
	/** Peripheral clock */
	MAX_CAPI_TIMER_CLOCK_APB,
	/** External clock input */
	MAX_CAPI_TIMER_CLOCK_EXTERNAL,
	/** Internal baud rate oscillator (7.3728 MHz) */
	MAX_CAPI_TIMER_CLOCK_IBRO,
	/** External RTC oscillator (32.768 kHz) */
	MAX_CAPI_TIMER_CLOCK_ERTCO,
	/** Internal nano-ring oscillator */
	MAX_CAPI_TIMER_CLOCK_INRO,
	/** IBRO divided by 8 (921.6 kHz) */
	MAX_CAPI_TIMER_CLOCK_IBRO_DIV8,
};

/**
 * @enum max_capi_timer_bit_mode
 * @brief Counter bit-mode selection
 */
enum max_capi_timer_bit_mode {
	/** 32-bit timer mode */
	MAX_CAPI_TIMER_BIT_MODE_32BIT,
	/** 16-bit timer mode */
	MAX_CAPI_TIMER_BIT_MODE_16BIT_DUAL,
};

/**
 * @enum max_capi_timer_channel_mode
 * @brief MAX32657 platform-specific timer modes beyond default CAPI
 */
enum max_capi_timer_channel_mode {
	MAX_CAPI_TIMER_MODE_ONESHOT = CAPI_TIMER_CHANNEL_MODE_LIMIT,
	MAX_CAPI_TIMER_MODE_CONTINUOUS,
	MAX_CAPI_TIMER_MODE_COUNTER,
	MAX_CAPI_TIMER_MODE_GATED,
	MAX_CAPI_TIMER_MODE_CAPTURE_COMPARE,
	MAX_CAPI_TIMER_MODE_DUAL_EDGE,
};

/**
 * @struct max_capi_timer_extra
 * @brief MAX32657 platform-specific extra configuration
 */
struct max_capi_timer_extra {
	/** Timer bit mode (32-bit or 16-bit dual) */
	enum max_capi_timer_bit_mode bit_mode;
	/** Enable interrupt-driven events (connect and enable the timer IRQ) */
	bool use_irq;
};

/**
 * @struct max_capi_timer_channel_extra
 * @brief MAX32657 platform-specific timer channel extra configuration
 */

struct max_capi_timer_channel_extra {
	/** GPIO voltage selection */
	enum max_capi_gpio_vssel vssel;
	/** GPIO initialization flag */
	bool init_pin;
	/** Alternate pin selection */
	bool use_alternate_pin;
};

/**
 * @struct max_capi_timer_counter_extra
 * @brief MAX32657 platform-specific counter extra configuration
 */
struct max_capi_timer_counter_extra {
	/** GPIO voltage selection */
	enum max_capi_gpio_vssel vssel;
	/** GPIO initialization flag */
	bool init_pin;
	/** Alternate pin selection */
	bool use_alternate_pin;
};

extern struct capi_timer_ops max_capi_timer_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_TIMER_H_ */
