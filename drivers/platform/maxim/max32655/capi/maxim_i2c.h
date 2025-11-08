/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_i2c.h
 * @brief MAX32655 I2C CAPI implementation header
 * @author Claude Code (noreply@anthropic.com)
 */

#ifndef MAXIM_CAPI_I2C_H
#define MAXIM_CAPI_I2C_H

#include "capi_i2c.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MAX32655 I2C CAPI private handle structure
 */
struct maxim_capi_i2c_handle {
	/** I2C instance identifier (0-2) */
	uint32_t instance;
	/** I2C registers pointer */
	mxc_i2c_regs_t *regs;
	/** I2C request structure */
	mxc_i2c_req_t req;
	/** Clock frequency in Hz */
	uint32_t clk_freq_hz;
	/** Controller mode: true=initiator, false=target */
	bool initiator;
	/** Target device address (when acting as target) */
	uint8_t address;
	/** Callback function */
	capi_i2c_callback callback;
	/** Callback argument */
	void *callback_arg;
	/** Current transfer in progress */
	bool transfer_active;
};

/**
 * @brief Static allocation macro for MAX32655 I2C CAPI handle
 *
 * Usage:
 *   struct capi_i2c_controller_handle *handle = CAPI_I2C_HANDLE;
 */
#define CAPI_I2C_HANDLE &(struct capi_i2c_controller_handle){ \
	.priv = &(struct maxim_capi_i2c_handle){} \
}

/**
 * @brief MAX32655 I2C CAPI operations structure
 */
extern const struct capi_i2c_ops maxim_i2c_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_I2C_H */