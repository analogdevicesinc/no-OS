/***************************************************************************//**
 *   @file   adxl372_i2c.c
 *   @brief  Implementation of adxl372 I2C Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "no_os_error.h"
#include "adxl372.h"

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_i2c_reg_read(struct adxl372_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data)
{
	uint8_t data;
	int32_t ret;

	data = reg_addr;

	ret = no_os_i2c_write(dev->i2c_desc, &data, 1, 0);
	if (ret < 0)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, &data, 1, 0);
	if (ret < 0)
		return ret;

	*reg_data = data;

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_i2c_reg_write(struct adxl372_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data)
{
	uint8_t buf[2];

	buf[0] = reg_addr;
	buf[1] = reg_data & 0xFF;

	return no_os_i2c_write(dev->i2c_desc, buf, NO_OS_ARRAY_SIZE(buf), 0);
}

/**
 * Multibyte read from device. A register read begins with the address
 * and autoincrements for each aditional byte in the transfer.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @param count - Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_i2c_reg_read_multiple(struct adxl372_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count)
{
	uint8_t buf[512];
	int32_t ret;

	if (count > 512)
		return -1;

	buf[0] = reg_addr;
	memset(&buf[1], 0x00, count - 1);

	ret = no_os_i2c_write(dev->i2c_desc, buf, 1, 0);
	if (ret < 0)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, count, 0);
	if (ret < 0)
		return ret;

	memcpy(reg_data, buf, count);

	return ret;
}
