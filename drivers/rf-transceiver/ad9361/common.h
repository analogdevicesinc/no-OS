/***************************************************************************//**
 *   @file   common.h
 *   @brief  Header file of Common Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef COMMON_H_
#define COMMON_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_error.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
#if defined (__STDC__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#else
typedef enum { false, true } bool;
#endif

struct no_os_clk {
	const char	*name;
	uint32_t	rate;
};

struct no_os_clk_hw {
	struct no_os_clk *clk;
};

#endif
