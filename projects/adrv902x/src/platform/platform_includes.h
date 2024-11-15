/***************************************************************************//**
 *   @file   adrv902x/src/platform/platform_includes.h
 *   @brief  Includes for used platforms used by ad719x project.
 *   @author GMois (george.mois@analog.com)
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
#ifdef XILINX_PLATFORM
#include "xilinx/parameters.h"
#include "xparameters.h"
#include <xil_cache.h>
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
