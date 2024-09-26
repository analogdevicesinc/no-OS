/*******************************************************************************
 * @file altera_a10_fpll.h
 * @brief Implementation of a10-fpll dynamic reconfiguration driver.
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
