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

void *capi_malloc_impl(size_t size)
{
	return malloc(size);
}

void *capi_calloc_impl(size_t num, size_t size)
{
	return calloc(num, size);
}

void capi_free_impl(void *ptr)
{
	free(ptr);
}
