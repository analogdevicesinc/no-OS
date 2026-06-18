/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform Timer private driver contract.
 */

#ifndef _XILINX_CAPI_TIMER_PRIV_H_
#define _XILINX_CAPI_TIMER_PRIV_H_

#include <xilinx_capi_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct capi_timer_xilinx_channel
 * @brief Per-channel state tracking.
 */
struct capi_timer_xilinx_channel {
	/** Channel has been initialized */
	bool initialized;
	/** Channel is running */
	bool enabled;
	/** Current mode */
	enum capi_timer_channel_mode mode;
	/** User callback */
	capi_timer_channel_callback callback;
	/** Callback argument */
	void *callback_arg;
	/** IRQ enabled for this channel */
	bool use_irq;
};

/**
 * @struct capi_timer_xilinx_handle
 * @brief Shared Xilinx timer private state.
 */
struct capi_timer_xilinx_handle {
	/** XTmrCtr*, XTtcPs*, or XScuTimer* instance */
	void *instance;
	/** Input clock frequency in Hz */
	uint32_t input_clock_hz;
	/** IRQ ID (only valid if use_irq is true) */
	uint32_t irq_id;
	/** True if IRQ was successfully connected during init */
	bool use_irq;
	/** Global timer event IRQ is logically enabled */
	bool event_irq_enabled;
	/** Number of channels actually used by this backend (set at init) */
	uint8_t num_channels;
	/** Per-channel state - sized for the widest backend (AXI: 2 channels) */
	struct capi_timer_xilinx_channel channels[2];
	/** Global timer event callback */
	capi_timer_event_callback event_callback;
	/** Event callback argument */
	void *event_callback_arg;
};

/**
 * @brief Declare a stack-allocated Xilinx Timer handle.
 *
 * Declares `name` (struct capi_timer_handle) with embedded private state.
 * Pass &name to capi_timer_init().
 */
#define CAPI_TIMER_HANDLE_XILINX_DEFINE(name)                                  \
	struct capi_timer_handle name = {                                      \
		.ops = NULL,                                                   \
		.init_allocated = false,                                       \
		.priv = &(struct capi_timer_xilinx_handle){0}                  \
	}

/**
 * @brief Xilinx timer operations tables. Select one via config->ops.
 */
extern struct capi_timer_ops capi_timer_xilinx_ps_ttc_ops;
extern struct capi_timer_ops capi_timer_xilinx_ps_scu_ops;
extern struct capi_timer_ops capi_timer_xilinx_pl_ops;

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_TIMER_PRIV_H_ */
