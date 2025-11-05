/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CAPI_IO_H_
#define _CAPI_IO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "capi_platform.h"

/**
 * @brief Read from the specified memory address and return the
 *        8 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 8 bit read value.
 */
#define capi_io_read8(addr) CAPI_PLATFORM_IO_READ8(addr)

/**
 * @brief Read from the specified memory address and return the
 *        16 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 16 bit read value.
 */
#define capi_io_read16(addr) CAPI_PLATFORM_IO_READ16(addr)

/**
 * @brief Read from the specified memory address and return the
 *        32 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 32 bit read value.
 */
#define capi_io_read32(addr) CAPI_PLATFORM_IO_READ32(addr)

/**
 * @brief Read from the specified memory address and return the
 *        64 bit read value.
 *
 * @param [in] addr The memory address to read from.
 *
 * @return The 64 bit read value.
 */
#define capi_io_read64(addr) CAPI_PLATFORM_IO_READ64(addr)

/**
 * @brief Write to the specified memory address the specified
 *        8 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 8 bit value to write.
 *
 * @return None.
 */
#define capi_io_write8(addr, val) CAPI_PLATFORM_IO_WRITE8(addr, val)

/**
 * @brief Write to the specified memory address the specified
 *        16 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 16 bit value to write.
 *
 * @return None.
 */
#define capi_io_write16(addr, val) CAPI_PLATFORM_IO_WRITE16(addr, val)

/**
 * @brief Write to the specified memory address the specified
 *        32 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 32 bit value to write.
 *
 * @return None.
 */
#define capi_io_write32(addr, val) CAPI_PLATFORM_IO_WRITE32(addr, val)

/**
 * @brief Write to the specified memory address the specified
 *        64 bit value.
 *
 * @param [in] addr The memory address to write to.
 * @param [in] val The 64 bit value to write.
 *
 * @return None.
 */
#define capi_io_write64(addr, val) CAPI_PLATFORM_IO_WRITE64(addr, val)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAPI_IO_H_ */
