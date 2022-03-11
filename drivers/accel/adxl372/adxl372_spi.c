/***************************************************************************//**
 *   @file   adxl372_spi.c
 *   @brief  Implementation of adxl372 SPI Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
int32_t adxl372_spi_reg_read(struct adxl372_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = ADXL372_REG_READ(reg_addr);
	buf[1] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
	if (ret < 0)
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
int32_t adxl372_spi_reg_write(struct adxl372_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data)
{
	uint8_t buf[2];

	buf[0] = ADXL372_REG_WRITE(reg_addr);
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
int32_t adxl372_spi_reg_read_multiple(struct adxl372_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count)
{
	uint8_t buf[513];
	int32_t ret;

	if (count > 512)
		return -1;

	buf[0] = ADXL372_REG_READ(reg_addr);
	memset(&buf[1], 0x00, count);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, count + 1);
	if (ret < 0)
		return ret;

	memcpy(reg_data, &buf[1], count);

	return ret;
}
