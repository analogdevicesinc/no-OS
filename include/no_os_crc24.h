/***************************************************************************//**
 *   @file   no_os_crc24.h
 *   @brief  Header file of CRC-24 computation.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_CRC24_H_
#define _NO_OS_CRC24_H_

#include <stdint.h>
#include <stddef.h>

#define NO_OS_CRC24_TABLE_SIZE 256

#define NO_OS_DECLARE_CRC24_TABLE(_table) \
	static uint32_t _table[NO_OS_CRC24_TABLE_SIZE]

void no_os_crc24_populate_msb(uint32_t * table, const uint32_t polynomial);
uint32_t no_os_crc24(const uint32_t * table, const uint8_t *pdata,
		     size_t nbytes,
		     uint32_t crc);

#endif // _NO_OS_CRC24_H_
