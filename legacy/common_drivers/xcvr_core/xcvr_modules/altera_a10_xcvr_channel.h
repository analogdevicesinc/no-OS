/*******************************************************************************
 * @file altera_a10_xcvr_channel.h
 * @brief Implementation of a10 cdr/cmu dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

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
