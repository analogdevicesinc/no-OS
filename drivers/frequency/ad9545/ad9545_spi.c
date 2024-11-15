/***************************************************************************//**
 *   @file   ad9545_spi.c
 *   @brief  Implementation of ad9545 SPI Driver.
 *   @author Jonathan Santos (Jonathan.Santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_spi.h"
#include "ad9545.h"

#define AD9535_SPI_READ NO_OS_BIT(7)

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9545_spi_reg_read(struct ad9545_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = no_os_field_get(BYTE_ADDR_H, reg_addr) | AD9535_SPI_READ;
	buf[1] = no_os_field_get(BYTE_ADDR_L, reg_addr);
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	*reg_data = buf[2];

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9545_spi_reg_write(struct ad9545_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data)
{
	uint8_t buf[3];
	buf[0] = no_os_field_get(BYTE_ADDR_H, reg_addr);
	buf[1] = no_os_field_get(BYTE_ADDR_L, reg_addr);
	buf[2] = reg_data & 0xFF;

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
int32_t ad9545_spi_reg_read_multiple(struct ad9545_dev *dev,
				     uint16_t reg_addr,
				     uint8_t *reg_data,
				     uint16_t count)
{
	uint8_t buf[513];
	int32_t ret;

	if (count > 512)
		return -1;

	buf[0] = no_os_field_get(BYTE_ADDR_H, reg_addr) | AD9535_SPI_READ;
	buf[1] = no_os_field_get(BYTE_ADDR_L, reg_addr);
	memset(&buf[2], 0x00, count);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, count + 2);
	if (ret < 0)
		return ret;

	memcpy(reg_data, &buf[2], count);

	return ret;
}

/**
 * Write Multiple bytes to the device. A register write begins with the address
 * and autoincrements for each aditional byte in the transfer.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @param count - Number of bytes to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9545_spi_reg_write_multiple(struct ad9545_dev *dev,
				      uint16_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count)
{
	uint8_t buf[513];
	int32_t ret;

	if (count > 512)
		return -1;

	buf[0] = no_os_field_get(BYTE_ADDR_H, reg_addr);
	buf[1] = no_os_field_get(BYTE_ADDR_L, reg_addr);
	memcpy(&buf[2], reg_data, count);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, count + 2);

	return ret;
}
