/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_CAPI_GPIO_H_
#define STM32_CAPI_GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi_gpio.h"

/**
 * STM32-specific GPIO pin flags (can be OR'd with CAPI_GPIO_* flags)
 */
#define STM32_GPIO_OPEN_DRAIN  0x02

/**
 * @struct stm32_capi_gpio_port_config
 * @brief STM32-specific GPIO port configuration for CAPI interface
 */
struct stm32_capi_gpio_port_config {
	/** Default output mode for pins (GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, etc.) */
	uint32_t mode;
	/** Default speed grade for pins */
	uint32_t speed;
	/** Default alternate functionality (for AF modes) */
	uint32_t alternate;
	/** Default pull configuration (GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN) */
	uint32_t pull;
};

/**
 * @struct stm32_capi_gpio_port_priv
 * @brief STM32 platform specific GPIO port private data
 */
struct stm32_capi_gpio_port_priv {
	/** STM32 GPIO port peripheral (GPIOA, GPIOB, etc.) */
	GPIO_TypeDef *port;
	/** Number of pins in this port */
	uint8_t num_pins;
	/** Current direction configuration for all pins (1=input, 0=output) */
	uint16_t direction_mask;
	/** Default configuration */
	struct stm32_capi_gpio_port_config default_config;
};

/**
 * @brief STM32 platform specific GPIO operations for CAPI
 */
extern const struct capi_gpio_ops stm32_capi_gpio_ops;

#endif /* STM32_CAPI_GPIO_H_ */
