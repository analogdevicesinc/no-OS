/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_gpio.h
 * @brief MAX32655 platform-specific GPIO CAPI implementation header
 * @author Claude Code (noreply@anthropic.com)
 */

#ifndef MAXIM_CAPI_GPIO_H_
#define MAXIM_CAPI_GPIO_H_

#include "capi_gpio.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private handle structure for MAX32655 GPIO CAPI implementation
 */
struct maxim_capi_gpio_handle {
	struct capi_gpio_port_handle capi_handle;  /**< Must be first field */
	mxc_gpio_regs_t *gpio_regs;               /**< Maxim GPIO port registers */
	uint32_t port_id;                         /**< GPIO port ID */
	uint8_t num_pins;                         /**< Number of pins in port */
	uint32_t *pin_flags;                      /**< Pin flags array */
};

#define CAPI_GPIO_HANDLE &(struct capi_gpio_port_handle){	\
		.priv = &(struct maxim_capi_gpio_handle){}	\
	}

/**
 * @brief MAX32655 GPIO CAPI operations structure
 *
 * This structure contains function pointers for all GPIO CAPI operations
 * specific to the MAX32655 platform.
 */
extern const struct capi_gpio_ops maxim_capi_gpio_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_GPIO_H_ */