/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform Timer driver for CAPI
 *
 * Backends (select via config.ops):
 *   capi_timer_xilinx_pl_ops     - XTmrCtr  (AXI Timer, 2 channels)
 *   capi_timer_xilinx_ps_ttc_ops - XTtcPs   (Zynq PS TTC, 1 channel per instance)
 *   capi_timer_xilinx_ps_scu_ops - XScuTimer (PS SCU Private Timer, 1 channel)
 *
 * Pass config->extra = NULL for polled mode. To use interrupts, pass
 * struct capi_timer_xilinx_config with use_irq=true and the desired irq_id.
 * IRQ ID 0 is valid for INTC input 0.
 */

#ifndef _XILINX_CAPI_TIMER_H_
#define _XILINX_CAPI_TIMER_H_

#include <xparameters.h>
#include <stdbool.h>
#include <stdint.h>
#include <capi_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct capi_timer_xilinx_config
 * @brief Optional Xilinx platform-specific Timer configuration.
 *
 * Passed via config->extra during init. use_irq decides whether the driver
 * connects irq_id through the CAPI IRQ singleton. IRQ ID 0 is valid for an
 * INTC input, so it is passed through unchanged.
 */
struct capi_timer_xilinx_config {
	/** Enable CAPI IRQ connection during init */
	bool use_irq;
	/** Normalized CAPI IRQ ID. Zero is valid for INTC input 0. */
	uint32_t irq_id;
};

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
 * @brief Shared Xilinx timer handle.
 *
 * Base handle MUST be first member for container_of pattern.
 * Used by all three timer backends.
 */
struct capi_timer_xilinx_handle {
	/** Base handle - MUST be first */
	struct capi_timer_handle handle;
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
 * @brief Xilinx timer operations tables. Select one via config->ops.
 */
extern struct capi_timer_ops
	capi_timer_xilinx_ps_ttc_ops; /**< XTtcPs (Zynq/ZynqMP PS TTC) */
extern struct capi_timer_ops
	capi_timer_xilinx_ps_scu_ops; /**< XScuTimer (Cortex-A9 SCU timer) */
extern struct capi_timer_ops
	capi_timer_xilinx_pl_ops;     /**< XTmrCtr (AXI Timer in PL) */

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_TIMER_H_ */
