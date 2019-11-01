/***************************************************************************//**
 *   @file   altera_adxcvr.h
 *   @brief  Driver for the Altera ADXCVR Configuration.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
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
#ifndef ALTERA_ADXCVR_H_
#define ALTERA_ADXCVR_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* XCVR Registers */

#define XCVR_REG_ARBITRATION				0x000
#define XCVR_ARBITRATION_MASK				0xFF
#define XCVR_ARBITRATION_GET_AVMM			0x02
#define XCVR_ARBITRATION_RELEASE_AVMM_CALIB	0x01
#define XCVR_ARBITRATION_RELEASE_AVMM		0x03

#define XCVR_REG_CALIB_ATX_PLL_EN		0x100
#define XCVR_CALIB_ATX_PLL_EN_MASK		0x01
#define XCVR_CALIB_ATX_PLL_EN			0x01

#define XCVR_REG_CAPAB_ATX_PLL_STAT		0x280
#define XCVR_CAPAB_ATX_PLL_CAL_BSY_MASK	0x02
#define XCVR_CAPAB_ATX_PLL_CAL_DONE		0x00


#define XCVR_REG_CALIB_PMA_EN			0x100
#define XCVR_CALIB_TX_TERM_VOD_MASK		0x20
#define XCVR_CALIB_TX_TERM_VOD_EN		0x20
#define XCVR_CALIB_CMU_CDR_PLL_EN_MASK	0x02
#define XCVR_CALIB_CMU_CDR_PLL_EN		0x02

#define XCVR_REG_CAPAB_PMA				0x281
#define XCVR_CAPAB_RX_CAL_BUSY_EN_MASK	0x20
#define XCVR_CAPAB_RX_CAL_BUSY_EN		0x20
#define XCVR_CAPAB_RX_CAL_BUSY_DIS		0x00
#define XCVR_CAPAB_RX_CAL_BUSY_MASK		0x02
#define XCVR_CAPAB_RX_CAL_DONE			0x00
#define XCVR_CAPAB_TX_CAL_BUSY_EN_MASK	0x10
#define XCVR_CAPAB_TX_CAL_BUSY_EN		0x10
#define XCVR_CAPAB_TX_CAL_BUSY_DIS		0x00
#define XCVR_CAPAB_TX_CAL_BUSY_MASK		0x01
#define XCVR_CAPAB_TX_CAL_DONE			0x00

#define XCVR_REG_RATE_SWITCH_FLAG				0x166
#define XCVR_RATE_SWITCH_FLAG_MASK				0x80
#define XCVR_RATE_SWITCH_FLAG_RATE_SWITCH		0x00
#define XCVR_RATE_SWITCH_FLAG_NO_RATE_SWITCH	0x80

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct adxcvr {
	const char *name;
	uint32_t base;
	bool is_transmit;
	uint32_t lanes_per_link;
	uint32_t adxcfg_base[4];
	uint32_t atx_pll_base;
	uint32_t reset_counter;
	uint32_t lane_rate_khz;
	uint32_t parent_rate_khz;
	bool initial_recalc;
};

struct adxcvr_init {
	const char *name;
	uint32_t base;
	uint32_t adxcfg_base[4];
	uint32_t atx_pll_base;
	uint32_t lane_rate_khz;
	uint32_t parent_rate_khz;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t adxcvr_init(struct adxcvr **ad_xcvr,
		    const struct adxcvr_init *init);
int32_t adxcvr_remove(struct adxcvr *xcvr);
void adxcvr_post_lane_rate_change(struct adxcvr *xcvr,
				  unsigned int lane_rate_khz);
int32_t adxcvr_round_rate(struct adxcvr *xcvr, uint32_t rate_khz);
int32_t adxcvr_set_rate(struct adxcvr *xcvr,
			uint32_t rate_khz);
uint32_t adxcvr_recalc_rate(struct adxcvr *xcvr);

void adxcvr_pre_lane_rate_change(struct adxcvr *xcvr);
void adxcvr_post_lane_rate_change(struct adxcvr *xcvr,
				  unsigned int lane_rate);

void atx_pll_acquire_arbitration(struct adxcvr *xcvr);
void atx_pll_release_arbitration(struct adxcvr *xcvr,
				 bool calibrate);
int32_t atx_pll_write(struct adxcvr *xcvr, uint32_t reg, uint32_t val);
int32_t atx_pll_read(struct adxcvr *xcvr, uint32_t reg, uint32_t *val);
void atx_pll_update(struct adxcvr *xcvr, uint32_t reg,
		    uint32_t mask, uint32_t val);
int32_t atx_pll_calibration_check(struct adxcvr *xcvr);
int32_t xcvr_calib_tx(struct adxcvr *xcvr);

void adxcfg_acquire_arbitration(struct adxcvr *xcvr,
				uint32_t lane);
void adxcfg_release_arbitration(struct adxcvr *xcvr,
				uint32_t lane, bool calibrate);
int32_t adxcfg_write(struct adxcvr *xcvr, uint32_t lane, uint32_t reg,
		     uint32_t val);
int32_t adxcfg_read(struct adxcvr *xcvr, uint32_t lane,
		    uint32_t reg, uint32_t *val);
void adxcfg_update(struct adxcvr *xcvr, uint32_t lane,
		   uint32_t reg, uint32_t mask, uint32_t val);
int32_t adxcfg_calibration_check(struct adxcvr *xcvr, uint32_t lane,
				 bool tx);
#endif

