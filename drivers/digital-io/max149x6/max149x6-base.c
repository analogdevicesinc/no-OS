/***************************************************************************//**
 *   @file   max149x6-base.c
 *   @brief  Source file of MAX149X6 Base Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "max149x6-base.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Compute the CRC5 value for an array of bytes when writing to MAX149X6
 * @param data - array of data to encode
 * @return the resulted CRC5
 */
static uint8_t max149x6_crc(uint8_t *data, bool encode)
{
	uint8_t crc5_start = 0x1f;
	uint8_t crc5_poly = 0x15;
	uint8_t crc5_result = crc5_start;
	uint8_t extra_byte = 0x00;
	uint8_t data_bit;
	uint8_t result_bit;
	int i;

	/*
	 * This is a custom implementation of a CRC5 algorithm, detailed here:
	 * https://www.analog.com/en/app-notes/how-to-program-the-max14906-quadchannel-industrial-digital-output-digital-input.html
	 */

	for (i = (encode) ? 0 : 2; i < 8; i++) {
		data_bit = (data[0] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (i = 0; i < 8; i++) {
		data_bit = (data[1] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (i = 0; i < 3; i++) {
		data_bit = (extra_byte >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	return crc5_result;
}

/**
 * @brief Write the value of a device register
 * @param desc - device descriptor for the MAX149X6
 * @param addr - address of the register
 * @param val - value of the register
 * @return 0 in case of success, negative error code otherwise
 */
int max149x6_reg_write(struct max149x6_desc *desc, uint32_t addr, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.bytes_number = MAX149X6_FRAME_SIZE,
		.cs_change = 1,
	};

	desc->buff[0] = no_os_field_prep(MAX149X6_CHIP_ADDR_MASK, desc->chip_address) |
			no_os_field_prep(MAX149X6_ADDR_MASK, addr) |
			no_os_field_prep(MAX149X6_RW_MASK, 1);
	desc->buff[1] = val;

	if (desc->crc_en) {
		xfer.bytes_number++;
		desc->buff[2] = max149x6_crc(desc->buff, true);
	}

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

/**
 * @brief Read the value of a device register
 * @param desc - device descriptor for the MAX149X6
 * @param addr - address of the register
 * @param val - value of the register
 * @return 0 in case of success, negative error code otherwise
 */
int max149x6_reg_read(struct max149x6_desc *desc, uint32_t addr, uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX149X6_FRAME_SIZE,
		.cs_change = 1,
	};
	uint8_t crc;
	int ret;

	if (desc->crc_en)
		xfer.bytes_number++;

	memset(desc->buff, 0, xfer.bytes_number);
	desc->buff[0] = no_os_field_prep(MAX149X6_CHIP_ADDR_MASK, desc->chip_address) |
			no_os_field_prep(MAX149X6_ADDR_MASK, addr) |
			no_os_field_prep(MAX149X6_RW_MASK, 0);

	if (desc->crc_en)
		desc->buff[2] = max149x6_crc(&desc->buff[0], true);

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (desc->crc_en) {
		crc = max149x6_crc(&desc->buff[0], false);
		if (crc != desc->buff[2])
			return -EINVAL;
	}

	*val = desc->buff[1];

	return 0;
}

/**
 * @brief Update the value of a device register (read/write sequence).
 * @param desc - device descriptor for the MAX149X6
 * @param addr - address of the register
 * @param mask - bit mask of the field to be updated
 * @param val - value of the masked field. Should be bit shifted by using
 * 		 no_os_field_prep(mask, val)
 * @return 0 in case of success, negative error code otherwise
 */
int max149x6_reg_update(struct max149x6_desc *desc, uint32_t addr,
			uint32_t mask, uint32_t val)
{
	int ret;
	uint32_t reg_val = 0;

	ret = max149x6_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return max149x6_reg_write(desc, addr, reg_val);
}
