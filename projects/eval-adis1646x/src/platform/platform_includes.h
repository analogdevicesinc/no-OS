/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by eval-adis1646x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLATFORM_INCLUDES_H__
#define __PLATFORM_INCLUDES_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#ifdef PICO_PLATFORM
#include "pico/parameters.h"
#endif

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

#ifdef STM32_PLATFORM
#include "stm32/parameters.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_app.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
