/***************************************************************************//**
 *   @file   chibios/chibios_i2c.h
 *   @brief  Header file for the chibios i2c driver.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef CHIBIOS_I2C_H_
#define CHIBIOS_I2C_H_

#include <stdint.h>
#include "no_os_i2c.h"
#include "hal.h"

#if (HAL_USE_I2C==TRUE)

/**
 * @struct chibios_i2c_desc
 * @brief chibios platform specific I2C descriptor iwth config and slave address parameters
 */
struct chibios_i2c_desc {
	/** I2C instance */
	I2CDriver *hi2c;
	/** I2C init specific parameter*/
	I2CConfig *i2ccfg;
	/**I2C address*/
	uint16_t i2caddr;
	/** Temporary buffer staged by a no-stop-bit write, consumed by the
	 *  following no-stop-bit read */
	uint8_t *buffer;
	/** Number of bytes staged in buffer */
	uint16_t buffer_size;
};

/**
 * @struct chibios_i2c_init_param
 * @brief ChibiOS i2c param struct
 *
 */
struct chibios_i2c_init_param {
	/** I2C instance*/
	I2CDriver *hi2c;
	/** I2C Config*/
	I2CConfig *i2ccfg;
};

/**
 * @brief chibios specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops chibios_i2c_ops;

#endif // HAL_USE_I2C==TRUE

#endif // CHIBIOS_I2C_H_
