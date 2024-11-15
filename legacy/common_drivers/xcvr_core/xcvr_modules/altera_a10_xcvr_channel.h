/*******************************************************************************
 * @file altera_a10_xcvr_channel.h
 * @brief Implementation of a10 cdr/cmu dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef ALTERA
#ifndef A10_XCVR_CHANNEL_H_
#define A10_XCVR_CHANNEL_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"
#include "xcvr_core.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* xcvr PMA (CMU CDR PLL) parameter definitions are pll_common.h*/

#define A10_CDR_PLL_PFD_MIN 50000    /*  50.00 Mhz */
#define A10_CDR_PLL_PFD_MAX 800000   /* 800.00 Mhz */
#define A10_CDR_PLL_VCO_MIN 4900000  /*   4.90 GHz */
#define A10_CDR_PLL_VCO_MAX 14150000 /*  14.15 GHz */

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

uint32_t altera_a10_cdr_pll_round_rate(uint32_t fout_khz, uint32_t fref_khz);

int32_t altera_a10_cdr_pll_set_rate(xcvr_core *core,
	uint32_t fout_khz, uint32_t fref_khz);

int32_t altera_a10_calib_tx_pll(xcvr_core *core);

#endif
#endif

/******************************************************************************/
/******************************************************************************/
