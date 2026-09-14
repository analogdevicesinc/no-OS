/***************************************************************************//**
 *   @file   no_os_crc32.c
 *   @brief  Source file of CRC-32 computation.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_crc32.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Compute a bitwise big-endian Ethernet AUTODIN II CRC-32.
 *
 * MSB-first, polynomial NO_OS_CRC32_POLY_BE, with no input or output bit
 * reflection and no final XOR - so the seed and the result are both raw. This
 * is the same convention as the Linux kernel's crc32_be(), which some device
 * profiles are checksummed with, and the two must agree byte for byte.
 *
 * Unlike no_os_crc8/16/24 this needs no lookup table. Those are called
 * repeatedly on short frames, where a table earns its 256 entries; the callers
 * here checksum a single multi-kilobyte buffer once at init, so the shift-based
 * form is both smaller and easier to check against the reference.
 *
 * @param crc    - Seed. 0 for a fresh computation, or the previous return value
 *                 to continue one incrementally.
 * @param pdata  - Buffer to compute the CRC over.
 * @param nbytes - Length of pdata in bytes.
 * @return The computed CRC-32.
 */
uint32_t no_os_crc32_be(uint32_t crc, const uint8_t *pdata, size_t nbytes)
{
	unsigned int i;

	if (!pdata)
		return crc;

	while (nbytes--) {
		crc ^= (uint32_t)*pdata++ << 24;

		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x80000000) ?
					    NO_OS_CRC32_POLY_BE : 0);
	}

	return crc;
}
