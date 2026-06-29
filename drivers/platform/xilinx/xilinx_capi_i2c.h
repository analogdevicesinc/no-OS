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

#include "xparameters.h"
#include <stdbool.h>
#include <stdint.h>
#include <capi_i2c.h>

#ifdef XPAR_XIICPS_NUM_INSTANCES
#include "xiicps.h"
#endif

#ifdef XPAR_XIIC_NUM_INSTANCES
#include "xiic.h"
#include "xiic_l.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct capi_i2c_xilinx_config
 * @brief Optional Xilinx platform-specific I2C configuration.
 *
 * Passed via config->extra during init. use_irq decides whether the driver
 * connects irq_id through the CAPI IRQ singleton. IRQ ID 0 is valid for an
 * INTC input, so it is passed through unchanged.
 */
struct capi_i2c_xilinx_config {
	/** Enable CAPI IRQ connection during init */
	bool use_irq;
	/** Normalized CAPI IRQ ID. Zero is valid for INTC input 0. */
	uint32_t irq_id;
};

/**
 * @enum capi_i2c_xilinx_async_phase
 * @brief Internal async operation phase tracking.
 */
enum capi_i2c_xilinx_async_phase {
	CAPI_I2C_ASYNC_IDLE = 0,
	CAPI_I2C_ASYNC_RX_SUBADDR,
	CAPI_I2C_ASYNC_TX,
	CAPI_I2C_ASYNC_RX,
};

/**
 * @struct capi_i2c_xilinx_handle
 * @brief Shared Xilinx I2C controller handle.
 *
 * Base handle MUST be first for container_of pattern.
 * Used by both PS and PL backends.
 */
struct capi_i2c_xilinx_handle {
	/** Base handle - MUST be first */
	struct capi_i2c_controller_handle handle;
	/** Xilinx driver instance (XIicPs* or XIic*) */
	void *instance;
	/** True if controller is initiator (master) */
	bool is_initiator;
	/** Last-configured SCL rate in Hz */
	uint32_t clk_freq_hz;
	/** IRQ ID (only valid if use_irq is true) */
	uint32_t irq_id;
	/** True if IRQ was successfully connected */
	bool use_irq;
	/** Target address when in slave mode */
	uint16_t target_addr;
	/** User callback for async operations */
	capi_i2c_callback callback;
	/** User callback argument */
	void *callback_arg;
	/** Async transfer done flag */
	volatile bool xfer_done;
	/** Async transfer status (0=success, <0=error) */
	volatile int xfer_status;
	/** Temp buffer for async transmit with sub-address (NULL = not allocated) */
	uint8_t *async_tx_temp;
	/** Current async operation phase */
	enum capi_i2c_xilinx_async_phase async_phase;
	/** Pending receive buffer (for phase 2 of rx with sub_address) */
	uint8_t *async_rx_buf;
	/** Pending receive length */
	uint32_t async_rx_len;
	/** Device address for pending receive */
	uint16_t async_dev_addr;
	/** no_stop flag for pending receive */
	bool async_no_stop;
};

/**
 * @brief Xilinx I2C operations tables. Select one via config->ops.
 */
extern struct capi_i2c_ops
	capi_i2c_xilinx_ps_ops;  /**< XIicPs (Zynq/ZynqMP PS I2C) */
extern struct capi_i2c_ops capi_i2c_xilinx_pl_ops;  /**< XIic (AXI IIC in PL) */


#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_I2C_H_ */
