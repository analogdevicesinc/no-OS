/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform I2C driver for CAPI
 *
 * Backends (select via config.ops):
 *   capi_i2c_xilinx_ps_ops  - XIicPs  (Zynq PS I2C)
 *   capi_i2c_xilinx_pl_ops  - XIic    (AXI IIC)
 *
 * Pass config->extra = NULL for polled mode. To use interrupts, pass
 * struct capi_i2c_xilinx_config with use_irq=true and the desired irq_id.
 * IRQ ID 0 is valid for INTC input 0.
 */

#ifndef _XILINX_CAPI_I2C_H_
#define _XILINX_CAPI_I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include <capi_i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct capi_i2c_xilinx_config
 * @brief Optional Xilinx platform-specific I2C configuration.
 *
 * Passed via config->extra during init. use_irq decides whether the driver
 * connects irq_id through the CAPI IRQ singleton. input_clock_hz optionally
 * enables PL XIic timing-register writes for designs that expose them.
 * IRQ ID 0 is valid for an INTC input, so it is passed through unchanged.
 */
struct capi_i2c_xilinx_config {
	/** Enable CAPI IRQ connection during init */
	bool use_irq;
	/** Normalized CAPI IRQ ID. Zero is valid for INTC input 0. */
	uint32_t irq_id;
	/** Clock feeding optional AXI IIC timing logic. Zero disables timing writes. */
	uint32_t input_clock_hz;
	/** Optional SCL-high timing register offset. Zero selects the driver default. */
	uint32_t thigh_offset;
	/** Optional SCL-low timing register offset. Zero selects the driver default. */
	uint32_t tlow_offset;
	/** Optional init-time bus rate. Zero leaves timing unchanged unless config asks. */
	uint32_t default_bus_hz;
	/** Optional init/config duty cycle percent. Zero selects 50%. */
	uint8_t default_duty_percent;
};

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_I2C_H_ */
