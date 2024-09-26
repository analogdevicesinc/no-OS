/*******************************************************************************
 *   @file   util/no_os_alloc.c
 *   @brief  Implementation of no-OS memory allocation functions.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
