/***************************************************************************//**
 *   @file   no_os_crc8.c
 *   @brief  Source file of CRC-8 computation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
