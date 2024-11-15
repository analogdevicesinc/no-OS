/*******************************************************************************
 * @file altera_a10_atx_pll.h
 * @brief Implementation of a10-atx_pll dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef ALTERA
#ifndef A10_ATX_PLL_H_
#define A10_ATX_PLL_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"
#include "xcvr_core.h"

/******************************************************************************/
/******************************* Definitions **********************************/
/******************************************************************************/

/* xcvr ATX_PLL parameter definitions are pll_common.h*/

#define A10_ATX_PLL_PFD_MIN 61440    /*  61.44 Mhz */
#define A10_ATX_PLL_PFD_MAX 800000   /* 800.00 Mhz */
#define A10_ATX_PLL_VCO_MIN 7200000  /*   7.20 GHz */
#define A10_ATX_PLL_VCO_MAX 14400000 /*  14.40 GHz */

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

void atx_calc_params(uint32_t fref_khz,
	uint32_t fout_khz, uint32_t *best_n, uint32_t *best_m,
	uint32_t *best_l, uint32_t *best_fvco);

uint32_t atx_pll_round_rate(uint32_t fout_khz, uint32_t fref_khz);

uint32_t atx_pll_set_rate(xcvr_core *core,
	uint32_t fout_khz, uint32_t fref_khz);

uint32_t atx_pll_recalc_rate(xcvr_core *core,
	uint32_t fref_khz);

#endif
#endif

/******************************************************************************/
/******************************************************************************/
