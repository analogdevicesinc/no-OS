/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform GPIO driver for CAPI
 *
 * Supported Xilinx GPIO peripherals:
 *   - XGpioPs: Zynq PS GPIO (MIO/EMIO)
 *   - XGpio: AXI GPIO (Programmable Logic)
 */

#ifndef _XILINX_CAPI_GPIO_H_
#define _XILINX_CAPI_GPIO_H_

#include "xparameters.h"
#include <stdbool.h>
#include <stdint.h>
#include <capi_gpio.h>

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
 * Platform-specific max pins (for validation only):
 *   - Zynq-7000 :        118 pins (4 banks)
 *   - Zynq Ultrascale+ : 174 pins (6 banks)
 *   - Versal PMC :       116 pins (4 banks)
 *   - Versal PS :        58 pins  (2 banks)
 *   - AXI GPIO :         32 pins per channel (max 2 channels)
 */

/**
 * @brief Xilinx platform-specific GPIO configuration.
 *
 * Passed via config->extra during init. Use the struct matching the backend
 * selected via config->ops.
 */
struct capi_gpio_xilinx_ps_config {
	/** Starting global XGpioPs pin number. EMIO starts at 54 on Zynq-7000 and 78 on ZynqMP. */
	uint8_t base_pin;
};

struct capi_gpio_xilinx_pl_config {
	/** AXI GPIO channel: 1 or 2 (required) */
	uint8_t channel;
};

/**
 * @struct capi_gpio_xilinx_port_handle
 * @brief Xilinx GPIO port handle.
 *
 * Embeds the base CAPI handle as first member for safe casting.
 */
struct capi_gpio_xilinx_port_handle {
	/** Base handle - MUST be first for container_of pattern */
	struct capi_gpio_port_handle handle;
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
 * @brief Xilinx GPIO operations tables. Select one via config->ops.
 */
extern struct capi_gpio_ops
	capi_gpio_xilinx_ps_ops;  /**< XGpioPs (Zynq PS MIO/EMIO) */
extern struct capi_gpio_ops
	capi_gpio_xilinx_pl_ops;  /**< XGpio (AXI GPIO in PL) */

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_GPIO_H_ */
