/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "capi_alloc.h"
#include "no_os_alloc.h"
#include <stdlib.h>

void *capi_malloc_impl(size_t size)
{
	return no_os_malloc(size);
}

void capi_free_impl(void *ptr)
{
	no_os_free(ptr);
}

void *capi_calloc_impl(size_t num, size_t size)
{
	return no_os_calloc(num, size);
}

void *capi_realloc_impl(void *ptr, size_t size)
{
	/* no_os_alloc.h has no realloc counterpart. */
	return realloc(ptr, size);
}
