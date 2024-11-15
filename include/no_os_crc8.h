/***************************************************************************//**
 *   @file   no_os_crc8.h
 *   @brief  Header file of CRC-8 computation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_CRC8_H_
#define _NO_OS_CRC8_H_

#include <stdint.h>
#include <stddef.h>

#define NO_OS_CRC8_TABLE_SIZE 256

#define NO_OS_DECLARE_CRC8_TABLE(_table) \
	static uint8_t _table[NO_OS_CRC8_TABLE_SIZE]

void no_os_crc8_populate_msb(uint8_t * table, const uint8_t polynomial);
uint8_t no_os_crc8(const uint8_t * table, const uint8_t *pdata, size_t nbytes,
		   uint8_t crc);

#endif // _NO_OS_CRC8_H_
