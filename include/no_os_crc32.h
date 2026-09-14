/***************************************************************************//**
 *   @file   no_os_crc32.h
 *   @brief  Header file of CRC-32 computation.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef _NO_OS_CRC32_H_
#define _NO_OS_CRC32_H_

#include <stdint.h>
#include <stddef.h>

/** Ethernet AUTODIN II CRC-32 polynomial, MSB-first representation. */
#define NO_OS_CRC32_POLY_BE 0x04C11DB7

uint32_t no_os_crc32_be(uint32_t crc, const uint8_t *pdata, size_t nbytes);

#endif // _NO_OS_CRC32_H_
