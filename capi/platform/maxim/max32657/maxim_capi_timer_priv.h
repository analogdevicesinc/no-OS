/*******************************************************************************
 *   @file   maxim_capi_timer_priv.h
 *   @brief  Header file for Timer private handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_TIMER_PRIV_H_
#define MAXIM_CAPI_TIMER_PRIV_H_

#include "maxim_capi_timer.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

#define MAX_CAPI_PWM_TMR_MAX_VAL	0x0000FFFF
#define MAX_CAPI_PWM_PRESCALER_VAL(n)	((n - 1U) * 16)
#define MAX_CAPI_PWM_PRESCALER_TRUE(n)	(1U << ((n) / 16))
#define MAX_CAPI_PWM_GET_PRESCALER(n)	(1U << ((n) - 1))
#define MAX_CAPI_MAX_PRESCALER_INDEX	13U	/* 2^(13 - 1) = 4096 */

/**
 * @struct max_capi_timer_channel_state
 * @brief State data for Timer channels
 */
struct max_capi_timer_channel_state {
	/** Timer config */
	mxc_tmr_cfg_t config;
	/** PWM mode parameters */
	struct {
		/** PWM period in ticks */
		uint32_t period_ticks;
		/** Duty cycle in ticks */
		uint32_t duty_cycle_ticks;
	} pwm;
	/** Compare mode parameters */
	struct {
		/** Compare match value */
		uint32_t value;
	} compare;
	/** GPIO initialization flag */
	bool init_pin;
	/** Alternate pin selection */
	bool use_alternate_pin;
	/** GPIO voltage selection */
	enum max_capi_gpio_vssel vssel;
	/** GPIO already initialized flag */
	bool gpio_initialized;
	/** Channel-specific event callback */
	capi_timer_channel_callback callback;
	/** Event callback arg */
	void *callback_arg;
	/** Channel event enable mask */
	uint32_t events_enabled;
	/** Timer enabled */
	bool enabled;
};

/**
 * @struct max_capi_timer_priv
 * @brief Private structure for Timer
 */
struct max_capi_timer_priv {
	/** Timer ID */
	uint32_t identifier;
	/** Timer frequency */
	uint32_t frequency;
	/** 32-bit or dual 16-bit mode */
	enum max_capi_timer_bit_mode bit_mode;
	/** Timer A/B state */
	struct max_capi_timer_channel_state *channel[2];
	/** Global timer event callback */
	capi_timer_event_callback global_callback;
	/** Global timer event callback arg */
	void *global_callback_arg;
	/** Bitmask for enabled global events */
	uint32_t global_events_enabled;
	/** Clock source */
	enum max_capi_timer_clock_source clock_source;
	/** Clock source frequency */
	uint32_t clock_freq_hz;
	/** MSDK clock storage */
	mxc_tmr_clock_t msdk_clock;
	/** Interrupt-driven events enabled */
	bool use_irq;
};

#define CAPI_TIMER_HANDLE_MAXIM_INIT()				\
	(&(struct capi_timer_handle) {				\
		.ops = NULL,					\
		.init_allocated = false,			\
		.priv = &(struct max_capi_timer_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_TIMER_PRIV_H_ */
