/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_CAPI_TIMER_H_
#define STM32_CAPI_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi_timer.h"

#ifdef HAL_TIM_MODULE_ENABLED

/** Maximum number of timer channels */
#define STM32_TIMER_MAX_CHANNELS	4

/**
 * @struct stm32_capi_timer_extra_config
 * @brief STM32-specific timer configuration for CAPI interface
 */
struct stm32_capi_timer_extra_config {
	/** Pre-initialized TIM handle (optional - if NULL, driver will init) */
	TIM_HandleTypeDef *htim;
	/** Get input clock frequency function (optional) */
	uint32_t (*get_input_clock)(void);
	/** IRQ number for the timer (0 to disable IRQ) */
	uint32_t irq_num;
};

/**
 * @brief STM32 platform specific timer operations for CAPI
 */
extern const struct capi_timer_ops stm32_capi_timer_ops;

#endif /* HAL_TIM_MODULE_ENABLED */

#endif /* STM32_CAPI_TIMER_H_ */
