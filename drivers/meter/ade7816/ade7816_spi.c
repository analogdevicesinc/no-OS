/***************************************************************************//**
 *   @file   ade7816_spi.c
 *   @brief  Source file of ADE7816 SPI Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "ade7816.h"

/**
 * @brief ADE7816 SPI Read Register function
 * @param desc - ADE7816 device descriptor.
 * @param reg - Register Address value.
 * @param val - Register Value to be read.
 * @return 0 in case of succes, negative error code otherwise.
 */
int ade7816_spi_reg_read(struct ade7816_desc *desc, uint16_t reg, uint32_t *val)
{
	uint8_t bytes_number;
	uint32_t mask = 0;
	uint8_t buff[7];
	int ret;

	buff[0] = 0x01;
	buff[1] = no_os_field_get(ADE7816_MSB_REG_MASK, reg);
	buff[2] = no_os_field_get(ADE7816_LSB_REG_MASK, reg);
	buff[3] = buff[4] = buff[5] = buff[6] = 0x00;

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

	ret = no_os_spi_write_and_read(desc->spi_desc, buff, 3 + bytes_number);
	if (ret)
		return ret;

	switch (bytes_number) {
	case 1:
		*val = buff[3];
		break;
	case 2:
		*val = no_os_get_unaligned_be16(&buff[3]);
		break;
	case 4:
		*val = no_os_get_unaligned_be32(&buff[3]);
		break;
	default:
		return -EINVAL;
	}

	if (mask)
		*val = no_os_field_get(*val, mask);


	return 0;
}

/**
 * @brief ADE7816 SPI Write Register function
 * @param desc - ADE7816 device descriptor
 * @param reg - Register Address value.
 * @param val - Register Value to be written.
 * @return 0 in case of succes, negative error code otherwise.
 */
int ade7816_spi_reg_write(struct ade7816_desc *desc, uint16_t reg, uint32_t val)
{
	uint8_t bytes_number;
	uint32_t mask = 0;
	uint8_t buff[7];

	buff[0] = 0x00;
	buff[1] = no_os_field_get(ADE7816_MSB_REG_MASK, reg);
	buff[2] = no_os_field_get(ADE7816_LSB_REG_MASK, reg);

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

	buff[3] = no_os_field_get(ADE7816_FOURTH_BYTE_MASK, val);
	buff[4] = no_os_field_get(ADE7816_THIRD_BYTE_MASK, val);
	buff[5] = no_os_field_get(ADE7816_SECOND_BYTE_MASK, val);
	buff[6] = no_os_field_get(ADE7816_FIRST_BYTE_MASK, val);

	return no_os_spi_write_and_read(desc->spi_desc, buff, 3 + bytes_number);
}
