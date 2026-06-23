/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_CAPI_PWM_H_
#define STM32_CAPI_PWM_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi_timer.h"

#ifdef HAL_TIM_MODULE_ENABLED

/**
 * @struct stm32_capi_pwm_config
 * @brief Configuration for STM32 CAPI PWM driver
 *
 * This driver supports basic TIM-based PWM. Advanced features such as LPTIM,
 * slave/master timer modes, DMA-driven PWM, complementary channels, and
 * one-pulse mode are not supported.
 */
struct stm32_capi_pwm_config {
	/** TIM peripheral number (1-17) or base address */
	uint32_t timer_id;
	/** Channel index 0-3 (maps to TIM_CHANNEL_1 through TIM_CHANNEL_4) */
	uint32_t channel;
	/** PWM period in nanoseconds */
	uint64_t period_ns;
	/** Active (high) time in nanoseconds */
	uint64_t duty_cycle_ns;
	/** true = idle high / active low, false = idle low / active high */
	bool inverted_polarity;
	/** Input clock frequency in Hz (0 = auto-detect from RCC) */
	uint64_t input_clock_hz;
	/** Optional function returning the timer input clock (overrides input_clock_hz) */
	uint32_t (*get_input_clock)(void);
};

/**
 * @struct stm32_capi_pwm_desc
 * @brief Runtime descriptor for an active PWM instance
 */
struct stm32_capi_pwm_desc {
	/** Underlying CAPI timer handle */
	struct capi_timer_handle *timer;
	/** Channel index (0-3) */
	uint32_t channel;
	/** Current period in nanoseconds */
	uint64_t period_ns;
	/** Current duty cycle (active time) in nanoseconds */
	uint64_t duty_cycle_ns;
	/** Current polarity setting */
	bool inverted_polarity;
	/** Whether the PWM output is currently enabled */
	bool enabled;
};

int stm32_capi_pwm_init(struct stm32_capi_pwm_desc **desc,
			const struct stm32_capi_pwm_config *config);

int stm32_capi_pwm_remove(struct stm32_capi_pwm_desc *desc);

int stm32_capi_pwm_enable(struct stm32_capi_pwm_desc *desc);

int stm32_capi_pwm_disable(struct stm32_capi_pwm_desc *desc);

int stm32_capi_pwm_set_period(struct stm32_capi_pwm_desc *desc,
			      uint64_t period_ns);

int stm32_capi_pwm_get_period(struct stm32_capi_pwm_desc *desc,
			      uint64_t *period_ns);

int stm32_capi_pwm_set_duty_cycle(struct stm32_capi_pwm_desc *desc,
				  uint64_t duty_cycle_ns);

int stm32_capi_pwm_get_duty_cycle(struct stm32_capi_pwm_desc *desc,
				  uint64_t *duty_cycle_ns);

int stm32_capi_pwm_set_polarity(struct stm32_capi_pwm_desc *desc,
				bool inverted);

int stm32_capi_pwm_get_polarity(struct stm32_capi_pwm_desc *desc,
				bool *inverted);

#endif /* HAL_TIM_MODULE_ENABLED */

#endif /* STM32_CAPI_PWM_H_ */
