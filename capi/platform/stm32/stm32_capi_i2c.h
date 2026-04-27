/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_CAPI_I2C_H_
#define STM32_CAPI_I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi_i2c.h"

/**
 * @struct stm32_i2c_extra_config
 * @brief STM32 platform specific I2C configuration for CAPI interface
 */
struct stm32_i2c_extra_config {
	/** I2C Handle - can be NULL for auto-init based on identifier */
	I2C_HandleTypeDef *hi2c;
	/** I2C Timing (for STM32 families that use timing register) */
	uint32_t i2c_timing;
};

/**
 * @brief STM32 CAPI I2C operations structure
 */
extern const struct capi_i2c_ops stm32_capi_i2c_ops;

#endif /* STM32_CAPI_I2C_H_ */
