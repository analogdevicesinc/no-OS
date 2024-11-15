/***************************************************************************//**
 *   @file   stm32/stm32_gpio.h
 *   @brief  Header file for stm32 gpio specifics.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_GPIO_H_
#define STM32_GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"

/**
 * @struct stm32_gpio_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 */
struct stm32_gpio_init_param {
	/** Output mode */
	uint32_t mode;
	/** Speed grade */
	uint32_t speed;
	/** Alternate functionality */
	uint32_t alternate;
};

/**
 * @struct stm32_gpio_desc
 * @brief stm32 platform specific gpio descriptor
 */
struct stm32_gpio_desc {
	/** Port */
	GPIO_TypeDef *port;
	/** GPIO configuration */
	GPIO_InitTypeDef gpio_config;
};

/**
 * @brief stm32 platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops stm32_gpio_ops;

#endif
