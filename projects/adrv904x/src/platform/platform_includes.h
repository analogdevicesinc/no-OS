/***************************************************************************//**
 *   @file   adrv904x/src/platform/platform_includes.h
 *   @brief  Includes for used platforms used by adrv904x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef PLATFORM_INCLUDES_H_
#define PLATFORM_INCLUDES_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#ifdef XILINX_PLATFORM
#include "xilinx/parameters.h"
#include "xparameters.h"
#include <xil_cache.h>
#endif

#endif /* PLATFORM_INCLUDES_H_ */
