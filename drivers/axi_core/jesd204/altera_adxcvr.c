/***************************************************************************//**
 *   @file   altera_adxcvr.c
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "io.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "altera_a10_atx_pll.h"
#include "altera_a10_cdr_pll.h"
#include "altera_adxcvr.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* ADXCVR Registers */

#define ADXCVR_REG_VERSION				0x0000
#define VERSION(major, minor, letter)	((major << 16) | (minor << 8) | letter)
#define VERSION_MAJOR(version)			(version >> 16)
#define VERSION_MINOR(version)			((version >> 8) & 0xff)
#define VERSION_LETTER(version)			(version & 0xff)

#define ADXCVR_REG_ID				0x0004

#define ADXCVR_REG_SCRATCH			0x0008

#define ADXCVR_REG_RESETN			0x0010
#define ADXCVR_RESETN				(1 << 0)

#define ADXCVR_REG_STATUS			0x0014
#define ADXCVR_STATUS				(1 << 0)

#define ADXCVR_REG_STATUS2			0x0018
#define ADXCVR_STATUS2_XCVR(x)		NO_OS_BIT(x)

#define ADXCVR_REG_SYNTH_CONF		0x0024

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief adxcvr_write
 */
int32_t adxcvr_write(struct adxcvr *xcvr,
		     uint32_t reg_addr,
		     uint32_t reg_val)
{
	IOWR_32DIRECT(xcvr->base, reg_addr, reg_val);

	return 0;
}

/**
 * @brief adxcvr_read
 */
int32_t adxcvr_read(struct adxcvr *xcvr,
		    uint32_t reg_addr,
		    uint32_t *reg_val)
{
	*reg_val = IORD_32DIRECT(xcvr->base, reg_addr);

	return 0;
}

/**
 * @brief adxcvr_acquire_arbitration
 */
void adxcvr_acquire_arbitration(struct adxcvr *xcvr,
				uint32_t addr, uint32_t status_reg)
{
	uint32_t timeout = 0;
	uint32_t val;

	IOWR_32DIRECT(addr, 0, XCVR_ARBITRATION_GET_AVMM);

	do {
		val = IORD_32DIRECT(addr, status_reg * 4);
		if ((val & NO_OS_BIT(2)) == 0)
			return;
		no_os_udelay(10);
	} while (timeout++ < 10000);

	printf("%s: Failed to acquire arbitration\n", xcvr->name);
}

/**
 * @brief adxcvr_release_arbitration
 */
void adxcvr_release_arbitration(uint32_t addr, bool calibrate)
{
	unsigned int val;

	if (calibrate)
		val = XCVR_ARBITRATION_RELEASE_AVMM_CALIB;
	else
		val = XCVR_ARBITRATION_RELEASE_AVMM;
	IOWR_32DIRECT(addr, 0, val);
}

/**
 * @brief atx_pll_write
 */
int32_t atx_pll_write(struct adxcvr *xcvr, uint32_t reg, uint32_t val)
{
	IOWR_32DIRECT(xcvr->atx_pll_base, reg * 4, val);

	return 0;
}

/**
 * @brief atx_pll_read
 */
int32_t atx_pll_read(struct adxcvr *xcvr, uint32_t reg, uint32_t *val)
{
	*val = IORD_32DIRECT(xcvr->atx_pll_base, reg * 4);

	return 0;
}

/**
 * @brief atx_pll_update
 */
void atx_pll_update(struct adxcvr *xcvr, uint32_t reg,
		    uint32_t mask, uint32_t val)
{
	uint32_t rval;

	atx_pll_read(xcvr, reg, &rval);
	rval &= ~mask;
	rval |= val;
	atx_pll_write(xcvr, reg, rval);
}

/**
 * @brief atx_pll_acquire_arbitration
 */
void atx_pll_acquire_arbitration(struct adxcvr *xcvr)
{
	adxcvr_acquire_arbitration(xcvr, xcvr->atx_pll_base,
				   XCVR_REG_CAPAB_ATX_PLL_STAT);
}

/**
 * @brief atx_pll_release_arbitration
 */
void atx_pll_release_arbitration(struct adxcvr *xcvr,
				 bool calibrate)
{
	adxcvr_release_arbitration(xcvr->atx_pll_base, calibrate);
}

/**
 * @brief adxcfg_write
 */
int32_t adxcfg_write(struct adxcvr *xcvr, uint32_t lane, uint32_t reg,
		     uint32_t val)
{
	IOWR_32DIRECT(xcvr->adxcfg_base[lane], reg * 4, val);

	return 0;
}

/**
 * @brief adxcfg_read
 */
int32_t adxcfg_read(struct adxcvr *xcvr, uint32_t lane,
		    uint32_t reg, uint32_t *val)
{
	*val = IORD_32DIRECT(xcvr->adxcfg_base[lane], reg * 4);

	return 0;
}

/**
 * @brief adxcfg_update
 */
void adxcfg_update(struct adxcvr *xcvr, uint32_t lane,
		   uint32_t reg, uint32_t mask, uint32_t val)
{
	uint32_t rval;

	adxcfg_read(xcvr, lane, reg, &rval);
	rval &= ~mask;
	rval |= val;
	adxcfg_write(xcvr, lane, reg, rval);
}

/**
 * @brief adxcfg_acquire_arbitration
 */
void adxcfg_acquire_arbitration(struct adxcvr *xcvr,
				uint32_t lane)
{
	adxcvr_acquire_arbitration(xcvr, xcvr->adxcfg_base[lane],
				   XCVR_REG_CAPAB_PMA);
}

/**
 * @brief adxcfg_release_arbitration
 */
void adxcfg_release_arbitration(struct adxcvr *xcvr,
				uint32_t lane, bool calibrate)
{
	adxcvr_release_arbitration(xcvr->adxcfg_base[lane], calibrate);
}

/**
 * @brief atx_pll_calibration_check
 */
int32_t atx_pll_calibration_check(struct adxcvr *xcvr)
{
	uint32_t timeout = 0;
	uint32_t val;

	/* Wait max 100ms for cal_busy to de-assert */
	do {
		no_os_mdelay(10);

		/* Read ATX PLL calibration status from capability register */
		atx_pll_read(xcvr, XCVR_REG_CAPAB_ATX_PLL_STAT, &val);
		if ((val & XCVR_CAPAB_ATX_PLL_CAL_BSY_MASK) ==
		    XCVR_CAPAB_ATX_PLL_CAL_DONE) {
			printf("%s: ATX PLL calibration OK (%"PRIu32" ms)\n", xcvr->name,
			       timeout * 10);
			return 0;
		}
	} while (timeout++ < 10);

	printf("%s: ATX PLL calibration FAILED\n", xcvr->name);

	return 1;
}

/**
 * @brief adxcfg_calibration_check
 */
int32_t adxcfg_calibration_check(struct adxcvr *xcvr, uint32_t lane,
				 bool tx)
{
	uint32_t timeout = 0;
	uint32_t mask;
	uint32_t val;
	const char *msg;

	if (tx) {
		mask = XCVR_CAPAB_TX_CAL_BUSY_MASK;
		msg = "TX termination and VOD calibration";
	} else {
		mask = XCVR_CAPAB_RX_CAL_BUSY_MASK;
		msg = "CDR/CMU PLL & RX offset calibration";
	}

	/* Wait max 100ms for cal_busy to de-assert */
	do {
		no_os_udelay(100);

		/* Read PMA calibration status from capability register */
		adxcfg_read(xcvr, lane, XCVR_REG_CAPAB_PMA, &val);
		if ((val & mask) == 0) {
			printf("%s: Lane %"PRIu32" %s OK (%"PRIu32" us)\n", xcvr->name,
			       lane, msg, timeout * 100);
			return 0;
		}
	} while (timeout++ < 1000);

	printf("%s: Lane %"PRIu32" %s FAILED\n", xcvr->name, lane, msg);

	return 1;
}

/**
 * @brief xcvr_calib_tx
 */
int32_t xcvr_calib_tx(struct adxcvr *xcvr)
{
	uint32_t lane;
	uint32_t err = 0;

	for (lane = 0; lane < xcvr->lanes_per_link; lane++) {
		adxcfg_acquire_arbitration(xcvr, lane);

		/* Perform TX termination & Vod calibration through
		   PMA calibration enable register */
		adxcfg_update(xcvr, lane, XCVR_REG_CALIB_PMA_EN,
			      XCVR_CALIB_TX_TERM_VOD_MASK,
			      XCVR_CALIB_TX_TERM_VOD_EN);

		/* Disable rx_cal_busy and enable tx_cal_busy output through
		   capability register */
		adxcfg_update(xcvr, lane, XCVR_REG_CAPAB_PMA,
			      XCVR_CAPAB_RX_CAL_BUSY_EN_MASK |
			      XCVR_CAPAB_TX_CAL_BUSY_EN_MASK,
			      XCVR_CAPAB_RX_CAL_BUSY_DIS |
			      XCVR_CAPAB_TX_CAL_BUSY_EN);

		adxcfg_release_arbitration(xcvr, lane, true);

		err |= adxcfg_calibration_check(xcvr, lane, true);
	}

	return err;
}

/**
 * @brief adxcvr_pre_lane_rate_change
 */
void adxcvr_pre_lane_rate_change(struct adxcvr *xcvr)
{
	/*
	 * Multiple re-configuration requests can be active at the same time.
	 * Make sure that the reset stays asserted until all of them have
	 * completed.
	 */
	if (xcvr->reset_counter++ == 0)
		adxcvr_write(xcvr, ADXCVR_REG_RESETN, 0);
}

/**
 * @brief adxcvr_finalize_lane_rate_change
 */
void adxcvr_finalize_lane_rate_change(struct adxcvr *xcvr)
{
	uint32_t status;
	int32_t timeout = 1000;
	uint32_t i;

	if (--xcvr->reset_counter != 0)
		return;

	adxcvr_write(xcvr, ADXCVR_REG_RESETN, ADXCVR_RESETN);
	do {
		adxcvr_read(xcvr, ADXCVR_REG_STATUS, &status);
		if (status == ADXCVR_STATUS)
			break;
		no_os_mdelay(1);
	} while (timeout--);

	if (timeout < 0) {
		adxcvr_read(xcvr, ADXCVR_REG_STATUS2, &status);
		printf("%s: Link activation error:\n", xcvr->name);
		printf("\tLink PLL %slocked\n",
		       (status & ADXCVR_STATUS2_XCVR(xcvr->lanes_per_link)) ?
		       "" : "not ");
		for (i = 0; i < xcvr->lanes_per_link; i++) {
			printf("\tLane %"PRIu32" transceiver %sready\n", i,
			       (status & ADXCVR_STATUS2_XCVR(i)) ?
			       "" : "not ");
		}
	}
}

/**
 * @brief adxcvr_post_lane_rate_change
 */
void adxcvr_post_lane_rate_change(struct adxcvr *xcvr,
				  unsigned int lane_rate_khz)
{
	xcvr->lane_rate_khz = lane_rate_khz;

	adxcvr_finalize_lane_rate_change(xcvr);
}

/**
 * @brief adxcvr_round_rate
 */
int32_t adxcvr_round_rate(struct adxcvr *xcvr, uint32_t rate_khz)
{
	if (xcvr->is_transmit)
		return altera_a10_atx_pll_round_rate(xcvr, rate_khz);
	else
		return altera_a10_cdr_pll_round_rate(xcvr, rate_khz);
}

/**
 * @brief adxcvr_init
 */
int32_t adxcvr_set_rate(struct adxcvr *xcvr,
			uint32_t rate_khz)
{
	if (xcvr->is_transmit)
		return altera_a10_atx_pll_set_rate(xcvr, rate_khz);
	else
		return altera_a10_cdr_pll_set_rate(xcvr, rate_khz);
}

/**
 * @brief adxcvr_recalc_rate
 */
uint32_t adxcvr_recalc_rate(struct adxcvr *xcvr)
{
	if (xcvr->is_transmit)
		return altera_a10_atx_pll_recalc_rate(xcvr);
	else
		return altera_a10_cdr_pll_recalc_rate(xcvr);
}

/**
 * @brief adxcvr_init
 */
int32_t adxcvr_init(struct adxcvr **ad_xcvr,
		    const struct adxcvr_init *init)
{
	struct adxcvr *xcvr;
	uint32_t synth_conf;
	uint32_t i;

	xcvr = (struct adxcvr *)no_os_malloc(sizeof(*xcvr));
	if (!xcvr)
		return -1;

	xcvr->base = init->base;
	xcvr->name = init->name;
	xcvr->lane_rate_khz = init->lane_rate_khz;
	xcvr->parent_rate_khz = init->parent_rate_khz;

	xcvr->reset_counter = 0;

	adxcvr_read(xcvr, ADXCVR_REG_SYNTH_CONF, &synth_conf);

	xcvr->is_transmit = (bool)(synth_conf & 0x100);
	xcvr->lanes_per_link = synth_conf & 0xff;

	for (i = 0; i < xcvr->lanes_per_link; i++)
		if (init->adxcfg_base[i] != 0)
			xcvr->adxcfg_base[i] = init->adxcfg_base[i];
		else {
			printf("%s: Only up to %"PRIu32" lanes supported.\n",
			       xcvr->name, i);
			goto err;
		}

	if (xcvr->is_transmit)
		xcvr->atx_pll_base = init->atx_pll_base;

	xcvr->lane_rate_khz = adxcvr_round_rate(xcvr, xcvr->lane_rate_khz);
	adxcvr_set_rate(xcvr, xcvr->lane_rate_khz);

	*ad_xcvr = xcvr;

	return 0;

err:
	no_os_free(xcvr);

	return -1;
}

/**
 * @brief adxcvr_remove
 */
int32_t adxcvr_remove(struct adxcvr *xcvr)
{
	no_os_free(xcvr);

	return 0;
}

