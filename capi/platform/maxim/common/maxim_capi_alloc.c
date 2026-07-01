/*******************************************************************************
 *   @file   maxim_capi_alloc.c
 *   @brief  Override of weak CAPI alloc functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include "capi_alloc.h"

/**
 * @brief Allocate memory block implementation
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void *capi_malloc_impl(size_t size)
{
	return malloc(size);
}

/**
 * @brief Allocate and zero-initialize memory block implementation
 * @param num Number of elements
 * @param size Size of each element in bytes
 * @return Pointer to allocated and zero-initialized memory, or NULL on failure
 */
void *capi_calloc_impl(size_t num, size_t size)
{
	return calloc(num, size);
}

/**
 * @brief Free memory block implementation
 * @param ptr Pointer to memory block to free
 */
void capi_free_impl(void *ptr)
{
	free(ptr);
}

/**
 * @brief Reallocate memory block
 * @param ptr Pointer to existing memory block (or NULL)
 * @param size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void *capi_realloc_impl(void *ptr, size_t size)
{
	return realloc(ptr, size);
}
