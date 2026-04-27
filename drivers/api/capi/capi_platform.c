/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_platform.c
 * @brief Platform abstraction layer implementation
 *
 * This module provides a thin wrapper over platform-specific I/O operations.
 * Most functionality is implemented as macros in the platform headers.
 */

#include <capi_platform.h>
#include <errno.h>
#include <stddef.h>

/*
 * Platform I/O operations are primarily implemented as macros in the platform
 * headers (capi_platform.h). This source file is mostly for completeness and
 * future extension if needed.
 *
 * The actual I/O operations are defined as:
 * - CAPI_PLATFORM_IO_READ8(addr)
 * - CAPI_PLATFORM_IO_READ16(addr)
 * - CAPI_PLATFORM_IO_READ32(addr)
 * - CAPI_PLATFORM_IO_READ64(addr)
 * - CAPI_PLATFORM_IO_WRITE8(addr, val)
 * - CAPI_PLATFORM_IO_WRITE16(addr, val)
 * - CAPI_PLATFORM_IO_WRITE32(addr, val)
 * - CAPI_PLATFORM_IO_WRITE64(addr, val)
 *
 * For the stub platform, these are redirected to stub functions for testing.
 */

/**
 * @brief Verify platform I/O operation address alignment
 * @param addr Address to check
 * @param size Size in bytes (1, 2, 4, 8)
 * @return 0 if aligned, -EINVAL if misaligned
 */
static inline int capi_platform_check_alignment(uint64_t addr, size_t size)
{
	if (addr % size != 0) {
		return -EINVAL;
	}
	return 0;
}

/**
 * @brief Validate 8-bit read operation (helper for testing)
 * @param addr Memory address
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_read8(uint64_t addr)
{
	return capi_platform_check_alignment(addr, 1);
}

/**
 * @brief Validate 16-bit read operation (helper for testing)
 * @param addr Memory address
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_read16(uint64_t addr)
{
	return capi_platform_check_alignment(addr, 2);
}

/**
 * @brief Validate 32-bit read operation (helper for testing)
 * @param addr Memory address
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_read32(uint64_t addr)
{
	return capi_platform_check_alignment(addr, 4);
}

/**
 * @brief Validate 64-bit read operation (helper for testing)
 * @param addr Memory address
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_read64(uint64_t addr)
{
	return capi_platform_check_alignment(addr, 8);
}

/**
 * @brief Validate 8-bit write operation (helper for testing)
 * @param addr Memory address
 * @param val Value to write
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_write8(uint64_t addr, uint8_t val)
{
	(void)val; // Suppress unused parameter warning
	return capi_platform_check_alignment(addr, 1);
}

/**
 * @brief Validate 16-bit write operation (helper for testing)
 * @param addr Memory address
 * @param val Value to write
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_write16(uint64_t addr, uint16_t val)
{
	(void)val; // Suppress unused parameter warning
	return capi_platform_check_alignment(addr, 2);
}

/**
 * @brief Validate 32-bit write operation (helper for testing)
 * @param addr Memory address
 * @param val Value to write
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_write32(uint64_t addr, uint32_t val)
{
	(void)val; // Suppress unused parameter warning
	return capi_platform_check_alignment(addr, 4);
}

/**
 * @brief Validate 64-bit write operation (helper for testing)
 * @param addr Memory address
 * @param val Value to write
 * @return 0 on success, negative error code on failure
 */
int capi_platform_validate_write64(uint64_t addr, uint64_t val)
{
	(void)val; // Suppress unused parameter warning
	return capi_platform_check_alignment(addr, 8);
}