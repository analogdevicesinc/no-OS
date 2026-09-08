/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_i2c_priv.h
 * @brief Private I2C handle for the common Maxim backend.
 *
 * Controller-mode async is driven by the MSDK interrupt engine
 * (MXC_I2C_MasterTransactionAsync + MXC_I2C_AsyncHandler), so this backend
 * keeps only one in-flight request slot and a small bookkeeping block.
 * Target (peripheral) mode reuses that slot's async_in_progress flag and adds
 * its own RX/TX buffer bookkeeping, driven by the MSDK slave FSM.
 */

#ifndef MAXIM_CAPI_I2C_PRIV_H_
#define MAXIM_CAPI_I2C_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "i2c.h"
#include "maxim_capi_i2c.h"

/**
 * @struct max_capi_i2c_priv
 * @brief Private structure for the common Maxim I2C backend.
 */
struct max_capi_i2c_priv {
	/** I2C controller handle (back-pointer). */
	struct capi_i2c_controller_handle *i2c_handle;
	/** MSDK register block for this instance. */
	mxc_i2c_regs_t *regs;
	/** I2C instance index (MXC_I2C_GET_IDX). */
	uint32_t identifier;
	/** OPTIONAL - DMA controller handle (reserved). */
	struct capi_dma_handle *dma_handle;
	/** Platform-specific config. */
	struct max_capi_i2c_extra *extra;
	/** Bus clock frequency in Hz. */
	uint32_t freq;
	/** CAPI completion callback. */
	capi_i2c_callback callback;
	/** Callback argument. */
	void *callback_arg;
	/** In-flight async request (owned by MSDK while active). */
	mxc_i2c_req_t async_req;
	/** Heap buffer joined from sub_address+data for an async write (freed on completion). */
	uint8_t *async_combined;
	/** Async transfer in progress. */
	volatile bool async_in_progress;
	/** Target (peripheral) mode active on this instance. */
	volatile bool is_target;
	/** Target-mode RX buffer, capacity, and running byte count. */
	uint8_t *tgt_rx_buf;
	uint32_t tgt_rx_len;
	volatile uint32_t tgt_rx_cnt;
	/** Target-mode TX buffer, length, and running byte count. */
	uint8_t *tgt_tx_buf;
	uint32_t tgt_tx_len;
	volatile uint32_t tgt_tx_cnt;
};

#define CAPI_I2C_CONTROLLER_HANDLE_MAXIM_INIT()		\
	(&(struct capi_i2c_controller_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_i2c_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_I2C_PRIV_H_ */
