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

#include <stdbool.h>
#include <stdint.h>
#include <capi_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform-specific max pins :
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

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_GPIO_H_ */
