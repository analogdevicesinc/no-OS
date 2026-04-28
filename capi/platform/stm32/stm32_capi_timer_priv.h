/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STM32_CAPI_TIMER_PRIV_H_
#define _STM32_CAPI_TIMER_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "stm32_capi_timer.h"

#ifdef HAL_TIM_MODULE_ENABLED

/**
 * @brief Per-channel state for STM32 timer
 */
struct stm32_capi_timer_channel_state {
	/** Channel mode */
	uint32_t mode;
	/** Channel enabled flag */
	bool enabled;
	/** Channel-specific callback */
	capi_timer_channel_callback callback;
	/** Callback argument */
	void *callback_arg;
};

/**
 * @brief STM32 platform specific timer private data
 */
struct stm32_capi_timer_priv {
	/** STM32 HAL timer handle */
	TIM_HandleTypeDef htim;
	/** Timer input clock frequency in Hz */
	uint64_t input_clock_hz;
	/** Timer output/counting frequency in Hz */
	uint64_t output_freq_hz;
	/** Counter direction */
	uint32_t direction;
	/** Counter minimum value */
	uint32_t counter_min;
	/** Counter maximum value */
	uint32_t counter_max;
	/** Rollover enabled */
	bool rollover;
	/** IRQ number */
	uint32_t irq_num;
	/** Global event callback */
	capi_timer_event_callback event_callback;
	/** Global event callback argument */
	void *event_callback_arg;
	/** Channel states */
	struct stm32_capi_timer_channel_state channels[STM32_TIMER_MAX_CHANNELS];
};

#define CAPI_TIMER_HANDLE_STM32_INIT() \
	(&(struct capi_timer_handle){ \
		.ops = NULL, \
		.init_allocated = false, \
		.priv = &(struct stm32_capi_timer_priv){0}})

#endif /* HAL_TIM_MODULE_ENABLED */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_TIMER_PRIV_H_ */
