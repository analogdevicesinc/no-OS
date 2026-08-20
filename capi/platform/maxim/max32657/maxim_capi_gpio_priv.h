/***************************************************************************//**
 *   @file   maxim_capi_gpio_priv.h
 *   @brief  Header file for the GPIO private handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_GPIO_PRIV_H_
#define MAXIM_CAPI_GPIO_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "maxim_capi_gpio.h"

/**
 * @struct max_capi_gpio_port_priv
 * @brief Private structure for the GPIO port
 */
struct max_capi_gpio_port_priv {
	/** Identifier */
	uint32_t id;
	/** The GPIO port */
	mxc_gpio_regs_t *port;
	/** Number of pins on the GPIO port */
	uint8_t num_pins;
	/** Bitmask for the GPIO port pins */
	uint32_t pin_mask;
	/** Bitmask for the GPIO port pin directions */
	uint32_t direction_mask;
	/** Platform-specific extra configuration */
	struct max_capi_gpio_extra_config extra;
};

#define CAPI_GPIO_HANDLE_MAXIM_INIT()				\
	(&(struct capi_gpio_port_handle) {			\
		.ops = NULL,					\
		.init_allocated = false,			\
		.priv = &(struct max_capi_gpio_port_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_GPIO_PRIV_H_ */
