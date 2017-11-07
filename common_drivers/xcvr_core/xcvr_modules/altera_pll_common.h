/*******************************************************************************
 * @file altera_pll_common.h
 * @brief Implementation of a10-fpll Driver.
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
#ifndef PLL_COMMON_H_
#define PLL_COMMON_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum {
	atx_type,
	fpll_type,
	cmu_cdr_type,
	cmu_tx_type,
} pll_type;

typedef struct {
	pll_type		type;
	uint32_t		base_address;
	uint8_t			initial_recalc;
	uint8_t			enable; // not used
} xcvr_pll;

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* xcvr ATX_PLL and FPLL */
#define XCVR_REG_CALIB_PLL_EN			0x100
#define XCVR_CALIB_PLL_EN_MASK			0x01
#define XCVR_CALIB_PLL_EN			0x01

#define XCVR_REG_CAPAB_PLL_STAT			0x280
#define XCVR_CAPAB_PLL_CAL_BSY_MASK		0x02
#define XCVR_CAPAB_PLL_CAL_DONE			0x00

/* xcvr PMA (CMU CDR PLL) */
#define XCVR_REG_CALIB_PMA_EN			0x100
#define XCVR_CALIB_TX_TERM_VOD_MASK		0x20
#define XCVR_CALIB_TX_TERM_VOD_EN		0x20
#define XCVR_CALIB_CMU_CDR_PLL_EN_MASK		0x02
#define XCVR_CALIB_CMU_CDR_PLL_EN		0x02

#define XCVR_REG_CAPAB_PMA			0x281
#define XCVR_CAPAB_RX_CAL_BUSY_EN_MASK		0x20
#define XCVR_CAPAB_RX_CAL_BUSY_EN		0x20
#define XCVR_CAPAB_RX_CAL_BUSY_DIS		0x00
#define XCVR_CAPAB_RX_CAL_BUSY_MASK		0x02
#define XCVR_CAPAB_RX_CAL_DONE			0x00
#define XCVR_CAPAB_TX_CAL_BUSY_EN_MASK		0x10
#define XCVR_CAPAB_TX_CAL_BUSY_EN		0x10
#define XCVR_CAPAB_TX_CAL_BUSY_DIS		0x00
#define XCVR_CAPAB_TX_CAL_BUSY_MASK		0x01
#define XCVR_CAPAB_TX_CAL_DONE			0x00

#define XCVR_REG_RATE_SWITCH_FLAG		0x166
#define XCVR_RATE_SWITCH_FLAG_MASK		0x80
#define XCVR_RATE_SWITCH_FLAG_RATE_SWITCH	0x00
#define XCVR_RATE_SWITCH_FLAG_NO_RATE_SWITCH	0x80

/* common */
#define XCVR_REG_ARBITRATION			0x000
#define XCVR_ARBITRATION_MASK			0xFF
#define XCVR_ARBITRATION_GET_AVMM		0x02
#define XCVR_ARBITRATION_RELEASE_AVMM_CALIB	0x01
#define XCVR_ARBITRATION_RELEASE_AVMM		0x03

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t a10_pll_read(xcvr_pll *myfpll,
		uint32_t reg_addr,
		uint32_t *reg_data);

int32_t a10_pll_write(xcvr_pll *myfpll,
		uint32_t reg_addr,
		uint32_t reg_data);

void pll_update(xcvr_pll *myfpll,
	uint32_t addr, uint32_t mask, uint32_t val);

uint32_t altera_a10_acquire_arbitration(xcvr_pll *myfpll);

void altera_a10_release_arbitration(xcvr_pll *myfpll,
	uint8_t run_calibration);

uint8_t pll_calibration_check(xcvr_pll *my_fpll);

uint32_t pll_is_enabled(xcvr_pll *myfpll);

uint32_t pll_enable(xcvr_pll *myfpll);

uint32_t pll_disable(xcvr_pll *myfpll);

#endif
#endif

/******************************************************************************/
/******************************************************************************/
