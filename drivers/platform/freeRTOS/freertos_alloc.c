/*******************************************************************************
 *   @file   freeRTOS/freertos_alloc.c
 *   @brief  Implementation of freertos allocation functions.
 *   @author Lars Andre Landås (landas@gmail.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "no_os_alloc.h"
#include "portable.h"
#include <string.h>

/**
 * @brief Allocate memory and return a pointer to it.
 * @param size - Size of the memory block, in bytes.
 * @return Pointer to the allocated memory, or NULL if the request fails.
 */
void *no_os_malloc(size_t size)
{
	return pvPortMalloc(size);
}

/**
 * @brief Allocate memory and return a pointer to it, set memory to 0.
 * @param nitems - Number of elements to be allocated.
 * @param size - Size of elements.
 * @return Pointer to the allocated memory, or NULL if the request fails.
 */
void *no_os_calloc(size_t nitems, size_t size)
{
	void *p_ret;
	p_ret = pvPortMalloc(nitems * size);

	if (p_ret != NULL) {
		memset(p_ret, 0, nitems * size);
	}

	return p_ret;
}

/**
 * @brief Deallocate memory previously allocated by a call to no_os_calloc
 * 		  or no_os_malloc.
 * @param ptr - Pointer to a memory block previously allocated by a call
 * 		  to no_os_calloc or no_os_malloc.
 */
void no_os_free(void *ptr)
{
	vPortFree(ptr);
}
