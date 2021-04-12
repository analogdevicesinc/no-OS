/*******************************************************************************
 * @file xilinx_qpll.c
 * @brief part of xcvr dynamic reconfiguration driver..
 * @author andrei.grozav@analog.com)
 *******************************************************************************
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
 ******************************************************************************/


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "xilinx_qpll.h"

#ifdef XILINX

/*******************************************************************************
* @brief xilinx_xcvr_calc_qpll_config
*******************************************************************************/
int32_t xilinx_xcvr_calc_qpll_config(xcvr_core *core,
				     uint32_t refclk_khz,
				     uint32_t lane_rate_khz,
				     struct xilinx_xcvr_qpll_config *conf,
				     uint32_t *out_div)
{
	uint32_t n, d, m;
	uint32_t vco_freq;
	uint32_t band;
	uint32_t vco0_min;
	uint32_t vco0_max;
	uint32_t vco1_min;
	uint32_t vco1_max;
	const uint8_t *N_gt;

	static const uint8_t N_gtx2[] = {16, 20, 32, 40, 64, 66, 80, 100, 0};
	static const uint8_t N_gth34[] = {16, 20, 32, 40, 64, 66, 75, 80, 100,
					  112, 120, 125, 150, 160, 0
					 };

	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		N_gt = N_gtx2;
		vco0_min = 5930000;
		vco0_max = 8000000;
		vco1_min = 9800000;
		vco1_max = 12500000;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		N_gt = N_gth34;
		vco0_min = 9800000;
		vco0_max = 16375000;
		vco1_min = vco0_min;
		vco1_max = vco0_max;
		break;
	default:
		return -1;
	}

	for (m = 1; m <= 4; m++) {
		for (d = 1; d <= 16; d <<= 1) {
			for (n = 0; N_gt[n] != 0; n++) {
				vco_freq = refclk_khz * N_gt[n] / m;

				/*
				 * high band = 9.8G to 12.5GHz VCO
				 * low band = 5.93G to 8.0GHz VCO
				 */
				if (vco_freq >= vco1_min && vco_freq <= vco1_max)
					band = 1;
				else if (vco_freq >= vco0_min && vco_freq <= vco0_max)
					band = 0;
				else
					continue;

				if (refclk_khz / m / d == lane_rate_khz / N_gt[n]) {

					if (conf) {
						conf->refclk_div = m;
						conf->fb_div = N_gt[n];
						conf->band = band;
					}

					if (out_div)
						*out_div = d;

					return 0;
				}
			}
		}
	}

	printf("%s: Failed to find matching dividers for %u kHz rate\n",
	       __func__, lane_rate_khz);

	return -1;
}

int32_t xilinx_xcvr_gth34_qpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t qpll = 0;
	int32_t val;

	val = xilinx_xcvr_drp_read(core, drp_port, QPLL_REFCLK_DIV(qpll));
	if (val < 0)
		return val;

	switch ((val >> 7) & 0x1f) {
	case 16:
		conf->refclk_div = 1;
		break;
	case 0:
		conf->refclk_div = 2;
		break;
	case 1:
		conf->refclk_div = 3;
		break;
	case 2:
		conf->refclk_div = 4;
		break;
	default:
		conf->refclk_div = 5;
		break;
	}

	val = xilinx_xcvr_drp_read(core, drp_port, QPLL_FBDIV(qpll));
	if (val < 0)
		return val;

	conf->fb_div = (val & 0xff) + 2;

	conf->band = 0;

	return 0;
}


int32_t xilinx_xcvr_gtx2_qpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_qpll_config *conf)
{
	int32_t val;

	val = xilinx_xcvr_drp_read(core, drp_port, QPLL_CFG1_ADDR);
	if (val < 0)
		return val;

	switch ((val & QPLL_REFCLK_DIV_M_MASK) >> QPLL_REFCLK_DIV_M_OFFSET) {
	case 16:
		conf->refclk_div = 1;
		break;
	case 0:
		conf->refclk_div = 2;
		break;
	case 1:
		conf->refclk_div = 3;
		break;
	case 2:
		conf->refclk_div = 4;
		break;
	}

	val = xilinx_xcvr_drp_read(core, drp_port, QPLL_FBDIV_N_ADDR);
	if (val < 0)
		return val;

	switch (val & QPLL_FBDIV_N_MASK) {
	case 32:
		conf->fb_div = 16;
		break;
	case 48:
		conf->fb_div = 20;
		break;
	case 96:
		conf->fb_div = 32;
		break;
	case 128:
		conf->fb_div = 40;
		break;
	case 224:
		conf->fb_div = 64;
		break;
	case 320:
		conf->fb_div = 66;
		break;
	case 288:
		conf->fb_div = 80;
		break;
	case 368:
		conf->fb_div = 100;
		break;
	}

	val = xilinx_xcvr_drp_read(core, drp_port, QPLL_CFG0_ADDR);
	if (val < 0)
		return 0;

	if (val & QPLL_CFG0_LOWBAND_MASK)
		conf->band = 0;
	else
		conf->band = 1;

	return 0;
}

int32_t xilinx_xcvr_qpll_read_config(xcvr_core *core,
				     uint32_t drp_port,
				     struct xilinx_xcvr_qpll_config *conf)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_qpll_read_config(core, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth34_qpll_read_config(core, drp_port, conf);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_qpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_gtx2_qpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t cfg0, cfg1, fbdiv, fbdiv_ratio;
	int32_t ret;

	switch (conf->refclk_div) {
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
		printf("Invalid refclk divider: %u\n", conf->refclk_div);
		return -1;
	}

	fbdiv_ratio = QPLL_FBDIV_RATIO_MASK;

	switch (conf->fb_div) {
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
		printf("Invalid feedback divider: %u\n", conf->fb_div);
		return -1;
	}

	if (conf->band)
		cfg0 = 0;
	else
		cfg0 = QPLL_CFG0_LOWBAND_MASK;

	ret = xcvr_drp_update(core, drp_port, QPLL_CFG0_ADDR,
			      QPLL_CFG0_LOWBAND_MASK, cfg0);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(core, drp_port, QPLL_CFG1_ADDR,
			      QPLL_REFCLK_DIV_M_MASK, cfg1);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(core, drp_port, QPLL_FBDIV_N_ADDR,
			      QPLL_FBDIV_N_MASK, fbdiv);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(core, drp_port, QPLL_FBDIV_RATIO_ADDR,
			      QPLL_FBDIV_RATIO_MASK, fbdiv_ratio);
	if (ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
* @brief xcvr_gth34_qpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_gth34_qpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t refclk, fbdiv;
	int32_t ret;

	fbdiv = conf->fb_div - 2;

	switch (conf->refclk_div) {
	case 1:
		refclk = 16;
		break;
	case 2:
		refclk = 0;
		break;
	case 3:
		refclk = 1;
		break;
	case 4:
		refclk = 2;
		break;
	default:
		printf("Invalid refclk divider: %u\n", conf->refclk_div);
		return -1;
	}

	ret = xcvr_drp_update(core, drp_port, QPLL_FBDIV(0),
			      0xff, fbdiv);
	if (ret < 0)
		return ret;

	return xcvr_drp_update(core, drp_port, QPLL_REFCLK_DIV(0),
			       0xf80, refclk << 7);
}

/***************************************************************************//**
* @brief xcvr_qpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_qpll_write_config(xcvr_core *core,
				      uint32_t drp_port,
				      const struct xilinx_xcvr_qpll_config *conf)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_qpll_write_config(core, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth34_qpll_write_config(core, drp_port, conf);
	default:
		return -1;
	}
}

/*******************************************************************************
* @brief xilinx_xcvr_qpll_calc_lane_rate
*******************************************************************************/
uint32_t xilinx_xcvr_qpll_calc_lane_rate(xcvr_core *core,
		uint32_t refclk_hz,
		const struct xilinx_xcvr_qpll_config *conf,
		uint32_t out_div)
{
	if (conf->refclk_div == 0 || out_div == 0)
		return 0;

	return (refclk_hz * conf->fb_div) / (conf->refclk_div * out_div);
}

#endif

/******************************************************************************/
/******************************************************************************/
