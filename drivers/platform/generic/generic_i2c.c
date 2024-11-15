/***************************************************************************//**
 *   @file   generic/generic_i2c.c
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_i2c.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_i2c_init(struct no_os_i2c_desc **desc,
			 const struct no_os_i2c_init_param *param)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(param);

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_i2c_remove(struct no_os_i2c_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

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
int32_t generic_i2c_write(struct no_os_i2c_desc *desc,
			  uint8_t *data,
			  uint8_t bytes_number,
			  uint8_t stop_bit)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);
	NO_OS_UNUSED_PARAM(stop_bit);

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
int32_t generic_i2c_read(struct no_os_i2c_desc *desc,
			 uint8_t *data,
			 uint8_t bytes_number,
			 uint8_t stop_bit)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);
	NO_OS_UNUSED_PARAM(stop_bit);

	return 0;
}

/**
 * @brief Generic platform I2C ops
 */
const struct no_os_i2c_platform_ops generic_i2c_ops = {
	.i2c_ops_init = &generic_i2c_init,
	.i2c_ops_write = &generic_i2c_write,
	.i2c_ops_read = &generic_i2c_read,
	.i2c_ops_remove = &generic_i2c_remove
};
