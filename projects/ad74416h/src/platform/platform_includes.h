/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by eval-ad74416h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#ifdef STM32_PLATFORM
#include "stm32/parameters.h"
#endif

#ifdef MBED_PLATFORM
#include "mbed/parameters.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
