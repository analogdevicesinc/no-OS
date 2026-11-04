/*******************************************************************************
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by the project.
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef PLATFORM_INCLUDES_H_
#define PLATFORM_INCLUDES_H_

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_app.h"
#endif

#endif /* PLATFORM_INCLUDES_H_ */
