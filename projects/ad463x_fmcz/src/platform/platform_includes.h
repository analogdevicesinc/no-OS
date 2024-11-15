/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by ad463x_fmcz project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PLATFORM_INCLUDES_H__
#define __PLATFORM_INCLUDES_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#ifdef XILINX_PLATFORM
#include "xilinx/parameters.h"
#elif defined STM32_PLATFORM
#include "stm32/parameters.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_app.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
