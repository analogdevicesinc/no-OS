/***************************************************************************//**
 *   @file   xilinx_transceiver.c
 *   @brief  Driver for the Xilinx High-speed transceiver dynamic
 *           reconfiguration.
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
#include "util.h"
#include "error.h"
#include "axi_adxcvr.h"
#include "xilinx_transceiver.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define OUT_DIV_ADDR			0x88
#define OUT_DIV_TX_OFFSET		0x4
#define OUT_DIV_RX_OFFSET		0x0

#define RXCDR_CFG0_ADDR			0xa8
#define RXCDR_CFG0_MASK			0xffff

#define RXCDR_CFG1_ADDR			0xa9
#define RXCDR_CFG1_MASK			0xffff

#define RXCDR_CFG2_ADDR			0xaa
#define RXCDR_CFG2_MASK			0xffff

#define RXCDR_CFG3_ADDR			0xab
#define RXCDR_CFG3_MASK			0xffff

#define RXCDR_CFG4_ADDR			0xac
#define RXCDR_CFG4_MASK			0x00ff

#define RX_DFE_LPM_CFG_ADDR		0x29
#define RX_DFE_LPM_CFG_MASK		0xffff

#define QPLL_CFG0_ADDR				0x32
#define QPLL_CFG0_LOWBAND_MASK		0x0040

#define QPLL_CFG1_ADDR				0x33
#define QPLL_REFCLK_DIV_M_MASK		0xf800
#define QPLL_REFCLK_DIV_M_OFFSET	11
#define QPLL_REFCLK_DIV_M(x)		((x) << 11)

#define QPLL_FBDIV_N_ADDR		0x36
#define QPLL_FBDIV_N_MASK		0x03ff

#define QPLL_FBDIV_RATIO_ADDR	0x37
#define QPLL_FBDIV_RATIO_MASK	0x0040

#define CPLL_CFG0_ADDR			0x5c
#define CPLL_CFG0_MASK			0xff00

#define CPLL_CFG1_ADDR			0x5d
#define CPLL_CFG1_MASK			0xffff

#define CPLL_REFCLK_DIV_M_ADDR	0x5e
#define CPLL_REFCLK_DIV_M_MASK	0x1f00
#define CPLL_FB_DIV_45_N1_MASK	0x0080
#define CPLL_FBDIV_N2_MASK		0x007f

#define RX_CLK25_DIV			0x11
#define RX_CLK25_DIV_OFFSET		6
#define RX_CLK25_DIV_MASK		0x07c0

#define TX_CLK25_DIV			0x6a
#define TX_CLK25_DIV_MASK		0x1f

/**
 * @brief xilinx_xcvr_write
 */
int32_t xilinx_xcvr_write(struct xilinx_xcvr *xcvr, uint32_t drp_port,
			  uint32_t reg_addr, uint32_t reg_val)
{
	return adxcvr_drp_write(xcvr->ad_xcvr, drp_port, reg_addr, reg_val);
}

/**
 * @brief xilinx_xcvr_read
 */
int32_t xilinx_xcvr_read(struct xilinx_xcvr *xcvr, uint32_t drp_port,
			 uint32_t reg_addr, uint32_t *reg_val)
{
	return adxcvr_drp_read(xcvr->ad_xcvr, drp_port, reg_addr, reg_val);
}

/**
 * @brief xilinx_xcvr_drp_read
 */
int32_t xilinx_xcvr_drp_read(struct xilinx_xcvr *xcvr,
			     uint32_t drp_port, uint32_t reg, uint32_t *val)
{
	int32_t ret;

	ret = xilinx_xcvr_read(xcvr, drp_port, reg, val);

	if (ret < 0) {
		printf("%s: Failed to read reg %"PRIu32"-0x%"PRIX32": %"PRId32"\n",
		       __func__, drp_port, reg, ret);

		return FAILURE;
	}

	return ret;
}

/**
 * @brief xilinx_xcvr_drp_write
 */
int32_t xilinx_xcvr_drp_write(struct xilinx_xcvr *xcvr,
			      uint32_t drp_port, uint32_t reg, uint32_t val)
{
	uint32_t read_val;
	int32_t ret;

	ret = xilinx_xcvr_write(xcvr, drp_port, reg, val);
	if (ret < 0) {
		printf("%s: Failed to write reg %"PRIu32"-0x%"PRIX32": %"PRId32"\n",
		       __func__, drp_port, reg, ret);
		return ret;
	}

	xilinx_xcvr_drp_read(xcvr, drp_port, reg, &read_val);
	if (read_val != val)
		printf("%s: read-write mismatch: reg 0x%"PRIX32","
		       "val 0x%4"PRIX32", expected val 0x%4"PRIX32"\n",
		       __func__, reg, val, read_val);

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_drp_update
 */
int32_t xilinx_xcvr_drp_update(struct xilinx_xcvr *xcvr, uint32_t drp_port,
			       uint32_t reg, uint32_t mask, uint32_t val)
{
	uint32_t read_val;
	int32_t ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, reg, &read_val);
	if (ret < 0)
		return ret;

	val |= read_val & ~mask;

	return xilinx_xcvr_drp_write(xcvr, drp_port, reg, val);
}

/**
 * @brief xilinx_xcvr_gth3_configure_cdr
 */
int32_t xilinx_xcvr_gth3_configure_cdr(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, uint32_t out_div)
{
	/*
	 * TODO: UltraScale FPGAs Transceivers Wizard should be used for
	 *	 generating these settings
	 */

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_gtx2_configure_cdr
 */
int32_t xilinx_xcvr_gtx2_configure_cdr(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, uint32_t lane_rate, uint32_t out_div,
				       bool lpm_enable)
{
	uint32_t cfg0, cfg1, cfg2, cfg3, cfg4;

	cfg0 = 0x0020;
	cfg2 = 0x23FF;

	switch (xcvr->refclk_ppm) {
	case PM_200:
		cfg3 = 0x0000;
		break;
	case PM_700:
	case PM_1250:
		cfg3 = 0x8000;
		break;
	default:
		return FAILURE;
	}

	if (lane_rate > 6600000 && out_div == 1)
		cfg4 = 0x0B;
	else
		cfg4 = 0x03;

	if (xcvr->encoding == ENC_8B10B) {

		switch (out_div) {
		case 1:
			cfg1 = 0x1040;
			break;
		case 2:
			cfg1 = 0x1020;
			break;
		case 4:
			cfg1 = 0x1010;
			break;
		case 8:
			cfg1 = 0x1008;
			break;
		default:
			return FAILURE;
		}

	} else {

		switch (out_div) {
		case 1:
			if (lpm_enable) {
				if (lane_rate  > 6600000) {
					if (xcvr->refclk_ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					cfg1 = 0x1020;
				}
			} else { /* DFE */
				if (lane_rate  > 6600000) {
					if (xcvr->refclk_ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					if (xcvr->refclk_ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x2040;
				}
			}
			break;
		case 2:
			cfg1 = 0x4020;
			break;
		case 4:
			cfg1 = 0x4010;
			break;
		case 8:
			cfg1 = 0x4008;
			break;
		default:
			return FAILURE;
		}
	}

	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG0_ADDR, cfg0);
	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG1_ADDR, cfg1);
	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG2_ADDR, cfg2);
	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG3_ADDR, cfg3);
	xilinx_xcvr_drp_update(xcvr, drp_port, RXCDR_CFG4_ADDR,
			       RXCDR_CFG4_MASK, cfg4);

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_configure_cdr
 */
int32_t xilinx_xcvr_configure_cdr(struct xilinx_xcvr *xcvr,
				  uint32_t drp_port, uint32_t lane_rate, uint32_t out_div,
				  bool lpm_enable)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_configure_cdr(xcvr, drp_port, lane_rate,
						      out_div, lpm_enable);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth3_configure_cdr(xcvr, drp_port, out_div);
	default:
		return FAILURE;
	}
}

/**
 * @brief xilinx_xcvr_configure_lpm_dfe_mode
 */
int32_t xilinx_xcvr_configure_lpm_dfe_mode(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, bool lpm)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		/*
		 * TODO: UltraScale FPGAs Transceivers Wizard should be used for
		 *	 generating these settings
		 */
		break;
	case XILINX_XCVR_TYPE_S7_GTX2:
		if (lpm)
			xilinx_xcvr_drp_write(xcvr, drp_port, 0x029, 0x0104);
		else
			xilinx_xcvr_drp_write(xcvr, drp_port, 0x029, 0x0954);
		break;
	}

	return SUCCESS;
}

static void xilinx_xcvr_setup_cpll_vco_range(struct xilinx_xcvr *xcvr,
		uint32_t *vco_max)
{
	if  ((xcvr->type == XILINX_XCVR_TYPE_US_GTH3) |
	     (xcvr->type == XILINX_XCVR_TYPE_US_GTH4) |
	     (xcvr->type == XILINX_XCVR_TYPE_US_GTY4)) {
		if (xcvr->voltage < 850)
			*vco_max = 4250000;
		else if ((xcvr->speed_grade / 10) == 1)
			*vco_max = 4250000;
	}
}

static void xilinx_xcvr_setup_qpll_vco_range(struct xilinx_xcvr *xcvr,
		uint32_t *vco0_min,
		uint32_t *vco0_max,
		uint32_t *vco1_min,
		uint32_t *vco1_max)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		if ((xcvr->dev_package == AXI_FPGA_DEV_FB) |
		    (xcvr->dev_package == AXI_FPGA_DEV_SB))
			*vco0_max = 6600000;
		if ((xcvr->speed_grade / 10) == 2)
			*vco1_max = 10312500;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		*vco1_min = 8000000;
		*vco1_max = 13000000;
		if (((xcvr->voltage < 900) | (xcvr->voltage > 720)) &
		    ((xcvr->speed_grade / 10) == 1)) {
			*vco0_max = 12500000;
			*vco1_max = *vco0_max;
		}
		if (xcvr->voltage == 720) {
			if ((xcvr->speed_grade / 10) == 2)
				*vco0_max = 12500000;
			else if ((xcvr->speed_grade / 10) == 1)
				*vco0_max = 10312500;
			*vco1_max = *vco0_max;
		}
	}
}

/**
 * @brief xilinx_xcvr_calc_cpll_config
 */
int32_t xilinx_xcvr_calc_cpll_config(struct xilinx_xcvr *xcvr,
				     uint32_t refclk_khz, uint32_t lane_rate_khz,
				     struct xilinx_xcvr_cpll_config *conf, uint32_t *out_div)
{
	uint32_t n1, n2, d, m;
	uint32_t vco_freq;
	uint32_t vco_min;
	uint32_t vco_max;

	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		vco_min = 1600000;
		vco_max = 3300000;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		vco_min = 2000000;
		vco_max = 6250000;
		break;
	default:
		return FAILURE;
	}

	if (AXI_PCORE_VER_MAJOR(xcvr->version) > 0x10)
		xilinx_xcvr_setup_cpll_vco_range(xcvr, &vco_max);

	for (m = 1; m <= 2; m++) {
		for (d = 1; d <= 8; d <<= 1) {
			for (n1 = 5; n1 >= 4; n1--) {
				for (n2 = 5; n2 >= 1; n2--) {
					vco_freq = refclk_khz * n1 * n2 / m;

					if (vco_freq > vco_max || vco_freq < vco_min)
						continue;

					if (refclk_khz / m / d == lane_rate_khz / (2 * n1 * n2)) {
						if (conf) {
							conf->refclk_div = m;
							conf->fb_div_N1 = n1;
							conf->fb_div_N2 = n2;
						}

						if (out_div)
							*out_div = d;

						return SUCCESS;
					}
				}
			}
		}
	}

	return FAILURE;
}

/**
 * @brief xilinx_xcvr_calc_qpll_config
 */
int32_t xilinx_xcvr_calc_qpll_config(struct xilinx_xcvr *xcvr,
				     uint32_t refclk_khz, uint32_t lane_rate_khz,
				     struct xilinx_xcvr_qpll_config *conf, uint32_t *out_div)
{
	uint32_t n, d, m;
	uint32_t vco_freq;
	uint32_t band;
	uint32_t vco0_min;
	uint32_t vco0_max;
	uint32_t vco1_min;
	uint32_t vco1_max;
	const uint8_t *N;

	static const uint8_t N_gtx2[] = {16, 20, 32, 40, 64, 66, 80, 100, 0};
	static const uint8_t N_gth34[] = {16, 20, 32, 40, 64, 66, 75, 80, 100,
					  112, 120, 125, 150, 160, 0
					 };

	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		N = N_gtx2;
		vco0_min = 5930000;
		vco0_max = 8000000;
		vco1_min = 9800000;
		vco1_max = 12500000;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		N = N_gth34;
		vco0_min = 9800000;
		vco0_max = 16375000;
		vco1_min = vco0_min;
		vco1_max = vco0_max;
		break;
	default:
		return FAILURE;
	}

	if (AXI_PCORE_VER_MAJOR(xcvr->version) > 0x10)
		xilinx_xcvr_setup_qpll_vco_range(xcvr,
						 &vco0_min, &vco0_max,
						 &vco1_min, &vco1_max);

	for (m = 1; m <= 4; m++) {
		for (d = 1; d <= 16; d <<= 1) {
			for (n = 0; N[n] != 0; n++) {
				vco_freq = refclk_khz * N[n] / m;

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

				if (refclk_khz / m / d == lane_rate_khz / N[n]) {

					if (conf) {
						conf->refclk_div = m;
						conf->fb_div = N[n];
						conf->band = band;
					}

					if (out_div)
						*out_div = d;

					return SUCCESS;
				}
			}
		}
	}

	return FAILURE;
}

/**
 * @brief xilinx_xcvr_gth34_cpll_read_config
 */
int32_t xilinx_xcvr_gth34_cpll_read_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, 0x28, &val);
	if (ret < 0)
		return ret;

	if (val & CPLL_FB_DIV_45_N1_MASK)
		conf->fb_div_N1 = 5;
	else
		conf->fb_div_N1 = 4;

	switch ((val >> 8) & 0xff) {
	case 3:
		conf->fb_div_N2 = 5;
		break;
	case 2:
		conf->fb_div_N2 = 4;
		break;
	case 1:
		conf->fb_div_N2 = 3;
		break;
	case 0:
		conf->fb_div_N2 = 2;
		break;
	default:
		conf->fb_div_N2 = 1;
		break;
	}

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, 0x2a, &val);
	if (ret < 0)
		return ret;

	if (val & 0xf800)
		conf->refclk_div = 1;
	else
		conf->refclk_div = 2;

	printf("%s: cpll: fb_div_N1=%"PRIu32"\ncpll: fb_div_N2=%"PRIu32"\ncpll:"
	       " refclk_div=%"PRIu32"\n", __func__, conf->fb_div_N1,
	       conf->fb_div_N2, conf->refclk_div);

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_gtx2_cpll_read_config
 */
int32_t xilinx_xcvr_gtx2_cpll_read_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, CPLL_REFCLK_DIV_M_ADDR, &val);
	if (ret < 0)
		return ret;

	if (val & CPLL_FB_DIV_45_N1_MASK)
		conf->fb_div_N1 = 5;
	else
		conf->fb_div_N1 = 4;

	switch (val & CPLL_FBDIV_N2_MASK) {
	case 3:
		conf->fb_div_N2 = 5;
		break;
	case 2:
		conf->fb_div_N2 = 4;
		break;
	case 1:
		conf->fb_div_N2 = 3;
		break;
	case 0:
		conf->fb_div_N2 = 2;
		break;
	default:
		conf->fb_div_N2 = 1;
		break;
	}

	if (val & CPLL_REFCLK_DIV_M_MASK)
		conf->refclk_div = 1;
	else
		conf->refclk_div = 2;

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_cpll_read_config
 */
int32_t xilinx_xcvr_cpll_read_config(struct xilinx_xcvr *xcvr,
				     uint32_t drp_port, struct xilinx_xcvr_cpll_config *conf)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_cpll_read_config(xcvr, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_cpll_read_config(xcvr, drp_port, conf);
	default:
		return FAILURE;
	}
}

/**
 * @brief xilinx_xcvr_gth34_cpll_write_config
 */
int32_t xilinx_xcvr_gth34_cpll_write_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, const struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val;
	int ret;

	switch (conf->fb_div_N2) {
	case 1:
		val = 0x10;
		break;
	case 2:
		val = 0x00;
		break;
	case 3:
		val = 0x01;
		break;
	case 4:
		val = 0x2;
		break;
	case 5:
		val = 0x3;
		break;
	default:
		return FAILURE;
	}

	val <<= 8;

	switch (conf->fb_div_N1) {
	case 4:
		break;
	case 5:
		val |= CPLL_FB_DIV_45_N1_MASK;
		break;
	default:
		return FAILURE;
	}

	ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0x28,
				     0xff80, val);
	if (ret)
		return ret;

	switch (conf->refclk_div) {
	case 1:
		val = 16;
		break;
	case 2:
		val = 0;
		break;
	default:
		return FAILURE;
	}

	val <<= 11;

	return xilinx_xcvr_drp_update(xcvr, drp_port, 0x2a,
				      0xf800, val);
}

/**
 * @brief xilinx_xcvr_gtx2_cpll_write_config
 */
int32_t xilinx_xcvr_gtx2_cpll_write_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, const struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val = 0;

	switch (conf->fb_div_N1) {
	case 4:
		break;
	case 5:
		val |= CPLL_FB_DIV_45_N1_MASK;
		break;
	default:
		return FAILURE;
	}

	switch (conf->fb_div_N2) {
	case 1:
		val |= 0x10;
	case 2:
		val |= 0x00;
		break;
	case 3:
		val |= 0x01;
		break;
	case 4:
		val |= 0x2;
		break;
	case 5:
		val |= 0x3;
		break;
	default:
		return FAILURE;
	}

	switch (conf->refclk_div) {
	case 1:
		val |= CPLL_REFCLK_DIV_M_MASK;
		break;
	case 2:
		break;
	default:
		return FAILURE;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port, CPLL_REFCLK_DIV_M_ADDR,
				      CPLL_REFCLK_DIV_M_MASK | CPLL_FB_DIV_45_N1_MASK |
				      CPLL_FBDIV_N2_MASK, val);
}

/**
 * @brief xilinx_xcvr_cpll_write_config
 */
int32_t xilinx_xcvr_cpll_write_config(struct xilinx_xcvr *xcvr,
				      uint32_t drp_port, const struct xilinx_xcvr_cpll_config *conf)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_cpll_write_config(xcvr, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_cpll_write_config(xcvr, drp_port, conf);
	default:
		return FAILURE;
	}
}

/**
 * @brief xilinx_xcvr_cpll_calc_lane_rate
 */
int32_t xilinx_xcvr_cpll_calc_lane_rate(struct xilinx_xcvr *xcvr,
					uint32_t refclk_khz, const struct xilinx_xcvr_cpll_config *conf,
					uint32_t out_div)
{
	if (conf->refclk_div == 0 || out_div == 0)
		return SUCCESS;

	return DIV_ROUND_CLOSEST_ULL((uint64_t)refclk_khz *
				     conf->fb_div_N1 * conf->fb_div_N2 * 2,
				     conf->refclk_div * out_div * 1000);
}

/**
 * @brief xilinx_xcvr_gth34_qpll_read_config
 */
int32_t xilinx_xcvr_gth34_qpll_read_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t qpll = 0;
	uint32_t val;
	int ret;

#define QPLL0_FBDIV_DIV 0x14
#define QPLL0_REFCLK_DIV 0x18
#define QPLL1_FBDIV 0x94
#define QPLL1_REFCLK_DIV 0x98

#define QPLL_FBDIV(x) (0x14 + (x) * 0x80)
#define QPLL_REFCLK_DIV(x) (0x18 + (x) * 0x80)

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, QPLL_REFCLK_DIV(qpll), &val);
	if (ret < 0)
		return ret;

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

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, QPLL_FBDIV(qpll), &val);
	if (ret < 0)
		return ret;

	conf->fb_div = (val & 0xff) + 2;

	conf->band = 0;

	printf("%s: qpll: fb_div=%"PRIu32", qpll: refclk_div=%"PRIu32"\n",
	       __func__, conf->fb_div, conf->refclk_div);

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_gtx2_qpll_read_config
 */
int32_t xilinx_xcvr_gtx2_qpll_read_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t val;
	int32_t ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, QPLL_CFG1_ADDR, &val);
	if (ret < 0)
		return ret;

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

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, QPLL_FBDIV_N_ADDR, &val);
	if (ret < 0)
		return ret;

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

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, QPLL_CFG0_ADDR, &val);
	if (ret < 0)
		return ret;

	if (val & QPLL_CFG0_LOWBAND_MASK)
		conf->band = 0;
	else
		conf->band = 1;

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_qpll_read_config
 */
int32_t xilinx_xcvr_qpll_read_config(struct xilinx_xcvr *xcvr,
				     uint32_t drp_port, struct xilinx_xcvr_qpll_config *conf)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_qpll_read_config(xcvr, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_qpll_read_config(xcvr, drp_port, conf);
	default:
		return FAILURE;
	}
}

/**
 * @brief xilinx_xcvr_gth34_qpll_write_config
 */
int32_t xilinx_xcvr_gth34_qpll_write_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, const struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t refclk, fbdiv;
	int ret;

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
		printf("%s: Invalid refclk divider: %"PRIu32"\n",
		       __func__, conf->refclk_div);
		return FAILURE;
	}

	ret = xilinx_xcvr_drp_update(xcvr, drp_port, QPLL_FBDIV(0),
				     0xff, fbdiv);
	if (ret < 0)
		return ret;

	return xilinx_xcvr_drp_update(xcvr, drp_port, QPLL_REFCLK_DIV(0),
				      0xf80, refclk << 7);
}

/**
 * @brief xilinx_xcvr_gtx2_qpll_write_config
 */
int32_t xilinx_xcvr_gtx2_qpll_write_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, const struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t cfg0, cfg1, fbdiv, fbdiv_ratio;
	int ret;

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
		printf("%s: Invalid refclk divider: %"PRIu32"\n",
		       __func__, conf->refclk_div);
		return FAILURE;
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
		printf("%s: Invalid feedback divider: %"PRIu32"\n",
		       __func__, conf->fb_div);
		return FAILURE;
	}

	if (conf->band)
		cfg0 = 0;
	else
		cfg0 = QPLL_CFG0_LOWBAND_MASK;

	ret = xilinx_xcvr_drp_update(xcvr, drp_port, QPLL_CFG0_ADDR,
				     QPLL_CFG0_LOWBAND_MASK, cfg0);
	if (ret < 0)
		return ret;

	ret = xilinx_xcvr_drp_update(xcvr, drp_port, QPLL_CFG1_ADDR,
				     QPLL_REFCLK_DIV_M_MASK, cfg1);
	if (ret < 0)
		return ret;

	ret = xilinx_xcvr_drp_update(xcvr, drp_port, QPLL_FBDIV_N_ADDR,
				     QPLL_FBDIV_N_MASK, fbdiv);
	if (ret < 0)
		return ret;

	ret = xilinx_xcvr_drp_update(xcvr, drp_port, QPLL_FBDIV_RATIO_ADDR,
				     QPLL_FBDIV_RATIO_MASK, fbdiv_ratio);
	if (ret < 0)
		return ret;

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_qpll_write_config
 */
int32_t xilinx_xcvr_qpll_write_config(struct xilinx_xcvr *xcvr,
				      uint32_t drp_port, const struct xilinx_xcvr_qpll_config *conf)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_qpll_write_config(xcvr, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_qpll_write_config(xcvr, drp_port, conf);
	default:
		return FAILURE;
	}
}

/**
 * @brief xilinx_xcvr_qpll_calc_lane_rate
 */
int32_t xilinx_xcvr_qpll_calc_lane_rate(struct xilinx_xcvr *xcvr,
					uint32_t refclk_khz, const struct xilinx_xcvr_qpll_config *conf,
					uint32_t out_div)
{
	if (conf->refclk_div == 0 || out_div == 0)
		return SUCCESS;

	return DIV_ROUND_CLOSEST_ULL((uint64_t)refclk_khz * conf->fb_div,
				     conf->refclk_div * out_div * 1000);
}

/**
 * @brief xilinx_xcvr_gth34_read_out_div
 */
int32_t xilinx_xcvr_gth34_read_out_div(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, uint32_t *rx_out_div, uint32_t *tx_out_div)
{
	uint32_t val;
	int ret;

	if (rx_out_div) {
		ret = xilinx_xcvr_drp_read(xcvr, drp_port, 0x63, &val);
		if (ret < 0)
			return ret;

		*rx_out_div = 1 << (val & 7);
	}

	if (tx_out_div) {
		ret = xilinx_xcvr_drp_read(xcvr, drp_port, 0x7c, &val);
		if (ret < 0)
			return ret;

		*tx_out_div = 1 << ((val >> 8) & 7);
	}

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_gtx2_read_out_div
 */
int32_t xilinx_xcvr_gtx2_read_out_div(struct xilinx_xcvr *xcvr,
				      uint32_t drp_port, uint32_t *rx_out_div, uint32_t *tx_out_div)
{
	uint32_t val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, OUT_DIV_ADDR, &val);
	if (ret < 0)
		return ret;

	if (rx_out_div)
		*rx_out_div = 1 << ((ret >> OUT_DIV_RX_OFFSET) & 7);
	if (tx_out_div)
		*tx_out_div = 1 << ((ret >> OUT_DIV_TX_OFFSET) & 7);

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_read_out_div
 */
int32_t xilinx_xcvr_read_out_div(struct xilinx_xcvr *xcvr, uint32_t drp_port,
				 uint32_t *rx_out_div, uint32_t *tx_out_div)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_read_out_div(xcvr, drp_port,
						     rx_out_div, tx_out_div);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_read_out_div(xcvr, drp_port,
						      rx_out_div, tx_out_div);
	default:
		return FAILURE;
	}
}

/**
 * @brief xilinx_xcvr_out_div_to_val
 */
uint32_t xilinx_xcvr_out_div_to_val(uint32_t out_div)
{
	switch (out_div) {
	case 1:
		return 0;
	case 2:
		return 1;
	case 4:
		return 2;
	case 8:
		return 3;
	default:
		return 4;
	}
}

/**
 * @brief xilinx_xcvr_gth34_write_out_div
 */
int32_t xilinx_xcvr_gth34_write_out_div(struct xilinx_xcvr *xcvr,
					uint32_t drp_port, int32_t rx_out_div, int32_t tx_out_div)
{
	int ret;

	if (rx_out_div >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0x63, 0x7,
					     xilinx_xcvr_out_div_to_val(rx_out_div));
		if (ret)
			return ret;
	}
	if (tx_out_div >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0x7c, 0x700,
					     xilinx_xcvr_out_div_to_val(tx_out_div) << 8);
		if (ret)
			return ret;
	}

	return SUCCESS;
}

/**
 * @brief xilinx_xcvr_gtx2_write_out_div
 */
int32_t xilinx_xcvr_gtx2_write_out_div(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, int32_t rx_out_div, int32_t tx_out_div)
{
	uint32_t val = 0;
	uint32_t mask = 0;

	if (tx_out_div >= 0) {
		val |= xilinx_xcvr_out_div_to_val(tx_out_div) << OUT_DIV_TX_OFFSET;
		mask |= 0x7 << OUT_DIV_TX_OFFSET;
	}
	if (rx_out_div >= 0) {
		val |= xilinx_xcvr_out_div_to_val(rx_out_div) << OUT_DIV_RX_OFFSET;
		mask |= 0x7 << OUT_DIV_RX_OFFSET;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port, OUT_DIV_ADDR, mask, val);
}

/**
 * @brief xilinx_xcvr_write_out_div
 */
int32_t xilinx_xcvr_write_out_div(struct xilinx_xcvr *xcvr, uint32_t drp_port,
				  int32_t rx_out_div, int32_t tx_out_div)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_write_out_div(xcvr, drp_port,
						      rx_out_div, tx_out_div);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_write_out_div(xcvr, drp_port,
						       rx_out_div, tx_out_div);
	default:
		return FAILURE;
	}
}

/**
 * @brief xilinx_xcvr_write_rx_clk25_div
 */
int32_t xilinx_xcvr_write_rx_clk25_div(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return FAILURE;

	div--;

	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		div <<= RX_CLK25_DIV_OFFSET;
		mask = RX_CLK25_DIV_MASK;
		reg = RX_CLK25_DIV;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		div <<= 3;
		mask = 0xf8;
		reg = 0x6d;
		break;
	default:
		return FAILURE;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port, reg, mask, div);
}

/**
 * @brief xilinx_xcvr_write_tx_clk25_div
 */
int32_t xilinx_xcvr_write_tx_clk25_div(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return FAILURE;

	div--;

	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		mask = TX_CLK25_DIV_MASK;
		reg = TX_CLK25_DIV;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		div <<= 11;
		mask = 0xf800;
		reg = 0x7a;
		break;
	default:
		return FAILURE;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port, reg, mask, div);
}
