/***************************************************************************//**
 *   @file   ades1754.c
 *   @brief  Source file for the ADES1754 Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "ades1754.h"
#include "no_os_alloc.h"
#include "no_os_crc8.h"
#include "no_os_delay.h"
#include "no_os_error.h"

NO_OS_DECLARE_CRC8_TABLE(crc_table);

/**
 * @brief Manchester encoding/decoding function for ADES1754
 * @param nibble - Encoding - Only 4 LSb are considered (a nibble).
 * 		   Decoding - Data byte to be decoded.
 * @param encode - true - Encode
 * 		   false - Decode
 * @return A byte of data in case of encoding (the encoded nibble that turn into
 * 	   a byte amid encoding) or a nibble of data (4 MSb have a value of 0).
 */
static uint8_t ades1754_manchester(uint8_t nibble, bool encode)
{
	bool bit0, bit1, bit2, bit3;
	if (encode) {
		bit0 = no_os_field_get(NO_OS_BIT(0), nibble);
		bit1 = no_os_field_get(NO_OS_BIT(1), nibble);
		bit2 = no_os_field_get(NO_OS_BIT(2), nibble);
		bit3 = no_os_field_get(NO_OS_BIT(3), nibble);

		nibble = no_os_field_prep(NO_OS_BIT(0), bit0) |
			 no_os_field_prep(NO_OS_BIT(2), bit1) |
			 no_os_field_prep(NO_OS_BIT(4), bit2) |
			 no_os_field_prep(NO_OS_BIT(6), bit3);
		nibble |= no_os_field_prep(NO_OS_BIT(1), !bit0) |
			  no_os_field_prep(NO_OS_BIT(3), !bit1) |
			  no_os_field_prep(NO_OS_BIT(5), !bit2) |
			  no_os_field_prep(NO_OS_BIT(7), !bit3);

		return nibble;
	} else {
		nibble = no_os_field_prep(NO_OS_BIT(1), no_os_field_get(NO_OS_BIT(2), nibble)) |
			 no_os_field_prep(NO_OS_BIT(2), no_os_field_get(NO_OS_BIT(4), nibble)) |
			 no_os_field_prep(NO_OS_BIT(3), no_os_field_get(NO_OS_BIT(6), nibble)) |
			 no_os_field_get(NO_OS_BIT(0), nibble);

		return nibble;
	}
}

/**
 * @brief ADES1754 HELLO ALL specific command message to wake-up devices
 * 	  alongside daisy-chain.
 * @param desc - ADES1754 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_hello_all(struct ades1754_desc *desc)
{
	uint8_t tx_data[8];
	uint8_t rx_data[16];
	int ret;

	if (desc->uart_bridge) {
		tx_data[0] = ADES1754_HELLO_ALL_BYTE;
		tx_data[1] = 0x00;
		tx_data[2] = desc->no_dev;

		ret = no_os_uart_write(desc->uart_desc, tx_data, 3);
		if (ret < 0)
			return ret;

		return no_os_uart_read(desc->uart_desc, rx_data, 3);
	} else {
		tx_data[0] = ADES1754_PREAMBLE_BYTE;
		tx_data[1] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 ADES1754_HELLO_ALL_BYTE), true);
		tx_data[2] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 ADES1754_HELLO_ALL_BYTE), true);
		tx_data[3] = ades1754_manchester(0x00, true);
		tx_data[4] = ades1754_manchester(0x00, true);
		tx_data[5] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 desc->no_dev), true);
		tx_data[6] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 desc->no_dev), true);
		tx_data[7] = ADES1754_STOP_BYTE;

		ret = no_os_uart_write(desc->uart_desc, tx_data, 8);
		if (ret < 0)
			return ret;

		return no_os_uart_read(desc->uart_desc, rx_data, 8);
	}
}

/**
 * @brief ADES1754 write device command message.
 * @param desc - ADES1754 device descriptor.
 * @param reg - Register Address.
 * @param data - Data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_write_dev(struct ades1754_desc *desc, uint8_t reg, uint16_t data)
{
	uint8_t tx_data[12];
	uint8_t crc[4];
	uint8_t pec;
	int ret;

	if (desc->uart_bridge) {
		tx_data[0] = no_os_field_prep(ADES1754_RW_ADDR_MASK,
					      desc->dev_addr) | ADES1754_WR_MASK;
		tx_data[1] = reg;
		tx_data[2] = no_os_field_get(ADES1754_LSB_MASK, data);
		tx_data[3] = no_os_field_get(ADES1754_MSB_MASK, data);

		crc[0] = no_os_bit_swap_constant_8(tx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(tx_data[1]);
		crc[2] = no_os_bit_swap_constant_8(tx_data[2]);
		crc[3] = no_os_bit_swap_constant_8(tx_data[3]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);

		tx_data[4] = no_os_bit_swap_constant_8(pec);

		return no_os_uart_write(desc->uart_desc, tx_data, 5);
	} else {
		tx_data[0] = ADES1754_PREAMBLE_BYTE;
		tx_data[1] = no_os_field_prep(ADES1754_RW_ADDR_MASK,
					      desc->dev_addr) | ADES1754_WR_MASK;
		tx_data[2] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 tx_data[1]), true);
		tx_data[1] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 tx_data[1]), true);
		tx_data[3] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 reg), true);
		tx_data[4] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 reg), true);
		tx_data[5] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_LSB_MASK, data)), true);
		tx_data[6] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_LSB_MASK, data)), true);
		tx_data[7] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_MSB_MASK, data)), true);
		tx_data[8] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_MSB_MASK, data)), true);

		crc[0] = no_os_field_prep(ADES1754_RW_ADDR_MASK,
					  desc->dev_addr) | ADES1754_WR_MASK;
		crc[1] = reg;
		crc[2] = no_os_field_get(ADES1754_LSB_MASK, data);
		crc[3] = no_os_field_get(ADES1754_MSB_MASK, data);

		crc[0] = no_os_bit_swap_constant_8(crc[0]);
		crc[1] = no_os_bit_swap_constant_8(crc[1]);
		crc[2] = no_os_bit_swap_constant_8(crc[2]);
		crc[3] = no_os_bit_swap_constant_8(crc[3]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);

		tx_data[9] = no_os_bit_swap_constant_8(pec);
		tx_data[10] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						  tx_data[9]), true);
		tx_data[9] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 tx_data[9]), true);

		tx_data[11] = ADES1754_STOP_BYTE;

		ret = no_os_uart_write(desc->uart_desc, tx_data, 12);
		if (ret < 0)
			return ret;

		return 0;
	}
}

/**
 * @brief ADES1754 write all command message.
 * @param desc - ADES1754 device descriptor.
 * @param reg - Register Address.
 * @param data - Data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_write_all(struct ades1754_desc *desc, uint8_t reg, uint16_t data)
{
	uint8_t tx_data[12];
	uint8_t crc[4];
	uint8_t pec;

	if (desc->uart_bridge) {
		tx_data[0] = ADES1754_WR_ALL;
		tx_data[1] = reg;
		tx_data[2] = no_os_field_get(ADES1754_LSB_MASK, data);
		tx_data[3] = no_os_field_get(ADES1754_MSB_MASK, data);

		crc[0] = no_os_bit_swap_constant_8(tx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(tx_data[1]);
		crc[2] = no_os_bit_swap_constant_8(tx_data[2]);
		crc[3] = no_os_bit_swap_constant_8(tx_data[3]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);

		tx_data[4] = no_os_bit_swap_constant_8(pec);

		return no_os_uart_write(desc->uart_desc, tx_data, 5);
	} else {
		tx_data[0] = ADES1754_PREAMBLE_BYTE;
		tx_data[1] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 ADES1754_WR_ALL), true);
		tx_data[2] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 ADES1754_WR_ALL), true);
		tx_data[3] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 reg), true);
		tx_data[4] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 reg), true);
		tx_data[5] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_LSB_MASK, data)), true);
		tx_data[6] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_LSB_MASK, data)), true);
		tx_data[7] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_MSB_MASK, data)), true);
		tx_data[8] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 no_os_field_get(ADES1754_MSB_MASK, data)), true);

		crc[0] = ADES1754_WR_ALL;
		crc[1] = reg;
		crc[2] = no_os_field_get(ADES1754_LSB_MASK, data);
		crc[3] = no_os_field_get(ADES1754_MSB_MASK, data);

		crc[0] = no_os_bit_swap_constant_8(crc[0]);
		crc[1] = no_os_bit_swap_constant_8(crc[1]);
		crc[2] = no_os_bit_swap_constant_8(crc[2]);
		crc[3] = no_os_bit_swap_constant_8(crc[3]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);

		tx_data[9] = no_os_bit_swap_constant_8(pec);
		tx_data[10] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						  tx_data[9]), true);
		tx_data[9] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 tx_data[9]), true);

		tx_data[11] = ADES1754_STOP_BYTE;

		return no_os_uart_write(desc->uart_desc, tx_data, 12);
	}
}

/**
 * @brief ADES1754 read device command message.
 * @param desc - ADES1754 device descriptor.
 * @param reg - Register Address.
 * @param data - Data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_read_dev(struct ades1754_desc *desc, uint8_t reg, uint16_t *data)
{
	int ret, read_b = 0, retries = 0;
	uint8_t rx_data[14] = { 0 };
	uint8_t tx_data[14];
	uint8_t crc[6];
	uint8_t pec;

	if (desc->uart_bridge) {
		tx_data[0] = no_os_field_prep(ADES1754_RW_ADDR_MASK,
					      desc->dev_addr) | ADES1754_RD_MASK;
		tx_data[1] = reg;
		tx_data[2] = ADES1754_DEFAULT_DC_BYTE_SEED;

		crc[0] = no_os_bit_swap_constant_8(tx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(tx_data[1]);
		crc[2] = no_os_bit_swap_constant_8(tx_data[2]);

		pec = no_os_crc8(crc_table, crc, 3, 0x00);

		tx_data[3] = no_os_bit_swap_constant_8(pec);
		tx_data[4] = ADES1754_FILL_BYTE_C2;
		tx_data[5] = ADES1754_FILL_BYTE_D3;

		ret = no_os_uart_write(desc->uart_desc, tx_data, 6);
		if (ret < 0)
			return ret;

		ret = no_os_uart_read(desc->uart_desc, rx_data, 7);
		if (ret < 0)
			return ret;

		crc[0] = no_os_bit_swap_constant_8(rx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(rx_data[1]);
		crc[2] = no_os_bit_swap_constant_8(rx_data[2]);
		crc[3] = no_os_bit_swap_constant_8(rx_data[3]);
		crc[4] = no_os_bit_swap_constant_8(rx_data[4]);
		crc[5] = no_os_bit_swap_constant_8(rx_data[5]);

		pec = no_os_crc8(crc_table, crc, 6, 0x00);
		pec = no_os_bit_swap_constant_8(pec);
		if (pec != rx_data[6])
			return -EINVAL;

		*data = no_os_field_prep(ADES1754_MSB_MASK, rx_data[3]) | rx_data[2];
	} else {
		tx_data[0] = ADES1754_PREAMBLE_BYTE;
		tx_data[1] = no_os_field_prep(ADES1754_RW_ADDR_MASK,
					      desc->dev_addr) | ADES1754_RD_MASK;
		tx_data[2] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 tx_data[1]), true);
		tx_data[1] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 tx_data[1]), true);
		tx_data[3] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 reg), true);
		tx_data[4] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 reg), true);
		tx_data[5] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 ADES1754_DEFAULT_DC_BYTE_SEED), true);
		tx_data[6] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 ADES1754_DEFAULT_DC_BYTE_SEED), true);

		crc[0] = ADES1754_PREAMBLE_BYTE;
		crc[1] = no_os_field_prep(ADES1754_RW_ADDR_MASK,
					  desc->dev_addr) | ADES1754_RD_MASK;
		crc[2] = reg;
		crc[3] = ADES1754_DEFAULT_DC_BYTE_SEED;

		crc[0] = no_os_bit_swap_constant_8(crc[0]);
		crc[1] = no_os_bit_swap_constant_8(crc[1]);
		crc[2] = no_os_bit_swap_constant_8(crc[2]);
		crc[3] = no_os_bit_swap_constant_8(crc[3]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);
		pec = no_os_bit_swap_constant_8(pec);
		tx_data[7] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 pec), true);
		tx_data[8] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 pec), true);
		tx_data[9] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 ADES1754_FILL_BYTE_C2), true);
		tx_data[10] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						  ADES1754_FILL_BYTE_C2), true);
		tx_data[11] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						  ADES1754_FILL_BYTE_D3), true);
		tx_data[12] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						  ADES1754_FILL_BYTE_D3), true);
		tx_data[13] = ADES1754_STOP_BYTE;

		ret = no_os_uart_write(desc->uart_desc, tx_data, 14);
		if (ret < 0)
			return ret;

		while (read_b < 14) {
			ret = no_os_uart_read(desc->uart_desc, &rx_data[read_b],
					      14 - read_b);
			if (ret >= 0)
				read_b += ret;

			retries++;
			if (retries == 100)
				return -ETIMEDOUT;
		}

		crc[0] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[2], false)) | ades1754_manchester(rx_data[1],
							  false);
		crc[1] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[4], false)) | ades1754_manchester(rx_data[3],
							  false);
		crc[2] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[6], false)) | ades1754_manchester(rx_data[5],
							  false);
		crc[3] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[8], false)) | ades1754_manchester(rx_data[7],
							  false);
		crc[4] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[10], false)) | ades1754_manchester(rx_data[9],
							  false);
		crc[5] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[12], false)) | ades1754_manchester(rx_data[11],
							  false);

		crc[0] = no_os_bit_swap_constant_8(crc[0]);
		crc[1] = no_os_bit_swap_constant_8(crc[1]);
		crc[2] = no_os_bit_swap_constant_8(crc[2]);
		crc[3] = no_os_bit_swap_constant_8(crc[3]);
		crc[4] = no_os_bit_swap_constant_8(crc[4]);

		pec = no_os_crc8(crc_table, crc, 5, 0x00);
		pec = no_os_bit_swap_constant_8(pec);

		if (pec != crc[5] && rx_data[13] == ADES1754_STOP_BYTE)
			return -EINVAL;

		*data = no_os_field_prep(ADES1754_MSB_MASK, crc[4]) | crc[3];
	}

	return 0;
}

/**
 * @brief ADES1754 read all command message.
 * @param desc - ADES1754 device descriptor.
 * @param reg - Register Address.
 * @param data - Data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_read_all(struct ades1754_desc *desc, uint8_t reg, uint16_t *data)
{
	int ret, i, j, read_b = 0, retries = 0;
	uint8_t tx_data[10 + 4 * desc->no_dev];
	uint8_t rx_data[10 + 4 * desc->no_dev];
	uint8_t crc[9 + 4 * desc->no_dev];
	uint8_t pec;

	if (desc->uart_bridge) {
		tx_data[0] = ADES1754_RD_ALL;
		tx_data[1] = reg;
		tx_data[2] = ADES1754_DEFAULT_DC_BYTE_SEED;

		crc[0] = no_os_bit_swap_constant_8(tx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(tx_data[1]);
		crc[2] = no_os_bit_swap_constant_8(tx_data[2]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);
		tx_data[3] = no_os_bit_swap_constant_8(pec);

		for (i = 0; i < 2 * desc->no_dev; i++) {
			if (i % 2)
				tx_data[i + 4] = ADES1754_FILL_BYTE_C2;
			else
				tx_data[i + 4] = ADES1754_FILL_BYTE_D3;
		}

		ret = no_os_uart_write(desc->uart_desc, tx_data,
				       4 + 2 * desc->no_dev);
		if (ret < 0)
			return ret;

		ret = no_os_uart_read(desc->uart_desc, rx_data,
				      5 + 2 * desc->no_dev);
		if (ret < 0)
			return ret;

		crc[0] = no_os_bit_swap_constant_8(rx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(rx_data[1]);
		for (i = 0; i < 2 * desc->no_dev; i++)
			crc[i + 2] = no_os_bit_swap_constant_8(rx_data[i + 2]);

		crc[i + 2] = no_os_bit_swap_constant_8(rx_data[i + 2]);
		crc[i + 3] = no_os_bit_swap_constant_8(rx_data[i + 3]);
		pec = no_os_crc8(crc_table, crc, 4 + 2 * desc->no_dev, 0x00);
		pec = no_os_bit_swap_constant_8(pec);
		if (pec != rx_data[i + 4])
			return -EINVAL;

		for (j = 0; j < desc->no_dev; j++)
			data[j] = no_os_field_prep(ADES1754_MSB_MASK,
						   rx_data[3 + 2 * j]) | rx_data[2 + 2 * j];
	} else {
		tx_data[0] = ADES1754_PREAMBLE_BYTE;
		tx_data[1] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 ADES1754_RD_ALL), true);
		tx_data[2] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 ADES1754_RD_ALL), true);
		tx_data[3] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 reg), true);
		tx_data[4] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 reg), true);
		tx_data[5] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 ADES1754_DEFAULT_DC_BYTE_SEED), true);
		tx_data[6] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 ADES1754_DEFAULT_DC_BYTE_SEED), true);

		crc[0] = ADES1754_RD_ALL;
		crc[1] = reg;
		crc[2] = ADES1754_DEFAULT_DC_BYTE_SEED;

		crc[0] = no_os_bit_swap_constant_8(crc[0]);
		crc[1] = no_os_bit_swap_constant_8(crc[1]);
		crc[2] = no_os_bit_swap_constant_8(crc[2]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);
		pec = no_os_bit_swap_constant_8(pec);

		tx_data[7] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 pec), true);
		tx_data[8] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 pec), true);

		for (i = 0; i < 4 * desc->no_dev; i++) {
			switch (i % 4) {
			case 0:
				tx_data[i + 9] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
								     ADES1754_FILL_BYTE_C2), true);
				break;
			case 1:
				tx_data[i + 9] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
								     ADES1754_FILL_BYTE_C2), true);
				break;
			case 2:
				tx_data[i + 9] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
								     ADES1754_FILL_BYTE_D3), true);
				break;
			case 3:
				tx_data[i + 9] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
								     ADES1754_FILL_BYTE_D3), true);
				break;
			default:
				return -EINVAL;
			}
		}
		tx_data[i + 9] = ADES1754_STOP_BYTE;

		ret = no_os_uart_write(desc->uart_desc, tx_data, i + 10);
		if (ret < 0)
			return ret;

		while (read_b < 10 + 4 * desc->no_dev) {
			ret = no_os_uart_read(desc->uart_desc, &rx_data[read_b],
					      (10 + 4 * desc->no_dev) - read_b);
			if (ret >= 0)
				read_b += ret;
			else {
				retries++;
				if (retries > 100)
					return -ETIMEDOUT;
			}
		}

		crc[0] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[2], false)) | ades1754_manchester(rx_data[1],
							  false);
		crc[1] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[4], false)) | ades1754_manchester(rx_data[3],
							  false);

		for (i = 0; i < 2 * desc->no_dev; i++)
			crc[i + 2] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
						      ades1754_manchester(rx_data[2 * (i + 2)],
								      false)) | ades1754_manchester(rx_data[2 * (i + 2) - 1], false);

		crc[i + 2] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					      ades1754_manchester(rx_data[2 * (i + 2)],
							      false)) | ades1754_manchester(rx_data[2 * (i + 2) - 1], false);
		crc[i + 3] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					      ades1754_manchester(rx_data[2 * (i + 3)],
							      false)) | ades1754_manchester(rx_data[2 * (i + 3) - 1], false);

		for (j = 0; j < i + 3; i++)
			crc[j] = no_os_bit_swap_constant_8(crc[j]);

		pec = no_os_crc8(crc_table, crc, 3 + 2 * desc->no_dev, 0x00);
		pec = no_os_bit_swap_constant_8(pec);
		if (pec != crc[i + 3] && rx_data[11 + (4 * desc->no_dev)] == ADES1754_STOP_BYTE)
			return -EINVAL;

		j = 0;
		for (i = 3; i < 3 + 2 * desc->no_dev; i += 4) {
			data[j] = no_os_field_prep(ADES1754_MSB_MASK,
						   no_os_bit_swap_constant_8(crc[i + 1])) | no_os_bit_swap_constant_8(crc[i]);
			j++;
		}
	}

	return 0;
}

/**
 * @brief ADES1754 read block command message.
 * @param desc - ADES1754 device descriptor.
 * @param block - Block number.
 * @param reg - Register Address.
 * @param data - Data to be read.
 * @param double_size - true - Block is double sized (16 bits).
 * 			false - Block is single sized (8 bits).
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_read_block(struct ades1754_desc *desc, uint8_t block, uint8_t reg,
			uint16_t *data, bool double_size)
{
	int ret, read_b = 0, retries = 0;
	uint8_t tx_data[20];
	uint8_t rx_data[20];
	uint8_t crc[9];
	uint8_t pec;

	if (desc->uart_bridge) {
		tx_data[0] = no_os_field_prep(ADES1754_RW_ADDR_MASK, block) | ADES1754_BL_MASK;
		tx_data[1] = desc->dev_addr;
		tx_data[2] = reg;
		tx_data[3] = ADES1754_DEFAULT_DC_BYTE_SEED;

		crc[0] = no_os_bit_swap_constant_8(tx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(tx_data[1]);
		crc[2] = no_os_bit_swap_constant_8(tx_data[2]);
		crc[3] = no_os_bit_swap_constant_8(tx_data[3]);

		pec = no_os_crc8(crc_table, crc, 4, 0x00);
		tx_data[4] = no_os_bit_swap_constant_8(pec);
		tx_data[5] = ADES1754_FILL_BYTE_C2;
		tx_data[6] = ADES1754_FILL_BYTE_D3;

		if (double_size) {
			tx_data[7] = ADES1754_FILL_BYTE_C2;
			tx_data[8] = ADES1754_FILL_BYTE_D3;

			ret = no_os_uart_write(desc->uart_desc, tx_data, 9);
			if (ret < 0)
				return ret;

			ret = no_os_uart_read(desc->uart_desc, rx_data, 10);
			if (ret < 0)
				return ret;
		} else {
			ret = no_os_uart_write(desc->uart_desc, tx_data, 7);
			if (ret < 0)
				return ret;

			ret = no_os_uart_read(desc->uart_desc, rx_data, 8);
			if (ret < 0)
				return ret;
		}

		crc[0] = no_os_bit_swap_constant_8(rx_data[0]);
		crc[1] = no_os_bit_swap_constant_8(rx_data[1]);
		crc[2] = no_os_bit_swap_constant_8(rx_data[2]);
		crc[3] = no_os_bit_swap_constant_8(rx_data[3]);
		crc[4] = no_os_bit_swap_constant_8(rx_data[4]);
		crc[5] = no_os_bit_swap_constant_8(rx_data[5]);
		crc[6] = no_os_bit_swap_constant_8(rx_data[6]);

		if (double_size) {
			crc[6] = no_os_bit_swap_constant_8(rx_data[6]);
			crc[7] = no_os_bit_swap_constant_8(rx_data[7]);
			crc[8] = no_os_bit_swap_constant_8(rx_data[8]);

			pec = no_os_crc8(crc_table, crc, 9, 0x00);
			pec = no_os_bit_swap_constant_8(pec);
			if (pec != rx_data[9])
				return -EINVAL;

			data[0] = no_os_field_prep(ADES1754_MSB_MASK, rx_data[5]) | rx_data[4];
			data[1] = no_os_field_prep(ADES1754_MSB_MASK, rx_data[7]) | rx_data[5];
		} else {
			pec = no_os_crc8(crc_table, crc, 7, 0x00);
			pec = no_os_bit_swap_constant_8(pec);
			if (pec != rx_data[7])
				return -EINVAL;

			*data = no_os_field_prep(ADES1754_MSB_MASK, rx_data[5]) | rx_data[4];
		}
	} else {
		tx_data[0] = ADES1754_PREAMBLE_BYTE;
		tx_data[1] = no_os_field_prep(ADES1754_RW_ADDR_MASK, block) | ADES1754_BL_MASK;
		tx_data[2] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 tx_data[1]), true);
		tx_data[1] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 tx_data[1]), true);
		tx_data[3] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 desc->dev_addr), true);
		tx_data[4] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 desc->dev_addr), true);
		tx_data[5] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 reg), true);
		tx_data[6] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 reg), true);
		tx_data[7] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 ADES1754_DEFAULT_DC_BYTE_SEED), true);
		tx_data[8] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						 ADES1754_DEFAULT_DC_BYTE_SEED), true);

		crc[0] = ADES1754_PREAMBLE_BYTE;
		crc[1] = no_os_field_prep(ADES1754_RW_ADDR_MASK, block) | ADES1754_BL_MASK;
		crc[2] = desc->dev_addr;
		crc[3] = reg;
		crc[4] = ADES1754_DEFAULT_DC_BYTE_SEED;

		crc[0] = no_os_bit_swap_constant_8(crc[0]);
		crc[1] = no_os_bit_swap_constant_8(crc[1]);
		crc[2] = no_os_bit_swap_constant_8(crc[2]);
		crc[3] = no_os_bit_swap_constant_8(crc[3]);
		crc[4] = no_os_bit_swap_constant_8(crc[4]);

		pec = no_os_crc8(crc_table, crc, 5, 0x00);
		pec = no_os_bit_swap_constant_8(pec);
		tx_data[9] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						 pec), true);
		tx_data[10] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						  pec), true);
		tx_data[11] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						  ADES1754_FILL_BYTE_C2), true);
		tx_data[12] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						  ADES1754_FILL_BYTE_C2), true);
		tx_data[13] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
						  ADES1754_FILL_BYTE_D3), true);
		tx_data[14] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
						  ADES1754_FILL_BYTE_D3), true);
		tx_data[15] = ADES1754_STOP_BYTE;

		if (double_size) {
			tx_data[15] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
							  ADES1754_FILL_BYTE_C2), true);
			tx_data[16] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
							  ADES1754_FILL_BYTE_C2), true);
			tx_data[17] = ades1754_manchester(no_os_field_get(ADES1754_LOWER_NIBBLE_MASK,
							  ADES1754_FILL_BYTE_D3), true);
			tx_data[18] = ades1754_manchester(no_os_field_get(ADES1754_UPPER_NIBBLE_MASK,
							  ADES1754_FILL_BYTE_D3), true);

			tx_data[19] = ADES1754_STOP_BYTE;

			ret = no_os_uart_write(desc->uart_desc, tx_data, 20);
			if (ret < 0)
				return ret;

			while (read_b < 20) {
				ret = no_os_uart_read(desc->uart_desc, &rx_data[read_b], 20 - read_b);
				if (ret >= 0)
					read_b += ret;
				else {
					retries++;
					if (retries > 100)
						return -ETIMEDOUT;
				}
			}
		} else {
			ret = no_os_uart_write(desc->uart_desc, tx_data, 16);
			if (ret < 0)
				return ret;

			while (read_b < 16) {
				ret = no_os_uart_read(desc->uart_desc, &rx_data[read_b], 16 - read_b);
				if (ret >= 0)
					read_b += ret;
				else {
					retries++;
					if (retries > 100)
						return -ETIMEDOUT;
				}
			}
		}

		crc[0] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[2], false)) | ades1754_manchester(rx_data[1],
							  false);
		crc[1] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[4], false)) | ades1754_manchester(rx_data[3],
							  false);
		crc[2] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[6], false)) | ades1754_manchester(rx_data[5],
							  false);
		crc[3] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[8], false)) | ades1754_manchester(rx_data[7],
							  false);
		crc[4] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[10], false)) | ades1754_manchester(rx_data[9],
							  false);
		crc[5] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[12], false)) | ades1754_manchester(rx_data[11],
							  false);
		crc[6] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
					  ades1754_manchester(rx_data[14], false)) | ades1754_manchester(rx_data[13],
							  false);

		if (double_size) {
			crc[7] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
						  ades1754_manchester(rx_data[16], false)) | ades1754_manchester(rx_data[15],
								  false);
			crc[8] = no_os_field_prep(ADES1754_UPPER_NIBBLE_MASK,
						  ades1754_manchester(rx_data[18], false)) | ades1754_manchester(rx_data[17],
								  false);

			crc[0] = no_os_bit_swap_constant_8(crc[0]);
			crc[1] = no_os_bit_swap_constant_8(crc[1]);
			crc[2] = no_os_bit_swap_constant_8(crc[2]);
			crc[3] = no_os_bit_swap_constant_8(crc[3]);
			crc[4] = no_os_bit_swap_constant_8(crc[4]);
			crc[5] = no_os_bit_swap_constant_8(crc[5]);
			crc[6] = no_os_bit_swap_constant_8(crc[6]);
			crc[7] = no_os_bit_swap_constant_8(crc[7]);

			pec = no_os_crc8(crc_table, crc, 8, 0x00);
			pec = no_os_bit_swap_constant_8(pec);
			if (pec != crc[8] && rx_data[19] == ADES1754_STOP_BYTE)
				return -EINVAL;

			data[0] = no_os_field_prep(ADES1754_MSB_MASK, crc[5]) | crc[4];
			data[1] = no_os_field_prep(ADES1754_MSB_MASK, crc[7]) | crc[6];
		} else {
			pec = no_os_crc8(crc_table, crc, 6, 0x00);
			pec = no_os_bit_swap_constant_8(pec);
			if (pec != crc[6] && rx_data[15] == ADES1754_STOP_BYTE)
				return -EINVAL;

			*data = no_os_field_prep(ADES1754_MSB_MASK, crc[5]) | crc[4];
		}
	}

	return 0;
}

/**
 * @brief ADES1754 update device command message.
 * @param desc - ADES1754 device descriptor.
 * @param reg - Register Address.
 * @param mask - Mask of field to be updated.
 * @param val - Data to be updated.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_update_dev(struct ades1754_desc *desc, uint8_t reg, uint16_t mask,
			uint16_t val)
{
	uint16_t reg_val;
	int ret;

	ret = ades1754_read_dev(desc, reg, &reg_val);
	if (ret < 0)
		return ret;

	reg_val &= ~mask;
	reg_val |= no_os_field_prep(mask, val);

	return ades1754_write_dev(desc, reg, reg_val);
}

/**
 * @brief Set ADC scan method.
 * @param desc - ADES1754 device descriptor.
 * @param scan_method - Specific pre-defined value for the Scan Method.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_adc_method(struct ades1754_desc *desc,
			    enum ades1754_scan_method scan_method)
{
	return ades1754_update_dev(desc, ADES1754_SCANCTRL_REG,
				   ADES1754_ADC_METHOD_MASK, scan_method);
}

/**
 * @brief Select Scan Mode of the cells.
 * @param desc - ADES1754 device descriptor.
 * @param mode - Specific pre-defined value for the Cell Scan Mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_switch_scan_mode(struct ades1754_desc *desc,
			      enum ades1754_scan_mode mode)
{
	int ret;

	ret = ades1754_update_dev(desc, ADES1754_SCANCTRL_REG,
				  ADES1754_SCAN_MODE_MASK, mode);
	if (ret < 0)
		return ret;

	desc->scan_mode = mode;

	return 0;
}

/**
 * @brief Set Cell Polarity.
 * @param desc - ADES1754 device descriptor.
 * @param cell_polarity - Specific pre-defined value for cell polarity.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_cell_pol(struct ades1754_desc *desc,
			  enum ades1754_cell_polarity cell_polarity)
{
	uint16_t reg_val = 0;
	unsigned int i;
	int ret;

	reg_val = cell_polarity | no_os_field_prep(ADES1754_CELL_POLARITY_MASK,
			cell_polarity);

	ret = ades1754_write_dev(desc, ADES1754_POLARITYCTRL_REG, reg_val);
	if (ret < 0)
		return ret;

	desc->cell_polarity = cell_polarity;

	return 0;
}

/**
 * @brief ADES1754 Start/Stop Scanning function.
 * @param desc - ADES1754 device descriptor.
 * @param meas - true - Starts scan.
 * 		 false - Stops scan.
 * @param cell_mask - Specific Cell Mask word (each bit set means the cell
 * 		      is selected for measurement).
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_start_scan(struct ades1754_desc *desc, bool meas,
			uint16_t cell_mask)
{
	int ret;

	if (cell_mask > NO_OS_GENMASK(14, 1))
		return -EINVAL;

	ret = ades1754_write_dev(desc, ADES1754_MEASUREEN1_REG, cell_mask);
	if (ret < 0)
		return ret;

	return ades1754_update_dev(desc, ADES1754_SCANCTRL_REG,
				   ADES1754_SCAN_REQUEST_MASK, meas);
}

/**
 * @brief ADES1754 read cell data.
 * @param desc - ADES1754 device descriptor.
 * @param cell_nb - Selected Cell Number (0 to 15).
 * @param cell_voltage - Raw Cell Voltage data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_get_cell_data(struct ades1754_desc *desc, uint8_t cell_nb,
			   int32_t *cell_voltage)
{
	uint16_t reg_val;
	int ret;

	ret = ades1754_read_dev(desc, ADES1754_CELLN_REG(cell_nb), &reg_val);
	if (ret < 0)
		return ret;

	*cell_voltage = reg_val;

	if (no_os_field_get(NO_OS_BIT(cell_nb), desc->cell_polarity))
		*cell_voltage = no_os_sign_extend32(reg_val, 13);

	return 0;
}

/**
 * @brief Set IIR filter coefficient.
 * @param desc - ADES1754 device descriptor.
 * @param coef - Specific pre-defined IIR coefficient value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_iir(struct ades1754_desc *desc,
		     enum ades1754_iir_filter_coef coef)
{
	return ades1754_update_dev(desc, ADES1754_DEVCFG2_REG,
				   ADES1754_IIR_MASK, coef);
}

/**
 * @brief Set speciic IIR filter settings.
 * @param desc - ADES1754 device descriptor.
 * @param alrtfilt - true - Alert Issuance Based on IIR Filter Results.
 * 		     false - Alert Issuance Based on Raw Sequencer Results.
 * @param acc - true - ADC result is included in the IIR accumulator.
 * 		false - ADC result is not included in the IIR accumulator.
 * @param output - true - IIR Filtered ADC data is loaded into the output data
 * 		          registers.
 * 		   false - Unfiltered ADC data is loaded into the output data
 * 			   registers.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_iir_ctrl(struct ades1754_desc *desc, bool alrtfilt, bool acc,
			  bool output)
{
	uint16_t val;

	val = no_os_field_prep(ADES1754_ALRTFILT_MASK, alrtfilt);
	val |= no_os_field_prep(ADES1754_AMENDFILT_MASK, acc);
	val |= no_os_field_prep(ADES1754_RDFILT_MASK, output);

	return ades1754_update_dev(desc, ADES1754_SCANCTRL_REG,
				   ADES1754_IIR_SCAN_MASK, val);
}

/**
 * @brief Set Buffer mode for data acquisition.
 * @param desc - ADES1754 device descriptor.
 * @param mode - Specific pre-defined value for Buffer mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_buffer_mode(struct ades1754_desc *desc,
			     enum ades1754_buffer_mode mode)
{
	return ades1754_update_dev(desc, ADES1754_DEVCFG_REG,
				   ADES1754_DBLBUFEN_MASK, mode);
}

/**
 * @brief Set specific alert thershold value.
 * @param desc - ADES1754 device descriptor.
 * @param alert - Specific pre-defined Alert selection.
 * @param thr - Threshold value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_alert_thr(struct ades1754_desc *desc,
			   enum ades1754_alert alert,
			   uint16_t thr)
{
	uint8_t reg, reg2;
	uint16_t mask;
	int ret;

	switch (alert) {
	case ADES1754_CELL_OV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_OVTHCLR_REG;
		reg2 = ADES1754_OVTHSET_REG;
		break;
	case ADES1754_CELL_UV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_UVTHCLR_REG;
		reg2 = ADES1754_UVTHSET_REG;
		break;
	case ADES1754_BIPOLAR_OV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_BIPOVTHCLR_REG;
		reg2 = ADES1754_BIPOVTHSET_REG;
		break;
	case ADES1754_BIPOLAR_UV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_BIPUVTHCLR_REG;
		reg2 = ADES1754_BIPUVTHSET_REG;
		break;
	case ADES1754_BLOCK_OV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_BLKOVTHCLR_REG;
		reg2 = ADES1754_BLKOVTHSET_REG;
		break;
	case ADES1754_BLOCK_UV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_BLKUVTHCLR_REG;
		reg2 = ADES1754_BLKUVTHSET_REG;
		break;
	case ADES1754_CELL_MISMATCH:
		mask = NO_OS_GENMASK(15, 2);

		return ades1754_write_dev(desc, ADES1754_MSMTCH_REG,
					  no_os_field_prep(mask, thr));
	case ADES1754_AUXIN_OV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_AUXAOVTHCLR_REG;
		reg2 = ADES1754_AUXAOVTHSET_REG;
		break;
	case ADES1754_AUXIN_UV:
		mask = NO_OS_GENMASK(15, 2);
		reg = ADES1754_AUXAUVTHCLR_REG;
		reg2 = ADES1754_AUXAUVTHSET_REG;
		break;
	default:
		return -EINVAL;
	}

	ret = ades1754_write_dev(desc, reg, no_os_field_prep(mask, thr));
	if (ret < 0)
		return ret;

	return ades1754_write_dev(desc, reg2, no_os_field_prep(mask, thr));
}

/**
 * @brief Read specific alert state.
 * @param desc - ADES1754 device descriptor.
 * @param alert - Specific pre-defined Alert selection.
 * @param enable - true - Specific alert has triggered.
 * 		   false - Specific alert didn't trigger.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_get_alert(struct ades1754_desc *desc, enum ades1754_alert alert,
		       bool *enable)
{
	uint16_t mask, reg_val;
	int ret;

	switch (alert) {
	case ADES1754_CELL_OV:
	case ADES1754_BIPOLAR_OV:
		mask = NO_OS_BIT(12);
		break;
	case ADES1754_CELL_UV:
	case ADES1754_BIPOLAR_UV:
		mask = NO_OS_BIT(11);
		break;
	case ADES1754_BLOCK_OV:
		mask = NO_OS_BIT(10);
		break;
	case ADES1754_BLOCK_UV:
		mask = NO_OS_BIT(9);
		break;
	case ADES1754_CELL_MISMATCH:
		mask = NO_OS_BIT(13);
		break;
	case ADES1754_AUXIN_OV:
		mask = NO_OS_BIT(8);
		break;
	case ADES1754_AUXIN_UV:
		mask = NO_OS_BIT(7);
		break;
	default:
		return -EINVAL;
	}

	ret = ades1754_read_dev(desc, ADES1754_STATUS1_REG, &reg_val);
	if (ret < 0)
		return ret;

	*enable = no_os_field_get(mask, reg_val);

	return 0;
}

/**
 * @brief Set Cell Balancing mode.
 * @param desc - ADES1754 device descriptor.
 * @param mode - Specfic pre-defined value for Cell Balancing mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_balancing_mode(struct ades1754_desc *desc,
				enum ades1754_bal_mode mode)
{
	return ades1754_update_dev(desc, ADES1754_BALCTRL_REG,
				   ADES1754_CBMODE_MASK, mode);
}

/**
 * @brief Cell-Ballancing Measurement Enable/Disable.
 * @param desc - ADES1754 device descriptor.
 * @param meas - Specific pre-defined value for Cell-Balancing Measurement
 * 		 state.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_balancing_meas(struct ades1754_desc *desc,
				enum ades1754_bal_meas meas)
{
	return ades1754_update_dev(desc, ADES1754_BALCTRL_REG,
				   ADES1754_CBMEASEN_MASK, meas);
}

/**
 * @brief Cell-Balancing Calibration Period Selection
 * @param desc - ADES1754 device descriptor.
 * @param calib - Specific pre-defined value for Cell-Balancing Calibration
 * 		  Period number.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_set_balancing_calib(struct ades1754_desc *desc,
				 enum ades1754_bal_calib calib)
{
	return ades1754_update_dev(desc, ADES1754_BALDLYCTRL_REG,
				   ADES1754_CBCALDLY_MASK, calib);
}

/**
 * @brief ADES1754 device intialization function.
 * @param desc - ADES1754 device descriptor.
 * @param init_param - ADES1754 initialization parameter.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_init(struct ades1754_desc **desc,
		  struct ades1754_init_param *init_param)
{
	struct ades1754_desc *descriptor;
	int ret;

	if (init_param->dev_addr > ADES1754_DEV_ADDR_MAX)
		return -EINVAL;

	descriptor = (struct ades1754_desc *)no_os_calloc(sizeof(*descriptor),
			1);
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_uart_init(&descriptor->uart_desc,
			      init_param->uart_param);
	if (ret)
		goto free_desc;

	no_os_crc8_populate_msb(crc_table, 0x4D); // 0xB2 if 0xA6 doesn't work

	descriptor->uart_bridge = init_param->uart_bridge;
	descriptor->dev_addr = init_param->dev_addr;
	descriptor->no_dev = init_param->no_dev;

	ret = ades1754_hello_all(descriptor);
	if (ret < 0)
		goto free_comm;

	descriptor->alive = false;
	descriptor->id = init_param->id;

	*desc = descriptor;

	return 0;

free_comm:
	no_os_uart_remove(descriptor->uart_desc);
free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Deallocate any resources used by the initialization function.@
 * @param desc - ADES1754 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ades1754_remove(struct ades1754_desc *desc)
{
	no_os_mdelay(10);
	no_os_uart_remove(desc->uart_desc);
	no_os_free(desc);

	return 0;
}
