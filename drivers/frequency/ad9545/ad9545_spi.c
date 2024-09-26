/***************************************************************************//**
 *   @file   ad9545_spi.c
 *   @brief  Implementation of ad9545 SPI Driver.
 *   @author Jonathan Santos (Jonathan.Santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
