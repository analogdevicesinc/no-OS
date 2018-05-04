/*******************************************************************************
 * @file xilinx_xcvr_channel.c
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

#include "xilinx_xcvr_channel.h"

#ifdef XILINX

/*******************************************************************************
* @brief xilinx_xcvr_calc_cpll_config
*******************************************************************************/
int32_t xilinx_xcvr_calc_cpll_config(uint32_t ref_clk_khz,
		uint32_t lane_rate_khz,	xcvr_cpll *cpll_config)
{
	uint32_t n1, n2, d, m;
	uint32_t vco_freq;

	cpll_config->refclk_div_M = 0;
	cpll_config->fb_div_N1 = 0;
	cpll_config->fb_div_N2 = 0;
	cpll_config->out_div = 0;

	for (m = 1; m <= 2; m++) {
		for (d = 1; d <= 8; d <<= 1) {
			for (n1 = 5; n1 >= 4; n1--) {
				for (n2 = 5; n2 >= 1; n2--) {
					vco_freq = ref_clk_khz * n1 * n2 / m;

					if (vco_freq > 3300000 || vco_freq < 1600000)
						continue;

					if (vco_freq * 2 / d == lane_rate_khz) {
						cpll_config->refclk_div_M = m;
						cpll_config->fb_div_N1 = n1;
						cpll_config->fb_div_N2 = n2;
						cpll_config->out_div = d;

#ifdef DEBUG
						printf("CPLL config:\n");
						printf("\trefclk_div_M = %lu\n", m);
						printf("\tfb_div_N1 = %lu\n", n1);
						printf("\tfb_div_N2 = %lu\n", n2);
						printf("\tout_div = %lu\n", d);
#endif

						return 0;
					}
				}
			}
		}
	}

	printf("%s: Faild to find config for lane_rate_khz=%lu, ref_clock_khz=%lu\n",
		__func__, lane_rate_khz, ref_clk_khz);
	return -1;
}

/*******************************************************************************
* @brief xilinx_xcvr_cpll_read_config
*******************************************************************************/
int32_t xilinx_xcvr_cpll_read_config(xcvr_core *core,
		xcvr_cpll *cpll_config)
{
	uint32_t val;
	int32_t ret;

	ret = xcvr_drp_read(core, XCVR_REG_CH_SEL, CPLL_REFCLK_DIV_M_ADDR, &val);
	if (ret < 0)
		return ret;

	if (val & CPLL_FB_DIV_45_N1_MASK)
		cpll_config->fb_div_N1 = 5;
	else
		cpll_config->fb_div_N1 = 4;

	switch (val & CPLL_FBDIV_N2_MASK) {
	case 3:
		cpll_config->fb_div_N2 = 5;
		break;
	case 2:
		cpll_config->fb_div_N2 = 4;
		break;
	case 1:
		cpll_config->fb_div_N2 = 3;
		break;
	case 0:
		cpll_config->fb_div_N2 = 2;
		break;
	default:
		cpll_config->fb_div_N2 = 1;
		break;
	}

	if (val & CPLL_REFCLK_DIV_M_MASK)
		cpll_config->refclk_div_M = 1;
	else
		cpll_config->refclk_div_M = 2;

	return 0;
}

/*******************************************************************************
* @brief xilinx_xcvr_cpll_write_config
*******************************************************************************/
int32_t xilinx_xcvr_cpll_write_config(xcvr_core *core, xcvr_cpll *cpll_config)
{
	uint32_t val = 0;

	switch (cpll_config->fb_div_N1) {
	case 4:
		break;
	case 5:
		val |= CPLL_FB_DIV_45_N1_MASK;
		break;
	default:
		return -1;
	}

	switch (cpll_config->fb_div_N2) {
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

	switch (cpll_config->refclk_div_M) {
	case 1:
		val |= CPLL_REFCLK_DIV_M_MASK;
		break;
	case 2:
		break;
	default:
		return -1;
	}

	return xcvr_drp_update(core, XCVR_REG_CH_SEL, CPLL_REFCLK_DIV_M_ADDR,
		CPLL_REFCLK_DIV_M_MASK | CPLL_FB_DIV_45_N1_MASK | CPLL_FBDIV_N2_MASK,
		val);
}

/*******************************************************************************
* @brief xilinx_xcvr_cpll_calc_lane_rate
*******************************************************************************/
uint32_t xilinx_xcvr_cpll_calc_lane_rate(uint32_t ref_clk_khz,
		xcvr_cpll *cpll_config)
{
	if (cpll_config->refclk_div_M == 0 || cpll_config->out_div == 0)
		return 0;

	return (ref_clk_khz * cpll_config->fb_div_N1 * cpll_config->fb_div_N2 * 2)
	/ (cpll_config->refclk_div_M * cpll_config->out_div);
}

/*******************************************************************************
* @brief xilinx_xcvr_gth3_configure_cdr
*******************************************************************************/
int32_t xilinx_xcvr_gth3_configure_cdr(xcvr_core *core,
	uint32_t out_div)
{
	uint32_t cfg0, cfg1, cfg2, cfg3;

	switch (core->refclk_ppm) {
	case PM_200:
		cfg0 = 0x0018;
		break;
	case PM_700:
	case PM_1250:
		cfg0 = 0x8018;
		break;
	default:
		return -1;
	}

	if (core->encoding == ENC_8B10B) {

		cfg1 = 0xC208;
		cfg3 = 0x07FE;
		cfg3 = 0x0020;

		switch (out_div) {
		case 0: /* 1 */
			cfg2 = 0x2000;
			break;
		case 1: /* 2 */
			cfg2 = 0x1000;
			break;
		case 2: /* 4 */
			cfg2 = 0x0800;
			break;
		case 3: /* 8 */
			cfg2 = 0x0400;
			break;
		default:
			return -1;
		}

	} else {
		printf("%s: GTH PRBS CDR not implemented\n", __func__);
		return 0;
	}

	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG0_ADDR, cfg0);
	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG1_ADDR, cfg1);
	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG2_ADDR, cfg2);
	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG3_ADDR, cfg3);

	return 0;
}

/*******************************************************************************
* @brief xilinx_xcvr_gtx2_configure_cdr
*******************************************************************************/
int32_t xilinx_xcvr_gtx2_configure_cdr(xcvr_core *core,
	uint32_t lane_rate, uint32_t out_div,
	uint8_t lpm_enable)
{
	uint32_t cfg0, cfg1, cfg2, cfg3, cfg4;

	cfg0 = 0x0020;
	cfg2 = 0x23FF;

	switch (core->refclk_ppm) {
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

	if (core->encoding == ENC_8B10B) {

		cfg4 = 0x03;

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

		if (lane_rate > 6600000 && out_div == 1)
			cfg4 = 0x0B;
		else
			cfg4 = 0x03;

		switch (out_div) {
		case 1:
			if (lpm_enable) {
				if (lane_rate  > 6600000) {
					if (core->refclk_ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					cfg1 = 0x1020;
				}
			} else { /* DFE */
				if (lane_rate  > 6600000) {
					if (core->refclk_ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					if (core->refclk_ppm == PM_1250)
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

	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG0_ADDR, cfg0);
	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG1_ADDR, cfg1);
	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG2_ADDR, cfg2);
	xcvr_drp_write(core, XCVR_REG_CH_SEL, RXCDR_CFG3_ADDR, cfg3);
	xcvr_drp_update(core, XCVR_REG_CH_SEL, RXCDR_CFG4_ADDR, RXCDR_CFG4_MASK, cfg4);

	return 0;
}

/*******************************************************************************
* @brief xilinx_xcvr_configure_cdr
*******************************************************************************/
int32_t xilinx_xcvr_configure_cdr(xcvr_core *core,
	uint32_t lane_rate, uint32_t out_div,
	uint8_t lpm_enable)
{
	switch (core->dev.gt_type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_configure_cdr(core, lane_rate,
			out_div, lpm_enable);
	case XILINX_XCVR_TYPE_US_GTH3:
		return xilinx_xcvr_gth3_configure_cdr(core, out_div);
	default:
		printf("%s ERROR: xcvr type not defined\n",__func__);
		return -1;
	}
}

/*******************************************************************************
* @brief xilinx_xcvr_configure_lpm_dfe_mode
*******************************************************************************/
int32_t xilinx_xcvr_configure_lpm_dfe_mode(xcvr_core *core,
	uint8_t lpm_dfe_n)
{
	switch (core->dev.gt_type) {
	case XILINX_XCVR_TYPE_US_GTH3:
		if (lpm_dfe_n) {
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x036, 0x0032);
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x039, 0x1000);
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x062, 0x1980);
		} else {
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x036, 0x0002);
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x039, 0x0000);
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x062, 0x0000);
		}
		break;
	case XILINX_XCVR_TYPE_S7_GTX2:
		if (lpm_dfe_n)
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x029, 0x0104);
		else
			xcvr_drp_write(core, XCVR_REG_CH_SEL, 0x029, 0x0954);
		break;
	}

	return 0;
}

/*******************************************************************************
* @brief *xilinx_xcvr_out_div_to_val
*******************************************************************************/
uint32_t write_xilinx_xcvr_out_div_to_val(uint32_t out_div)
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

uint32_t read_xilinx_xcvr_out_div_to_val(uint32_t out_div)
{
	switch (out_div) {
	case 0:
		return 1;
	case 1:
		return 2;
	case 2:
		return 4;
	case 3:
		return 8;
	case 4:
		return 16;
	default:
		return -1;
	}
}

/*******************************************************************************
* @brief xilinx_xcvr_read_out_div
*******************************************************************************/
int32_t xilinx_xcvr_read_out_div(xcvr_core *core,
	uint32_t *rx_out_div, uint32_t *tx_out_div)
{
	uint32_t val;
	int32_t ret;

	ret = xcvr_drp_read(core, XCVR_REG_CH_SEL, OUT_DIV_ADDR, &val);
	if (ret < 0)
		return ret;

	if (rx_out_div)
		*rx_out_div = read_xilinx_xcvr_out_div_to_val((val >> OUT_DIV_RX_OFFSET) & 7);
	if (tx_out_div)
		*tx_out_div = read_xilinx_xcvr_out_div_to_val((val >> OUT_DIV_TX_OFFSET) & 7);

	return 0;
}

/*******************************************************************************
* @brief xilinx_xcvr_write_out_div
*******************************************************************************/
int32_t xilinx_xcvr_write_out_div(xcvr_core *core,
	int32_t rx_out_div, int32_t tx_out_div)
{
	uint32_t val = 0;
	uint32_t mask = 0;

	if (tx_out_div >= 0) {
		val |= write_xilinx_xcvr_out_div_to_val(tx_out_div) << OUT_DIV_TX_OFFSET;
		mask |= 0x7 << OUT_DIV_TX_OFFSET;
	}
	if (rx_out_div >= 0) {
		val |= write_xilinx_xcvr_out_div_to_val(rx_out_div) << OUT_DIV_RX_OFFSET;
		mask |= 0x7 << OUT_DIV_RX_OFFSET;
	}

	return xcvr_drp_update(core, XCVR_REG_CH_SEL, OUT_DIV_ADDR, mask, val);
}

/*******************************************************************************
* @brief xilinx_xcvr_write_rx_clk25_div
*******************************************************************************/
int32_t xilinx_xcvr_write_rx_clk25_div(xcvr_core *core,
	uint32_t div)
{
	if (div == 0 || div > 32)
		return -1;

	div--;
	div <<= RX_CLK25_DIV_OFFSET;

	return xcvr_drp_update(core, XCVR_REG_CH_SEL, RX_CLK25_DIV,
		RX_CLK25_DIV_MASK, div);
}

/*******************************************************************************
* @brief xilinx_xcvr_write_tx_clk25_div
*******************************************************************************/
int32_t xilinx_xcvr_write_tx_clk25_div(xcvr_core *core,
	uint32_t div)
{
	if (div == 0 || div > 32)
		return -1;

	div--;

	return xcvr_drp_update(core, XCVR_REG_CH_SEL, TX_CLK25_DIV,
		TX_CLK25_DIV_MASK, div);
}

#endif

/*******************************************************************************
*******************************************************************************/
