/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STM32_CAPI_I2C_PRIV_H_
#define _STM32_CAPI_I2C_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "stm32_capi_i2c.h"

/**
 * @brief STM32 platform specific I2C private handle
 */
struct stm32_i2c_priv_handle {
	/** I2C HAL Handle */
	I2C_HandleTypeDef hi2c;
	/** CAPI callback */
	capi_i2c_callback callback;
	/** CAPI callback argument */
	void *callback_arg;
	/** Async transfer in progress flag */
	bool async_in_progress;
	/** Current transfer for async operations */
	struct capi_i2c_transfer *current_transfer;
	/** true when controller is in initiator (master) role */
	bool is_initiator;
	/** Cached target address (7-bit, unshifted) when in target mode */
	uint16_t target_addr;
};

#define CAPI_I2C_CONTROLLER_HANDLE_STM32_INIT() \
	(&(struct capi_i2c_controller_handle){ \
		.ops = NULL, \
		.init_allocated = false, \
		.priv = &(struct stm32_i2c_priv_handle){0}})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_I2C_PRIV_H_ */
