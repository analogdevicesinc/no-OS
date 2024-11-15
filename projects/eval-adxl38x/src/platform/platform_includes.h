/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by eval-adxl355-pmdz project.
 *   @author BRajendran (balarupini.rajendran@analog.com)
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
#ifdef STM32_PLATFORM
#include "stm32/parameters.h"
#endif

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
