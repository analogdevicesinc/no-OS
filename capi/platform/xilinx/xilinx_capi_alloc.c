/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "capi_alloc.h"
#include <stdlib.h>

void *capi_malloc_impl(size_t size)
{
	return malloc(size);
}

void capi_free_impl(void *ptr)
{
	free(ptr);
}

void *capi_calloc_impl(size_t num, size_t size)
{
	return calloc(num, size);
}

void *capi_realloc_impl(void *ptr, size_t size)
{
	return realloc(ptr, size);
}
