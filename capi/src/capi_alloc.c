/*
 * Copyright (c) 2025 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "capi_alloc.h"

/* Default weak implementations: no-op implementations that return NULL/do nothing */

__attribute__((weak)) void *capi_malloc_impl(size_t size)
{
	(void)size;
	return NULL;
}

__attribute__((weak)) void capi_free_impl(void *ptr)
{
	(void)ptr;
}

__attribute__((weak)) void *capi_calloc_impl(size_t num, size_t size)
{
	(void)num;
	(void)size;
	return NULL;
}

__attribute__((weak)) void *capi_realloc_impl(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
	return NULL;
}

/* Strong wrapper functions that call the (possibly overridden) implementations */

void *capi_malloc(size_t size)
{
	return capi_malloc_impl(size);
}

void capi_free(void *ptr)
{
	capi_free_impl(ptr);
}

void *capi_calloc(size_t num, size_t size)
{
	return capi_calloc_impl(num, size);
}

void *capi_realloc(void *ptr, size_t size)
{
	return capi_realloc_impl(ptr, size);
}