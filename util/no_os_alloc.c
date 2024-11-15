/*******************************************************************************
 *   @file   util/no_os_alloc.c
 *   @brief  Implementation of no-OS memory allocation functions.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "no_os_alloc.h"

/**
 * @brief Allocate memory and return a pointer to it.
 * @param size - Size of the memory block, in bytes.
 * @return Pointer to the allocated memory, or NULL if the request fails.
 */
__attribute__((weak)) void *no_os_malloc(size_t size)
{
	return malloc(size);
}

/**
 * @brief Allocate memory and return a pointer to it, set memory to 0.
 * @param nitems - Number of elements to be allocated.
 * @param size - Size of elements.
 * @return Pointer to the allocated memory, or NULL if the request fails.
 */
__attribute__((weak)) void *no_os_calloc(size_t nitems, size_t size)
{
	return calloc(nitems, size);
}

/**
 * @brief Deallocate memory previously allocated by a call to no_os_calloc
 * 		  or no_os_malloc.
 * @param ptr - Pointer to a memory block previously allocated by a call
 * 		  to no_os_calloc or no_os_malloc.
 * @return None.
 */
__attribute__((weak)) void no_os_free(void *ptr)
{
	free(ptr);
}
