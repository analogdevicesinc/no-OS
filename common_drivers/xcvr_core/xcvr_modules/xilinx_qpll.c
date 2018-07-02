/*******************************************************************************
 * @file xilinx_qpll.c
 * @brief part of xcvr dynamic reconfiguration driver..
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


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
 
#include "xilinx_qpll.h"

#ifdef XILINX

/*******************************************************************************
* @brief xilinx_xcvr_calc_qpll_config
*******************************************************************************/
int32_t xilinx_xcvr_calc_qpll_config(uint32_t refclk_khz,
		uint32_t lane_rate_khz,	xcvr_qpll *qpll_config)
{
	uint32_t n, d, m;
	uint32_t vco_freq;
	uint32_t band;

	qpll_config->refclk_div_M = 0;
	qpll_config->out_div = 0;
	qpll_config->fb_div = 0;
	qpll_config->band = 0;

	const uint8_t N[] = {16, 20, 32, 40, 64, 66, 80, 100};

	for (m = 1; m <= 4; m++) {
		for (d = 1; d <= 16; d <<= 1) {
			for (n = 0; n < ARRAY_SIZE(N); n++) {
				vco_freq = refclk_khz * N[n] / m;

				/*
				 * high band = 9.8G to 12.5GHz VCO
				 * low band = 5.93G to 8.0GHz VCO
				 */
				if (vco_freq >= 9800000 && vco_freq <= 12500000)
					band = 1;
				else if (vco_freq >= 5930000 && vco_freq <= 8000000)
					band = 0;
				else
					continue;

				if (vco_freq / d == lane_rate_khz) {

					qpll_config->refclk_div_M = m;
					qpll_config->fb_div = N[n];
					qpll_config->band = band;
					qpll_config->out_div = d;

#ifdef DEBUG
					printf("QPLL config:\n");
					printf("\trefclk_div_M = %lu\n", m);
					printf("\tfb_div = %d\n", N[n]);
					printf("\tband = %lu\n", band);
					printf("\tout_div = %lu\n", d);
#endif

					return 0;
				}
			}
		}
	}

	printf("%s: Faild to find config for lane_rate_khz=%lu, ref_clock_khz=%lu\n",
		__func__, lane_rate_khz, refclk_khz);

	return -1;
}

/*******************************************************************************
* @brief xilinx_xcvr_qpll_read_config
*******************************************************************************/
int32_t xilinx_xcvr_qpll_read_config(xcvr_core *core,
	xcvr_qpll *qpll_config)
{
	uint32_t val;
	int32_t ret;

	ret = xcvr_drp_read(core, XCVR_REG_CM_SEL, QPLL_CFG1_ADDR, &val);
	if (ret < 0)
		return ret;

	switch ((val & QPLL_REFCLK_DIV_M_MASK) >> QPLL_REFCLK_DIV_M_OFFSET) {
	case 16:
		qpll_config->refclk_div_M = 1;
		break;
	case 0:
		qpll_config->refclk_div_M = 2;
		break;
	case 1:
		qpll_config->refclk_div_M = 3;
		break;
	case 2:
		qpll_config->refclk_div_M = 4;
		break;
	}

	ret = xcvr_drp_read(core, XCVR_REG_CM_SEL, QPLL_FBDIV_N_ADDR, &val);
	if (ret < 0)
		return ret;

	switch (val & QPLL_FBDIV_N_MASK) {
	case 32:
		qpll_config->fb_div = 16;
		break;
	case 48:
		qpll_config->fb_div = 20;
		break;
	case 96:
		qpll_config->fb_div = 32;
		break;
	case 128:
		qpll_config->fb_div = 40;
		break;
	case 224:
		qpll_config->fb_div = 64;
		break;
	case 320:
		qpll_config->fb_div = 66;
		break;
	case 288:
		qpll_config->fb_div = 80;
		break;
	case 368:
		qpll_config->fb_div = 100;
		break;
	}

	ret = xcvr_drp_read(core, XCVR_REG_CM_SEL, QPLL_CFG0_ADDR, &val);
	if (ret < 0)
		return 0;

	if (val & QPLL_CFG0_LOWBAND_MASK)
		qpll_config->band = 0;
	else
		qpll_config->band = 1;

	return 0;
}

/*******************************************************************************
* @brief xilinx_xcvr_qpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_qpll_write_config(xcvr_core *core,
	xcvr_qpll *qpll_config)
{
	uint32_t cfg0, cfg1, fbdiv, fbdiv_ratio;
	int32_t ret;

	switch (qpll_config->refclk_div_M) {
	case 1:
		cfg1 = QPLL_REFCLK_DIV_M(16);
		break;
	case 2:
		cfg1 = QPLL_REFCLK_DIV_M(0);
		break;
	case 3:
		cfg1 = QPLL_REFCLK_DIV_M(1);
		break;
	case 4:
		cfg1 = QPLL_REFCLK_DIV_M(2);
		break;
	default:
		printf("Invalid refclk divider: %lu\n",
			qpll_config->refclk_div_M);
		return -1;
	}

	fbdiv_ratio = QPLL_FBDIV_RATIO_MASK;

	switch (qpll_config->fb_div) {
	case 16:
		fbdiv = 32;
		break;
	case 20:
		fbdiv = 48;
		break;
	case 32:
		fbdiv = 96;
		break;
	case 40:
		fbdiv = 128;
		break;
	case 64:
		fbdiv = 224;
		break;
	case 66:
		fbdiv = 320;
		fbdiv_ratio = 0;
		break;
	case 80:
		fbdiv = 288;
		break;
	case 100:
		fbdiv = 368;
		break;
	default:
		printf("Invalid feedback divider: %lu\n",
			qpll_config->fb_div);
		return -1;
	}

	if (qpll_config->band)
		cfg0 = 0;
	else
		cfg0 = QPLL_CFG0_LOWBAND_MASK;

	ret = xcvr_drp_update(core, XCVR_REG_CM_SEL, QPLL_CFG0_ADDR,
		QPLL_CFG0_LOWBAND_MASK, cfg0);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(core, XCVR_REG_CM_SEL, QPLL_CFG1_ADDR,
		QPLL_REFCLK_DIV_M_MASK, cfg1);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(core, XCVR_REG_CM_SEL, QPLL_FBDIV_N_ADDR,
		QPLL_FBDIV_N_MASK, fbdiv);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(core, XCVR_REG_CM_SEL, QPLL_FBDIV_RATIO_ADDR,
		QPLL_FBDIV_RATIO_MASK, fbdiv_ratio);
	if (ret < 0)
		return ret;

	return 0;
}

/*******************************************************************************
* @brief xilinx_xcvr_qpll_calc_lane_rate
*******************************************************************************/
uint32_t xilinx_xcvr_qpll_calc_lane_rate(uint32_t ref_clk_khz,
		xcvr_qpll *qpll_config)
{
	if (qpll_config->refclk_div_M == 0 || qpll_config->out_div == 0)
		return 0;

	return (ref_clk_khz * qpll_config->fb_div) / (qpll_config->refclk_div_M * qpll_config->out_div);
}

#endif

/*******************************************************************************/
/*******************************************************************************/
