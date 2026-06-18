/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common Platform Definitions
 */

#ifndef _CAPI_PLATFORM_H_
#define _CAPI_PLATFORM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Read from the specified memory address and return the
 *        8 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 8 bit read value.
 */
#define CAPI_PLATFORM_IO_READ8(addr) UINT8_C(0x00)

/**
 * @brief Read from the specified memory address and return the
 *        16 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 16 bit read value.
 */
#define CAPI_PLATFORM_IO_READ16(addr) UINT16_C(0x0000)

/**
 * @brief Read from the specified memory address and return the
 *        32 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 32 bit read value.
 */
#define CAPI_PLATFORM_IO_READ32(addr) UINT32_C(0x00000000)

/**
 * @brief Read from the specified memory address and return the
 *        64 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 64 bit read value.
 */
#define CAPI_PLATFORM_IO_READ64(addr) UINT64_C(0x0000000000000000)

/**
 * @brief Write to the specified memory address the specified
 *        8 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 8 bit value to write.
 *
 * @return None.
 */
#define CAPI_PLATFORM_IO_WRITE8(addr, val)

/**
 * @brief Write to the specified memory address the specified
 *        16 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 16 bit value to write.
 *
 * @return None.
 */
#define CAPI_PLATFORM_IO_WRITE16(addr, val)

/**
 * @brief Write to the specified memory address the specified
 *        32 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 32 bit value to write.
 *
 * @return None.
 */
#define CAPI_PLATFORM_IO_WRITE32(addr, val)

/**
 * @brief Write to the specified memory address the specified
 *        64 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 64 bit value to write.
 *
 * @return None.
 */
#define CAPI_PLATFORM_IO_WRITE64(addr, val)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
