/***************************************************************************//**
 *   @file   no_os_crc16.c
 *   @brief  Source file of CRC-16 computation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "no_os_crc16.h"

/***************************************************************************//**
 * @brief Creates the CRC-16 lookup table for a given polynomial.
 *
 * @param table      - Pointer to a CRC-16 lookup table to write to.
 * @param polynomial - Msb-first representation of desired polynomial.
 *
 * Polynomials in CRC algorithms are typically represented as shown below.
 *
 *    poly = x^16 + x^14 + x^13 + x^12 + x^10 + x^8 + x^6 + x^4 + x^3 +
 *           x^1 + 1
 *
 * Using msb-first direction, x^15 maps to the msb.
 *
 *    msb first: poly = (1)0111010101011011 = 0x755B
 *                         ^
 *
 * @return None.
*******************************************************************************/
void no_os_crc16_populate_msb(uint16_t * table, const uint16_t polynomial)
{
	if (!table)
		return;

	for (int16_t n = 0; n < NO_OS_CRC16_TABLE_SIZE; n++) {
		uint16_t currByte = (uint16_t)(n << 8);
		for (uint8_t bit = 0; bit < 8; bit++) {
			if ((currByte & 0x8000) != 0) {
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
 * @brief Computes the CRC-16 over a buffer of data.
 *
 * @param table     - Pointer to a CRC-16 lookup table for the desired polynomial.
 * @param pdata     - Pointer to data buffer.
 * @param nbytes    - Number of bytes to compute the CRC-16 over.
 * @param crc       - Initial value for the CRC-16 computation. Can be used to
 *                    cascade calls to this function by providing a previous
 *                    output of this function as the crc parameter.
 *
 * @return crc      - Computed CRC-16 value.
*******************************************************************************/
uint16_t no_os_crc16(const uint16_t * table, const uint8_t *pdata,
		     size_t nbytes,
		     uint16_t crc)
{
	unsigned int idx;

	while (nbytes--) {
		idx = ((crc >> 8) ^ *pdata) & 0xff;
		crc = (table[idx] ^ (crc << 8)) & 0xffff;
		pdata++;
	}

	return crc;
}
