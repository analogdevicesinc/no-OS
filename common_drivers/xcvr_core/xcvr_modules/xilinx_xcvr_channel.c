/*******************************************************************************
 * @file xilinx_xcvr_channel.c
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

#include "xilinx_xcvr_channel.h"

#ifdef XILINX

/*******************************************************************************
* @brief xilinx_xcvr_calc_cpll_config
*******************************************************************************/
int32_t xilinx_xcvr_calc_cpll_config(xcvr_core *core,
				     uint32_t refclk_khz,
				     uint32_t lane_rate_khz,
				     struct xilinx_xcvr_cpll_config *conf,
				     uint32_t *out_div)
{
	uint32_t n1, n2, d, m;
	uint32_t vco_freq;
	uint32_t vco_min;
	uint32_t vco_max;

	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		vco_min = 1600000;
		vco_max = 3300000;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		vco_min = 2000000;
		vco_max = 6250000;
		break;
	default:
		return -1;
	}


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

						return 0;
					}
				}
			}
		}
	}

	printf("%s: Failed to find matching dividers for %u kHz rate\n",
	       __func__, lane_rate_khz);

	return -1;
}

/***************************************************************************//**
* @brief xilinx_xcvr_gth34_cpll_read_config
*******************************************************************************/
int32_t xilinx_xcvr_gth34_cpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_cpll_config *conf)
{
	int val;

	val = xilinx_xcvr_drp_read(core, drp_port, 0x28);
	if (val < 0)
		return val;

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

	val = xilinx_xcvr_drp_read(core, drp_port, 0x2a);
	if (val < 0)
		return val;

	if (val & 0xf800)
		conf->refclk_div = 1;
	else
		conf->refclk_div = 2;

	return 0;
}

/***************************************************************************//**
* @brief xilinx_xcvr_gtx2_cpll_read_config
*******************************************************************************/
int32_t xilinx_xcvr_gtx2_cpll_read_config(xcvr_core *core,
		uint32_t drp_port,
		struct xilinx_xcvr_cpll_config *conf)
{
	int val;

	val = xilinx_xcvr_drp_read(core, drp_port, CPLL_REFCLK_DIV_M_ADDR);
	if (val < 0)
		return val;

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

	return 0;
}

/***************************************************************************//**
* @brief xilinx_xcvr_cpll_read_config
*******************************************************************************/
int32_t xilinx_xcvr_cpll_read_config(xcvr_core *core,
				     uint32_t drp_port,
				     struct xilinx_xcvr_cpll_config *conf)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_cpll_read_config(core, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth34_cpll_read_config(core, drp_port, conf);
	default:
		return -1;
	}

	return -1;
}

/***************************************************************************//**
* @brief xcvr_gtx2_cpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_gtx2_cpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val = 0;

	switch (conf->fb_div_N1) {
	case 4:
		break;
	case 5:
		val |= CPLL_FB_DIV_45_N1_MASK;
		break;
	default:
		return -1;
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
		return -1;
	}

	switch (conf->refclk_div) {
	case 1:
		val |= CPLL_REFCLK_DIV_M_MASK;
		break;
	case 2:
		break;
	default:
		return -1;
	}

	return xcvr_drp_update(core, drp_port, CPLL_REFCLK_DIV_M_ADDR,
			       CPLL_REFCLK_DIV_M_MASK |
			       CPLL_FB_DIV_45_N1_MASK |
			       CPLL_FBDIV_N2_MASK,
			       val);
}

/***************************************************************************//**
* @brief xcvr_gth34_cpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_gth34_cpll_write_config(xcvr_core *core,
		uint32_t drp_port,
		const struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val;
	int32_t ret;

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
		return -1;
	}

	val <<= 8;

	switch (conf->fb_div_N1) {
	case 4:
		break;
	case 5:
		val |= CPLL_FB_DIV_45_N1_MASK;
		break;
	default:
		return -1;
	}

	ret = xcvr_drp_update(core, drp_port, 0x28,
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
		return -1;
	}

	val <<= 11;

	return xcvr_drp_update(core, drp_port, 0x2a,
			       0xf800, val);
}

/***************************************************************************//**
* @brief xcvr_cpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_cpll_write_config(xcvr_core *core,
				      uint32_t drp_port,
				      const struct xilinx_xcvr_cpll_config *conf)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_cpll_write_config(core, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth34_cpll_write_config(core, drp_port, conf);
	default:
		return -1;
	}
}

/*******************************************************************************
* @brief xilinx_xcvr_cpll_calc_lane_rate
*******************************************************************************/
uint32_t xilinx_xcvr_cpll_calc_lane_rate(xcvr_core *core,
		uint32_t refclk_hz,
		const struct xilinx_xcvr_cpll_config *conf,
		uint32_t out_div)
{
	if (conf->refclk_div == 0 || out_div == 0)
		return 0;

	return (refclk_hz * conf->fb_div_N1 * conf->fb_div_N2 * 2) /
	       (conf->refclk_div * out_div);
}

/***************************************************************************//**
* @brief xcvr_configure_cdr
*******************************************************************************/
int32_t xilinx_xcvr_gtx2_configure_cdr(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t lane_rate,
				       uint32_t out_div,
				       uint8_t lpm_enable)
{
	uint32_t cfg0, cfg1, cfg2, cfg3, cfg4;

	cfg0 = 0x0020;
	cfg2 = 0x23FF;

	switch (core->ppm) {
	case PM_200:
		cfg3 = 0x0000;
		break;
	case PM_700:
	case PM_1250:
		cfg3 = 0x8000;
		break;
	default:
		return -1;
	}

	if (lane_rate > 6600000 && out_div == 1)
		cfg4 = 0x0B;
	else
		cfg4 = 0x03;

	if (core->encoding == ENC_8B10B) {

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
			return -1;
		}

	} else {

		switch (out_div) {
		case 1:
			if (lpm_enable) {
				if (lane_rate  > 6600000) {
					if (core->ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					cfg1 = 0x1020;
				}
			} else { /* DFE */
				if (lane_rate  > 6600000) {
					if (core->ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					if (core->ppm == PM_1250)
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
			return -1;
		}
	}

	xilinx_xcvr_drp_write(core, drp_port, RXCDR_CFG0_ADDR, cfg0);
	xilinx_xcvr_drp_write(core, drp_port, RXCDR_CFG1_ADDR, cfg1);
	xilinx_xcvr_drp_write(core, drp_port, RXCDR_CFG2_ADDR, cfg2);
	xilinx_xcvr_drp_write(core, drp_port, RXCDR_CFG3_ADDR, cfg3);
	xcvr_drp_update(core, drp_port, RXCDR_CFG4_ADDR, RXCDR_CFG4_MASK, cfg4);

	return 0;
}

/***************************************************************************//**
* @brief xcvr_gth3_configure_cdr
*******************************************************************************/
int32_t xilinx_xcvr_gth3_configure_cdr(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t out_div)
{
	/*
	 * TODO: UltraScale FPGAs Transceivers Wizard should be used for
	 *	 generating these settings
	 */

	return 0;
}

/***************************************************************************//**
* @brief xcvr_configure_cdr
*******************************************************************************/
int32_t xilinx_xcvr_configure_cdr(xcvr_core *core,
				  uint32_t drp_port,
				  uint32_t lane_rate,
				  uint32_t out_div,
				  uint8_t lpm_enable)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_configure_cdr(core, drp_port, lane_rate,
						      out_div, lpm_enable);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth3_configure_cdr(core, drp_port, out_div);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_configure_lpm_dfe_mode
*******************************************************************************/
int32_t xilinx_xcvr_configure_lpm_dfe_mode(xcvr_core *core,
		uint32_t drp_port,
		uint8_t lpm)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		/*
		 * TODO: UltraScale FPGAs Transceivers Wizard should be used for
		 *	 generating these settings
		 */
		break;
	case XILINX_XCVR_TYPE_S7_GTX2:
		if (lpm)
			xilinx_xcvr_drp_write(core, drp_port, 0x029, 0x0104);
		else
			xilinx_xcvr_drp_write(core, drp_port, 0x029, 0x0954);
		break;
	}

	return 0;
}

/***************************************************************************//**
* @brief xcvr_out_div_to_val
*******************************************************************************/
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

/***************************************************************************//**
* @brief xilinx_xcvr_gth34_read_out_div
*******************************************************************************/
int32_t xilinx_xcvr_gth34_read_out_div(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t *rx_out_div,
				       uint32_t *tx_out_div)
{
	int ret;

	if (rx_out_div) {
		ret = xilinx_xcvr_drp_read(core, drp_port, 0x63);
		if (ret < 0)
			return ret;

		*rx_out_div = 1 << (ret & 7);
	}

	if (tx_out_div) {
		ret = xilinx_xcvr_drp_read(core, drp_port, 0x7c);
		if (ret < 0)
			return ret;

		*tx_out_div = 1 << ((ret >> 8) & 7);
	}

	return 0;
}

/***************************************************************************//**
* @brief xilinx_xcvr_gtx2_read_out_div
*******************************************************************************/
int32_t xilinx_xcvr_gtx2_read_out_div(xcvr_core *core,
				      uint32_t drp_port,
				      uint32_t *rx_out_div,
				      uint32_t *tx_out_div)
{
	int ret;

	ret = xilinx_xcvr_drp_read(core, drp_port, OUT_DIV_ADDR);
	if (ret < 0)
		return ret;

	if (rx_out_div)
		*rx_out_div = 1 << ((ret >> OUT_DIV_RX_OFFSET) & 7);
	if (tx_out_div)
		*tx_out_div = 1 << ((ret >> OUT_DIV_TX_OFFSET) & 7);

	return 0;
}

/***************************************************************************//**
* @brief xilinx_xcvr_read_out_div
*******************************************************************************/
int32_t xilinx_xcvr_read_out_div(xcvr_core *core,
				 uint32_t drp_port,
				 uint32_t *rx_out_div,
				 uint32_t *tx_out_div)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_read_out_div(core, drp_port,
						     rx_out_div, tx_out_div);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth34_read_out_div(core, drp_port,
						      rx_out_div, tx_out_div);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_gtx2_write_out_div
*******************************************************************************/
int32_t xilinx_xcvr_gtx2_write_out_div(xcvr_core *core,
				       uint32_t drp_port,
				       int32_t rx_out_div,
				       int32_t tx_out_div)
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

	return xcvr_drp_update(core, drp_port, OUT_DIV_ADDR, mask, val);
}

/***************************************************************************//**
* @brief xcvr_gth34_write_out_div
*******************************************************************************/
int32_t xilinx_xcvr_gth34_write_out_div(xcvr_core *core,
					uint32_t drp_port,
					int32_t rx_out_div,
					int32_t tx_out_div)
{
	int32_t ret;

	if (rx_out_div >= 0) {
		ret = xcvr_drp_update(core, drp_port, 0x63, 0x7,
				      xilinx_xcvr_out_div_to_val(rx_out_div));
		if (ret)
			return ret;
	}
	if (tx_out_div >= 0) {
		ret = xcvr_drp_update(core, drp_port, 0x7c, 0x700,
				      xilinx_xcvr_out_div_to_val(tx_out_div) << 8);
		if (ret)
			return ret;
	}

	return 0;
}

/***************************************************************************//**
* @brief xcvr_write_out_div
*******************************************************************************/
int32_t xilinx_xcvr_write_out_div(xcvr_core *core,
				  uint32_t drp_port,
				  int32_t rx_out_div,
				  int32_t tx_out_div)
{
	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_write_out_div(core, drp_port,
						      rx_out_div, tx_out_div);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth34_write_out_div(core, drp_port,
						       rx_out_div, tx_out_div);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_write_rx_clk25_div
*******************************************************************************/
int32_t xilinx_xcvr_write_rx_clk25_div(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return -1;

	div--;

	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		div <<= RX_CLK25_DIV_OFFSET;
		mask = RX_CLK25_DIV_MASK;
		reg = RX_CLK25_DIV;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		div <<= 3;
		mask = 0xf8;
		reg = 0x6d;
		break;
	default:
		return -1;
	}

	return xcvr_drp_update(core, drp_port, reg, mask, div);
}

/***************************************************************************//**
* @brief xcvr_write_tx_clk25_div
*******************************************************************************/
int32_t xilinx_xcvr_write_tx_clk25_div(xcvr_core *core,
				       uint32_t drp_port,
				       uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return -1;

	div--;

	switch (core->dev.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		mask = TX_CLK25_DIV_MASK;
		reg = TX_CLK25_DIV;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		div <<= 11;
		mask = 0xf800;
		reg = 0x7a;
		break;
	default:
		return -1;
	}

	return xcvr_drp_update(core, drp_port, reg, mask, div);
}

#endif

/*******************************************************************************
*******************************************************************************/
