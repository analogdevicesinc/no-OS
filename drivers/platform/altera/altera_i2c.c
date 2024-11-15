/***************************************************************************//**
 *   @file   altera/altera_i2c.c
 *   @brief  Implementation of Altera I2C Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_i2c.h"
#include "altera_i2c.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Altera platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops altera_i2c_ops = {
	.i2c_ops_init = &altera_i2c_init,
	.i2c_ops_write = &altera_i2c_write,
	.i2c_ops_read = &altera_i2c_read,
	.i2c_ops_remove = &altera_i2c_remove
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_i2c_init(struct no_os_i2c_desc **desc,
			const struct no_os_i2c_init_param *param)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (((struct altera_i2c_init_param *)param->extra)->type) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_i2c_remove(struct no_os_i2c_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_i2c_write(struct no_os_i2c_desc *desc,
			 uint8_t *data,
			 uint8_t bytes_number,
			 uint8_t stop_bit)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (data) {
		// Unused variable - fix compiler warning
	}

	if (bytes_number) {
		// Unused variable - fix compiler warning
	}

	if (stop_bit) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_i2c_read(struct no_os_i2c_desc *desc,
			uint8_t *data,
			uint8_t bytes_number,
			uint8_t stop_bit)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (data) {
		// Unused variable - fix compiler warning
	}

	if (bytes_number) {
		// Unused variable - fix compiler warning
	}

	if (stop_bit) {
		// Unused variable - fix compiler warning
	}

	return 0;
}
