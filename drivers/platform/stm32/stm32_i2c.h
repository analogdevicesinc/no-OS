/***************************************************************************//**
 *   @file   stm32/stm32_i2c.h
 *   @brief  Header file for the stm32 i2c driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_I2C_H_
#define STM32_I2C_H_

#include <stdint.h>
#include "no_os_i2c.h"
#include "stm32_hal.h"

/**
 * @struct stm32_i2c_desc
 * @brief stm32 platform specific I2C descriptor
 */
struct stm32_i2c_desc {
	/** I2C instance */
	I2C_HandleTypeDef hi2c;
};

/**
 * @struct stm32_i2c_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific I2C parameters.
 */
struct stm32_i2c_init_param {
	/** I2C Timing */
	uint32_t i2c_timing;
};

/**
 * @brief stm32 specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops stm32_i2c_ops;

#endif // STM32_I2C_H_
