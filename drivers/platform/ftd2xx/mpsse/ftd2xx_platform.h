/***************************************************************************//**
 *   @file   ftd2xx_platform.h
 *   @brief  Implementation of HAL for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
********************************************************************************/
#ifndef FTD2XX_PLATFORM_H_
#define FTD2XX_PLATFORM_H_
#include "ftd2xx.h"

#define FTD2XX_MAX_DEV_PER_CHIP		4

#if _WIN32
#define FTDIMPSSE_API
#endif

extern FT_HANDLE ftHandle[FTD2XX_MAX_DEV_PER_CHIP];

#endif /* FTD2XX_PLATFORM_H_ */
