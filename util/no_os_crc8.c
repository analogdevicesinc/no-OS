/***************************************************************************//**
 *   @file   no_os_crc8.c
 *   @brief  Source file of CRC-8 computation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "no_os_crc8.h"

/***************************************************************************//**
 * @brief Creates the CRC-8 lookup table for a given polynomial.
 *
 * @param table      - Pointer to a CRC-8 lookup table to write to.
 * @param polynomial - msb-first representation of desired polynomial.
 *
 * Polynomials in CRC algorithms are typically represented as shown below.
 *
 *	poly = x^8 + x^2 + x^1 + 1
 *
 * Using msb-first direction, x^7 maps to the msb.
 *
 * 	msb first: poly = (1)00000111 = 0x07
 *
 * @return None.
*******************************************************************************/
void no_os_crc8_populate_msb(uint8_t * table, const uint8_t polynomial)
{
	if (!table)
		return;

	for (int16_t n = 0; n < NO_OS_CRC8_TABLE_SIZE; n++) {
		uint8_t currByte = (uint8_t)n;
		for (uint8_t bit = 0; bit < 8; bit++) {
			if ((currByte & 0x80) != 0) {
				currByte <<= 1;
				currByte ^= polynomial;
			} else {
				currByte <<= 1;
			}
		}
		table[n] = currByte;
	}
}

/***************************************************************************//**
 * @brief Creates the CRC-8 lookup table for a given polynomial.
 *
 * @param table      - Pointer to a CRC-8 lookup table to write to.
 * @param polynomial - lsb-first representation of desired polynomial.
 *
 * Polynomials in CRC algorithms are typically represented as shown below.
 *
 *	poly = x^8 + x^2 + x^1 + 1
 *
 * Using lsb-first direction, x^0 maps to the lsb.
 *
 * 	lsb first: poly = 11111000(1) = 0x1F
 *
 * @return None.
*******************************************************************************/
void no_os_crc8_populate_lsb(uint8_t *table, const uint8_t polynomial)
{
	if (!table)
		return;

	for (int16_t n = 0; n < NO_OS_CRC8_TABLE_SIZE; n++) {
		uint8_t curr_byte = (uint8_t)n;
		for (uint8_t bit = 0; bit < 8; bit++) {
			if (curr_byte & 0x01) {
				curr_byte >>= 1;
				curr_byte ^= polynomial;
			} else {
				curr_byte >>= 1;
			}
		}
		table[n] = curr_byte;
	}
}

/***************************************************************************//**
 * @brief Computes the CRC-8 over a buffer of data.
 *
 * @param table     - Pointer to a CRC-8 lookup table for the desired polynomial.
 * @param pdata     - Pointer to 8-bit data buffer.
 * @param nbytes    - Number of bytes to compute the CRC-8 over.
 * @param crc       - Initial value for the CRC-8 computation. Can be used to
 *                    cascade calls to this function by providing a previous
 *                    output of this function as the crc parameter.
 *
 * @return crc      - Computed CRC-8 value.
*******************************************************************************/
uint8_t no_os_crc8(const uint8_t * table, const uint8_t *pdata, size_t nbytes,
		   uint8_t crc)
{
	unsigned int idx;

	while (nbytes--) {
		idx = (crc ^ *pdata);
		crc = (table[idx]) & 0xff;
		pdata++;
	}

	return crc;
}
