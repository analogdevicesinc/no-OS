/*******************************************************************************
 * @file altera_a10_fpll.h
 * @brief Implementation of a10-fpll dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef ALTERA
#ifndef A10_FPLL_H_
#define A10_FPLL_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"
#include "altera_pll_common.h"

/******************************************************************************/
/******************************* Definitions **********************************/
/******************************************************************************/

/* xcvr FPLL parameter definitions are pll_common.h*/

#define FPLL_REG_C_COUNTER0		0x10D
#define FPLL_REG_C_COUNTER1		0x10E
#define FPLL_REG_C_COUNTER2		0x10F
#define FPLL_REG_C_COUNTER3		0x110
#define FPLL_REG_C_COUNTER4		0x111

#define FPLL_REG_DIV_COUNTER0		0x12B
#define FPLL_REG_DIV_COUNTER1		0x12C

#define A10_FPLL_PFD_MIN		25000    /* 25.000 MHz */
#define A10_FPLL_PFD_MAX		60000    /* 60.000 MHz */
#define A10_FPLL_VCO_MIN		4800000  /*  4.800 GHz */
#define A10_FPLL_VCO_MAX		14025000 /* 14.025 GHz */

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t fpll_calc_params(uint32_t fref,
	uint32_t fout, uint32_t *best_n, uint32_t *best_m,
	uint32_t *best_c0, uint32_t *best_fvco);

uint32_t fpll_round_rate(uint32_t fout_khz, uint32_t fref_khz);

int32_t fpll_set_rate(xcvr_pll *fpll, uint32_t fout_khz,
	uint32_t fref_khz);

uint32_t fpll_recalc_rate(xcvr_pll *fpll,
	uint32_t fref_khz);

#endif
#endif

/******************************************************************************/
/******************************************************************************/
