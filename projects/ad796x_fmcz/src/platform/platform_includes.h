/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by iio project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef PLATFORM_INCLUDES_H_
#define PLATFORM_INCLUDES_H_

#if defined XILINX_PLATFORM
#include "xilinx/parameters.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_app.h"
#endif

#endif /* PLATFORM_INCLUDES_H_ */
