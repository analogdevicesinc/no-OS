/***************************************************************************//**
 *   @file   no_os_crc16.h
 *   @brief  Header file of CRC-16 computation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_CRC16_H_
#define _NO_OS_CRC16_H_

#include <stdint.h>
#include <stddef.h>

#define NO_OS_CRC16_TABLE_SIZE 256

#define NO_OS_DECLARE_CRC16_TABLE(_table) \
	static uint16_t _table[NO_OS_CRC16_TABLE_SIZE]

void no_os_crc16_populate_msb(uint16_t * table, const uint16_t polynomial);
uint16_t no_os_crc16(const uint16_t * table, const uint8_t *pdata,
		     size_t nbytes,
		     uint16_t crc);

#endif // _NO_OS_CRC16_H_
