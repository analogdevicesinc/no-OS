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

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_TIMER_H_ */
