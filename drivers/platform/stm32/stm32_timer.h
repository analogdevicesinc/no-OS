/***************************************************************************//**
 *   @file   stm32/stm32_timer.h
 *   @brief  Header file for the stm32 timer driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_TIMER_H_
#define STM32_TIMER_H_

#include <stdint.h>
#include "no_os_timer.h"
#include "stm32_hal.h"

/**
 * @struct stm32_timer_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific timer parameters.
 */
struct stm32_timer_init_param {
	TIM_HandleTypeDef *htimer;
};

/**
 * @struct stm32_timer_desc
 * @brief stm32 platform specific timer descriptor
 */
struct stm32_timer_desc {
	/** timer instance */
	TIM_HandleTypeDef *htimer;
};

/**
 * @brief stm32 specific timer platform ops structure
 */
extern const struct no_os_timer_platform_ops stm32_timer_ops;

#endif // STM32_TIMER_H_
