/*******************************************************************************
 *   @file   ftd2xx_i2c.h
 *   @brief  Implementation of I2C driver for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef FTD2XX_I2C_H_
#define FTD2XX_I2C_H_

#include <stdbool.h>
#include "no_os_i2c.h"
#include "ftd2xx.h"

/**
 * @struct ftd2xx_i2c_init
 * @brief Structure holding the initialization parameters for ftd2xx platform
 * specific I2C parameters.
 */
struct ftd2xx_i2c_init {
	/** Selected device specific PIN configuration.
	 *  BIT7-BIT0: Initial direction of the pins
	 *  BIT15-BIT8: Initial values of the pins
	 *  BIT23-BIT16: Final direction of the pins
	 *  BIT31-BIT24: Final values of the pins
	 */
	uint32_t channel_config_pin;
	/** Enable specific PIN configuration if set to true. */
	bool channel_config;
	/** Enable extra GPIO pin configuration at I2C init. */
	bool extra_pins;
	/** Extra pin values, bit 0 for low, bit 1 for high. */
	uint8_t extra_pins_val;
	/** Extra pin direction, bit 0 for in, bit 1 for out. */
	uint8_t extra_pins_dir;
};

/**
 * @struct ftd2xx_i2c_desc
 * @brief ftd2xx platform specific I2C descriptor
 */
struct ftd2xx_i2c_desc {
	/** Specific device handle */
	FT_HANDLE ftHandle;
};

/**
 * @brief ftd2xx platform specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops ftd2xx_i2c_ops;

#endif
