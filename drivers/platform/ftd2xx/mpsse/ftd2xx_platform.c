/***************************************************************************//**
 *   @file   ftd2xx_platform.c
 *   @brief  Implementation of HAL for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "ftd2xx_platform.h"

FT_HANDLE ftHandle[FTD2XX_MAX_DEV_PER_CHIP];
