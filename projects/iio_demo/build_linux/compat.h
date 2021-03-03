/*
 * libtinyiiod - Tiny IIO Daemon Library
 *
 * Copyright (C) 2019 Analog Devices, Inc.
 * Author: Cristian Pop <cristian.pop@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef COMPAT_H
#define COMPAT_H

#ifdef _USE_STD_INT_TYPES
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#else // _USE_STD_INT_TYPES
#if !defined(__ssize_t_defined) && !defined(_SSIZE_T_DEFINED)
typedef long int ssize_t;
typedef unsigned long int size_t;
#define __ssize_t_defined
#define _SSIZE_T_DEFINED
#endif

typedef long int32_t;
typedef unsigned long uint32_t;
typedef int bool;

#define true 1
#define false 0

#define NULL (void*)0

#define ENOENT		2	/* No such file or directory */
#define EIO		5	/* I/O error */
#define ENODEV		19	/* No such device */
#define EINVAL		22	/* Invalid argument */
#define ENOSYS		38	/* Function not implemented */

#define PRIi32		"li"
# define PRIx32		"x"
# define PRIu16		"u"
#endif //_USE_STD_INT_TYPES

#endif /* COMPAT_H */
