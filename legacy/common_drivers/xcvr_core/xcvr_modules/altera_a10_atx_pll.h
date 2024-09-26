/*******************************************************************************
 * @file altera_a10_atx_pll.h
 * @brief Implementation of a10-atx_pll dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

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
