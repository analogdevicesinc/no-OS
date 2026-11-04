/***************************************************************************//**
 *   @file   platform_includes.h
 *   @brief  Includes for used platforms used by ad719x project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef PLATFORM_INCLUDES_H_
#define PLATFORM_INCLUDES_H_

#ifdef XILINX_PLATFORM
#include "xilinx/parameters.h"
#include "xparameters.h"
#include <xil_cache.h>
#endif

#endif /* PLATFORM_INCLUDES_H_ */
