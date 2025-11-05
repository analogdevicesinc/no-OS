/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_timer.h
 * @brief MAX32655 Timer CAPI implementation header
 * @author Claude Code (noreply@anthropic.com)
 */

#ifndef MAXIM_CAPI_TIMER_H
#define MAXIM_CAPI_TIMER_H

#include "capi_timer.h"
#include "tmr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MAX32655 Timer CAPI private handle structure
 */
struct maxim_capi_timer_handle {
	/** Timer instance identifier (0-3) */
	uint32_t instance;
	/** Timer registers pointer */
	mxc_tmr_regs_t *regs;
	/** Timer configuration */
	mxc_tmr_cfg_t config;
	/** Input clock frequency in Hz */
	uint32_t input_clock_hz;
	/** Output frequency in Hz */
	uint32_t output_freq_hz;
	/** Channel configurations */
	struct capi_timer_channel_config channels[4];
	/** Channel callbacks */
	capi_timer_callback callbacks[4];
	/** Callback arguments */
	void *callback_args[4];
	/** Channel enable status */
	bool channel_enabled[4];
};

/**
 * @brief Static allocation macro for MAX32655 Timer CAPI handle
 *
 * Usage:
 *   struct capi_timer_handle *handle = CAPI_TIMER_HANDLE;
 */
#define CAPI_TIMER_HANDLE &(struct capi_timer_handle){ \
	.priv = &(struct maxim_capi_timer_handle){} \
}

/**
 * @brief MAX32655 Timer CAPI operations structure
 */
extern const struct capi_timer_ops maxim_capi_timer_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_TIMER_H */