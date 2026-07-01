/*
 * Copyright (c) 2025 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API Memory Allocation
 *
 * Overrideable allocation suite for CAPI drivers.
 * Provides malloc/free/calloc/realloc functions that can be overridden
 * at link time by providing strong implementations.
 */

#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocate memory block
 *
 * Weak implementation uses standard malloc(). Can be overridden by
 * providing a strong implementation of capi_malloc_impl().
 *
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void *capi_malloc(size_t size);

/**
 * @brief Free memory block
 *
 * Weak implementation uses standard free(). Can be overridden by
 * providing a strong implementation of capi_free_impl().
 *
 * @param ptr Pointer to memory block to free
 */
void capi_free(void *ptr);

/**
 * @brief Allocate and zero-initialize memory block
 *
 * Weak implementation uses standard calloc(). Can be overridden by
 * providing a strong implementation of capi_calloc_impl().
 *
 * @param num Number of elements
 * @param size Size of each element in bytes
 * @return Pointer to allocated and zero-initialized memory, or NULL on failure
 */
void *capi_calloc(size_t num, size_t size);

/**
 * @brief Reallocate memory block
 *
 * Weak implementation uses standard realloc(). Can be overridden by
 * providing a strong implementation of capi_realloc_impl().
 *
 * @param ptr Pointer to existing memory block (or NULL)
 * @param size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void *capi_realloc(void *ptr, size_t size);

/* Weak hook symbols to be overridden by the platform/app (don't call directly). */
void *capi_malloc_impl(size_t size);
void capi_free_impl(void *ptr);
void *capi_calloc_impl(size_t num, size_t size);
void *capi_realloc_impl(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif
