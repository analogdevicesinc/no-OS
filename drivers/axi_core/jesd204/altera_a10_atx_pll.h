/***************************************************************************//**
 *   @file   altera_a10_atx_pll.c
 *   @brief  Driver for the Altera ATX PLL dynamic reconfiguration.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ALTERA_A10_ATX_PLL_H_
#define ALTERA_A10_ATX_PLL_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "altera_adxcvr.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t altera_a10_atx_pll_round_rate(struct adxcvr *xcvr,
				      uint32_t rate_khz);
int32_t altera_a10_atx_pll_set_rate(struct adxcvr *xcvr,
				    uint32_t rate_khz);
uint32_t altera_a10_atx_pll_recalc_rate(struct adxcvr *xcvr);

#endif
