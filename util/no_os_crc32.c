/***************************************************************************//**
 *   @file   no_os_crc32.c
 *   @brief  Source file of CRC-32 computation.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
 * reflection and no final XOR - so the seed and the result are both raw.
 *
 * Unlike no_os_crc8/16/24 this needs no lookup table. Those are called
 * repeatedly on short frames, where a table earns its 256 entries; a checksum
 * taken once over a single large buffer does not, and the shift-based form is
 * smaller and easier to check against a reference.
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
		crc ^= (uint32_t)(*pdata++) << 24;

		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x80000000) ?
					    NO_OS_CRC32_POLY_BE : 0);
	}

	return crc;
}
