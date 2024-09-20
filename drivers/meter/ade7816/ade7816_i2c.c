/***************************************************************************//**
 *   @file   ade7816_i2c.c
 *   @brief  Source file of ADE7816 I2C Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
 ******************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_i2c.h"
#include "ade7816.h"

/**
 * @brief ADE7816 I2C Read Register function
 * @param desc - ADE7816 device descriptor.
 * @param reg - Register Address value.
 * @param val - Register Value to be read.
 * @return 0 in case of succes, negative error code otherwise.
 */
int ade7816_i2c_reg_read(struct ade7816_desc *desc, uint16_t reg, uint32_t *val)
{
	uint8_t bytes_number;
	uint32_t mask = 0;
	uint8_t data[4];
	int ret;

	data[0] = no_os_field_get(ADE7816_MSB_REG_MASK, reg);
	data[1] = no_os_field_get(ADE7816_LSB_REG_MASK, reg);

	switch (reg) {
	case ADE7816_VGAIN_REG ... ADE7816_FVAROS_REG:
	case ADE7816_VARNOLOAD_REG:
		bytes_number = 4;
		mask = NO_OS_GENMASK(23, 0);
		break;
	case ADE7816_CHSTATUS_REG ... ADE7816_CONFIG_REG:
		bytes_number = 2;
		break;
	case ADE7816_MMODE_REG ... ADE7816_CONFIG2_REG:
		bytes_number = 1;
		break;
	default:
		bytes_number = 4;
		break;
	}

	ret = no_os_i2c_write(desc->i2c_desc, data, 2, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc->i2c_desc, data, bytes_number, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be32(data);

	if (mask)
		*val = no_os_field_get(*val, mask);

	return 0;
}

/**
 * @brief ADE7816 I2C Write Register function
 * @param desc - ADE7816 device descriptor
 * @param reg - Register Address value.
 * @param val - Register Value to be written.
 * @return 0 in case of succes, negative error code otherwise.
 */
int ade7816_i2c_reg_write(struct ade7816_desc *desc, uint16_t reg, uint32_t val)
{
	uint8_t bytes_number;
	uint32_t mask = 0;
	uint8_t data[6];

	data[0] = no_os_field_get(ADE7816_MSB_REG_MASK, reg);
	data[1] = no_os_field_get(ADE7816_LSB_REG_MASK, reg);

	switch (reg) {
	case ADE7816_VGAIN_REG ... ADE7816_FVAROS_REG:
	case ADE7816_VARNOLOAD_REG:
		bytes_number = 4;
		mask = NO_OS_GENMASK(23, 0);
		break;
	case ADE7816_CHSTATUS_REG ... ADE7816_CONFIG_REG:
		bytes_number = 2;
		break;
	case ADE7816_MMODE_REG ... ADE7816_CONFIG2_REG:
		bytes_number = 1;
		break;
	default:
		bytes_number = 4;
		break;
	}

	if (mask) {
		val = no_os_sign_extend32(val, 23);
		val = no_os_field_get(ADE7816_ZSPE_MASK, val);
	}

	data[2] = no_os_field_get(ADE7816_FOURTH_BYTE_MASK, val);
	data[3] = no_os_field_get(ADE7816_THIRD_BYTE_MASK, val);
	data[4] = no_os_field_get(ADE7816_SECOND_BYTE_MASK, val);
	data[5] = no_os_field_get(ADE7816_FIRST_BYTE_MASK, val);

	return no_os_i2c_write(desc->i2c_desc, data, 2 + bytes_number, 1);
}
