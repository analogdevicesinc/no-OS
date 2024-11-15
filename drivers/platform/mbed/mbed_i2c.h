/***************************************************************************//**
 *   @file   mbed_i2c.h
 *   @brief  Header containing extra types required for I2C interface
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MBED_I2C_H_
#define MBED_I2C_H_

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/

/*
 * Note: The structure members are not strongly typed, as this file is included
 *       in application specific '.c' files. The mbed code structure does not
 *       allow inclusion of mbed driver files (e.g. mbed.h) into '.c' files.
 *       All the members are hence typecasted to mbed specific type during
 *       i2c init and read/write operations.
 **/

/**
* @struct mbed_i2c_init_param
* @brief Structure holding the I2C init parameters for mbed platform.
*/
struct mbed_i2c_init_param {
	uint16_t i2c_sda_pin;  	// I2C SDA pin (PinName)
	uint16_t i2c_scl_pin;  	// I2C SCL pin (PinName)
};

/**
* @struct mbed_i2c_desc
* @brief I2C specific descriptor for the mbed platform.
*/
struct mbed_i2c_desc {
	void *i2c_port;  		// I2C port instance (mbed::I2C)
};

/**
 * @brief Mbed specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops mbed_i2c_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /* MBED_I2C_H_ */
