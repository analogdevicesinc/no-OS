/*******************************************************************************
 *   @file   adxl37x_spi.c
 *   @brief  Implementation of adxl37x SPI Driver.
 *   @author Brandon Hurst (brandon.hurst@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "adxl37x.h"
#include "no_os_error.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Largest multibyte SPI payload, in bytes; the buffer adds one address byte. */
#define ADXL37X_SPI_MAX_XFER_BYTES	1024

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl37x_spi_reg_read(struct adxl37x_dev *dev, uint8_t reg_addr,
			     uint8_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	if (!dev)
		return -EINVAL;

	buf[0] = ADXL37X_REG_READ(reg_addr);
	buf[1] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
	if (ret)
		return ret;

	*reg_data = buf[1];

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl37x_spi_reg_write(struct adxl37x_dev *dev, uint8_t reg_addr,
			      uint8_t reg_data)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = ADXL37X_REG_WRITE(reg_addr);
	buf[1] = reg_data & 0xFF;

	return no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
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
int32_t adxl37x_spi_reg_read_multiple(struct adxl37x_dev *dev, uint8_t reg_addr,
				      uint8_t *reg_data, uint16_t count)
{
	uint8_t buf[ADXL37X_SPI_MAX_XFER_BYTES + 1] = {0};
	int32_t ret;

	if (!dev)
		return -EINVAL;

	if (count > ADXL37X_SPI_MAX_XFER_BYTES)
		return -EINVAL;

	buf[0] = ADXL37X_REG_READ(reg_addr);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, count + 1);
	if (ret)
		return ret;

	memcpy(reg_data, &buf[1], count);

	return ret;
}
