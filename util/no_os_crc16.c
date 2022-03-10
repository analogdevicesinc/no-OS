/***************************************************************************//**
 *   @file   no_os_crc16.c
 *   @brief  Source file of CRC-16 computation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
