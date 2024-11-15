/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by demo_esp project.
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
#ifdef ADUCM_PLATFORM
#include "aducm3029/parameters.h"
#endif

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
