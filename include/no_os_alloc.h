/*******************************************************************************
 *   @file   no_os_alloc.h
 *   @brief  Header file of memory allocator.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_ALLOC_H_
#define _NO_OS_ALLOC_H_

#include <stdio.h>
#include <stdlib.h>

/* Allocate memory and return a pointer to it */
void *no_os_malloc(size_t size);

/* Allocate memory and return a pointer to it, set memory to 0 */
void *no_os_calloc(size_t nitems, size_t size);

/* Deallocate memory previously allocated by a call to no_os_calloc or
 * no_os_malloc */
void no_os_free(void *ptr);

#endif // _NO_OS_ALLOC_H_
