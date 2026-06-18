/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform I2C private driver contract.
 */

#ifndef _XILINX_CAPI_I2C_PRIV_H_
#define _XILINX_CAPI_I2C_PRIV_H_

#include <xilinx_capi_i2c.h>

#include "xparameters.h"
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
 * @brief Shared Xilinx I2C private controller state.
 */
struct capi_i2c_xilinx_handle {
	/** Xilinx driver instance (XIicPs* or XIic*) */
	void *instance;
	/** True if controller is initiator (master) */
	bool is_initiator;
	/** Last-configured SCL rate in Hz */
	uint32_t clk_freq_hz;
	/** Clock feeding optional PL timing registers; zero means unsupported */
	uint32_t pl_input_clock_hz;
	/** Optional PL SCL-high timing register offset */
	uint32_t pl_thigh_offset;
	/** Optional PL SCL-low timing register offset */
	uint32_t pl_tlow_offset;
	/** Default PL timing duty cycle percent */
	uint8_t pl_default_duty_percent;
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
 * @brief Declare a stack-allocated Xilinx I2C controller handle.
 *
 * Declares `name` (struct capi_i2c_controller_handle) with embedded
 * private state. Pass &name to capi_i2c_init().
 */
#define CAPI_I2C_HANDLE_XILINX_DEFINE(name)								\
	struct capi_i2c_controller_handle name = {                          \
		.ops = NULL,                                                    \
		.init_allocated = false,                                        \
		.priv = &(struct capi_i2c_xilinx_handle){0}                     \
	}

/**
 * @brief Xilinx I2C operations tables. Select one via config->ops.
 */
extern struct capi_i2c_ops capi_i2c_xilinx_ps_ops;
extern struct capi_i2c_ops capi_i2c_xilinx_pl_ops;

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_I2C_PRIV_H_ */
