/*******************************************************************************
 *   @file   maxim_capi_timer.h
 *   @brief  Public interface for the common Maxim (MSDK) CAPI Timer/PWM backend
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

/*
 * Per-part capability: how many timer instances expose an MSDK output pin
 * (gpio_cfg_tmrN / gpio_cfg_tmrNb in <part>/mxc_pins.h).
 *
 * This is a package/pin-mux fact, so it is gated on TARGET_NUM -- NOT on
 * MXC_CFG_TMR_INSTANCES, which also counts low-power timers that have no output
 * pin (e.g. MAX32655 reports 6 instances but pins only tmr0..3). The numbered
 * (primary) and alternate ('b') pins are counted separately because parts pin
 * different subsets of the two.
 *
 * Timers 0..3 and their 'b' alternates are present on every part surveyed
 * (MAX32655/57/90), so they are the conservative default for an unlisted part;
 * add a row when a new part is brought up on hardware.
 */
#if (TARGET_NUM == 32690)
#define MAX_CAPI_TIMER_PIN_COUNT      6   /* tmr0..tmr5      */
#define MAX_CAPI_TIMER_ALT_PIN_COUNT  4   /* tmr0b..tmr3b    */
#elif (TARGET_NUM == 32657)
#define MAX_CAPI_TIMER_PIN_COUNT      6   /* tmr0..tmr5      */
#define MAX_CAPI_TIMER_ALT_PIN_COUNT  6   /* tmr0b..tmr5b    */
#else /* MAX32655 and the conservative default */
#define MAX_CAPI_TIMER_PIN_COUNT      4   /* tmr0..tmr3      */
#define MAX_CAPI_TIMER_ALT_PIN_COUNT  4   /* tmr0b..tmr3b    */
#endif

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
 * @brief Maxim platform-specific timer modes beyond the default CAPI modes
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
 * @brief Maxim platform-specific extra configuration
 */
struct max_capi_timer_extra {
	/** Timer bit mode (32-bit or 16-bit dual) */
	enum max_capi_timer_bit_mode bit_mode;
	/** Enable interrupt-driven events (connect and enable the timer IRQ) */
	bool use_irq;
};

/**
 * @struct max_capi_timer_channel_extra
 * @brief Maxim platform-specific timer channel extra configuration
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
 * @brief Maxim platform-specific counter extra configuration
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
