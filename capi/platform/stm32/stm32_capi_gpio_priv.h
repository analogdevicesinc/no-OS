/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STM32_CAPI_GPIO_PRIV_H_
#define _STM32_CAPI_GPIO_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "stm32_capi_gpio.h"

/**
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

#define CAPI_GPIO_PORT_HANDLE_STM32_INIT() \
	(&(struct capi_gpio_port_handle){ \
		.ops = NULL, \
		.init_allocated = false, \
		.priv = &(struct stm32_capi_gpio_port_priv){0}})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_GPIO_PRIV_H_ */
