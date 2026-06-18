/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform GPIO private driver contract.
 */

#ifndef _XILINX_CAPI_GPIO_PRIV_H_
#define _XILINX_CAPI_GPIO_PRIV_H_

#include <xilinx_capi_gpio.h>

#include "xparameters.h"
#ifdef XPAR_XGPIOPS_NUM_INSTANCES
#include "xgpiops.h"
#endif
#ifdef XPAR_XGPIO_NUM_INSTANCES
#include "xgpio.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct capi_gpio_xilinx_port_handle
 * @brief Xilinx GPIO private port state.
 */
struct capi_gpio_xilinx_port_handle {
	/** Xilinx driver instance (XGpioPs* or XGpio*) */
	void *instance;
	/** Starting global XGpioPs pin number (PS only) */
	uint8_t base_pin;
	/** AXI GPIO channel (PL only: 1 or 2) */
	uint8_t channel;
	/** Number of pins in this port */
	uint8_t num_pins;
	/** Cached direction bitmask (CAPI: 0=output, 1=input) */
	uint64_t direction;
	/** Per-pin flags array (malloc'd to num_pins size) */
	uint32_t *pin_flags;
	/** Cached active-low mask (computed once at init from pin_flags) */
	uint64_t active_low_mask;
};

/**
 * @brief Declare a stack-allocated Xilinx GPIO port handle.
 *
 * Declares `name` (struct capi_gpio_port_handle) with embedded private
 * state. Pass &name to capi_gpio_init().
 */
#define CAPI_GPIO_PORT_HANDLE_XILINX_DEFINE(name)                              \
	struct capi_gpio_port_handle name = {                                  \
		.ops = NULL,                                                   \
		.init_allocated = false,                                       \
		.priv = &(struct capi_gpio_xilinx_port_handle){0}              \
	}

/**
 * @brief Xilinx GPIO operations tables. Select one via config->ops.
 */
extern struct capi_gpio_ops capi_gpio_xilinx_ps_ops;
extern struct capi_gpio_ops capi_gpio_xilinx_pl_ops;

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_GPIO_PRIV_H_ */
