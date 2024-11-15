/***************************************************************************//**
 * @file clkgen_core.h
 * @brief Header file of CLKGEN Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CLKGEN_CORE_H_
#define CLKGEN_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	base_addr;
} clkgen_device;

typedef struct {
	uint32_t	base_addr;
	uint32_t 	rate;
	uint32_t 	parent_rate;
} clkgen_init_params;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t clkgen_setup(clkgen_device **clkgen_dev,
		clkgen_init_params init_params);

#endif
