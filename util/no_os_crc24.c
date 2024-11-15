/***************************************************************************//**
 *   @file   no_os_crc24.c
 *   @brief  Source file of CRC-24 computation.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "no_os_crc24.h"

/***************************************************************************//**
 * @brief Creates the CRC-24 lookup table for a given polynomial.
 *
 * @param table      - Pointer to a CRC-24 lookup table to write to.
 * @param polynomial - msb-first representation of desired polynomial.
 *
 * Polynomials in CRC algorithms are typically represented as shown below.
 *
 *    poly = x^24 + x^22 + x^20 + x^19 + x^18 + x^16 + x^14 + x^13 + x^11 +
 *           x^10 + x^8 + x^7 + x^6 + x^3 + x^1 + 1
 *
 * Using msb-first direction, x^24 maps to the msb.
 *
 *    msb first: poly = (1)010111010110110111001011 = 5D6DCB
 *                         ^
 *
 * @return None.
*******************************************************************************/
void no_os_crc24_populate_msb(uint32_t * table, const uint32_t polynomial)
{
	if (!table)
		return;

	for (int16_t n = 0; n < NO_OS_CRC24_TABLE_SIZE; n++) {
		uint32_t currByte = (uint32_t)(n << 16);
		for (uint8_t bit = 0; bit < 8; bit++) {
			if ((currByte & 0x800000) != 0) {
				currByte &= 0x7FFFFF;
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
 * @brief Computes the CRC-24 over a buffer of data.
 *
 * @param table     - Pointer to a CRC-24 lookup table for the desired polynomial.
 * @param pdata     - Pointer to data buffer.
 * @param nbytes    - Number of bytes to compute the CRC-24 over.
 * @param crc       - Initial value for the CRC-24 computation. Can be used to
 *                    cascade calls to this function by providing a previous
 *                    output of this function as the crc parameter.
 *
 * @return crc      - Computed CRC-24 value.
*******************************************************************************/
uint32_t no_os_crc24(const uint32_t * table, const uint8_t *pdata,
		     size_t nbytes,
		     uint32_t crc)
{
	unsigned int idx;

	while (nbytes--) {
		idx = ((crc >> 16) ^ *pdata) & 0xff;
		crc = (table[idx] ^ (crc << 8)) & 0xffffff;
		pdata++;
	}

	return (crc & 0xffffff);
}
