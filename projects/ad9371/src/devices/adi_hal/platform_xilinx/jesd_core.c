/***************************************************************************//**
 * @file jesd_core.c
 * @brief Implementation of JESD Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include <inttypes.h>
#include <stdio.h>
#include "mykonos.h"
#include "jesd_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define XCVR_REG_RESETN			0x0010
#define XCVR_RESETN				(1 << 0)

#define XCVR_REG_STATUS			0x0014
#define XCVR_STATUS				(1 << 0)

#define XCVR_REG_CONTROL		0x0020
#define XCVR_LPM_DFE_N			(1 << 12)
#define XCVR_RATE(x)			(((x) & 0x7) << 8)
#define XCVR_SYSCLK_SEL(x)		(((x) & 0x3) << 4)
#define XCVR_OUTCLK_SEL(x)		(((x) & 0x7) << 0)

#define XCVR_REG_SYNTH			0x24

#define XCVR_REG_DRP_SEL(x)		(0x0040 + (x))

#define XCVR_REG_DRP_CTRL(x)	(0x0044 + (x))
#define XCVR_DRP_CTRL_WR		(1 << 28)
#define XCVR_DRP_CTRL_ADDR(x)	(((x) & 0xFFF) << 16)
#define XCVR_DRP_CTRL_WDATA(x)	(((x) & 0xFFFF) << 0)

#define XCVR_REG_DRP_STATUS(x)		(0x0048 + (x))
#define XCVR_DRP_STATUS_BUSY		(1 << 16)
#define XCVR_DRP_STATUS_RDATA(x)	(((x) & 0xFFFF) << 0)

#define XCVR_DRP_PORT_ADDR_COMMON	0x00
#define XCVR_DRP_PORT_ADDR_CHANNEL	0x20

#define XCVR_DRP_PORT_COMMON		0x00
#define XCVR_DRP_PORT_CHANNEL(x)	(0x1 + x)
#define XCVR_DRP_PORT_CHANNEL_BCAST	0xff

#define XCVR_BROADCAST				0xff

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

#define QPLL_CFG0_ADDR			0x32
#define QPLL_CFG0_LOWBAND_MASK	0x0040

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

#define QPLL_FBDIV(x)			(0x14 + (x) * 0x80)
#define QPLL_REFCLK_DIV(x)		(0x18 + (x) * 0x80)

#define ENC_8B10B					810

#define JESD204_REG_LINK_DISABLE	0xc0
#define JESD204_REG_LINK_CONF0		0x210

#define DIV_ROUND_CLOSEST(x, y)	(uint32_t)(((double)x / y) + 0.5)

static const char *adxcvr_gt_names[] = {
	[XILINX_XCVR_TYPE_S7_GTX2] = "GTX2",
	[XILINX_XCVR_TYPE_US_GTH3] = "GTH3",
	[XILINX_XCVR_TYPE_US_GTH4] = "GTH4",
};

/***************************************************************************//**
 * @brief xcvr_write
 *******************************************************************************/
void xcvr_write(xcvr_device *dev,
				uint32_t reg_addr,
				uint32_t reg_val)
{
	Xil_Out32((dev->base_addr + reg_addr), reg_val);
}

/***************************************************************************//**
 * @brief xcvr_read
 *******************************************************************************/
int32_t xcvr_read(xcvr_device *dev,
				  uint32_t reg_addr)
{
	return Xil_In32(dev->base_addr + reg_addr);
}

/***************************************************************************//**
 * @brief xcvr_drp_wait_idle
 *******************************************************************************/
int32_t xcvr_drp_wait_idle(xcvr_device *dev,
						   uint32_t drp_addr)
{
	uint32_t val;
	int32_t timeout = 20;

	do {
		val = xcvr_read(dev, XCVR_REG_DRP_STATUS(drp_addr));
		if (!(val & XCVR_DRP_STATUS_BUSY))
			return XCVR_DRP_STATUS_RDATA(val);

		mdelay(1);
	} while (timeout--);

	printf("%s: Timeout!", __func__);

	return -1;
}

/***************************************************************************//**
 * @brief xcvr_drp_read
 *******************************************************************************/
int32_t xcvr_drp_read(xcvr_device *dev,
					  uint32_t drp_port,
					  uint32_t reg)
{
	uint32_t drp_sel = XCVR_BROADCAST;
	uint32_t drp_addr;
	int32_t ret;

	if (drp_port == XCVR_DRP_PORT_COMMON) {
		drp_addr = XCVR_DRP_PORT_ADDR_COMMON;
	} else {
		drp_addr = XCVR_DRP_PORT_ADDR_CHANNEL;
		if (drp_port != XCVR_DRP_PORT_CHANNEL_BCAST)
			drp_sel = drp_port - 1;
	}

	xcvr_write(dev, XCVR_REG_DRP_SEL(drp_addr), drp_sel);
	xcvr_write(dev, XCVR_REG_DRP_CTRL(drp_addr), XCVR_DRP_CTRL_ADDR(reg));

	ret = xcvr_drp_wait_idle(dev, drp_addr);
	if (ret < 0)
		return ret;

	return ret & 0xffff;
}

/***************************************************************************//**
 * @brief xcvr_drp_write
 *******************************************************************************/
int32_t xcvr_drp_write(xcvr_device *dev,
					   uint32_t drp_port,
					   uint32_t reg,
					   uint32_t val)
{
	uint32_t drp_sel = XCVR_BROADCAST;
	uint32_t drp_addr;
	int32_t ret;

	if (drp_port == XCVR_DRP_PORT_COMMON) {
		drp_addr = XCVR_DRP_PORT_ADDR_COMMON;
	} else {
		drp_addr = XCVR_DRP_PORT_ADDR_CHANNEL;
		if (drp_port != XCVR_DRP_PORT_CHANNEL_BCAST)
			drp_sel = drp_port - 1;
	}

	xcvr_write(dev, XCVR_REG_DRP_SEL(drp_addr), drp_sel);
	xcvr_write(dev, XCVR_REG_DRP_CTRL(drp_addr), (XCVR_DRP_CTRL_WR |
		XCVR_DRP_CTRL_ADDR(reg) | XCVR_DRP_CTRL_WDATA(val)));

	ret = xcvr_drp_wait_idle(dev, drp_addr);
	if (ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
* @brief xcvr_drp_update
*******************************************************************************/
int32_t xcvr_drp_update(xcvr_device *dev,
						uint32_t drp_port,
						uint32_t reg,
						uint32_t mask,
						uint32_t val)
{
	int32_t ret;

	ret = xcvr_drp_read(dev, drp_port, reg);
	if (ret < 0)
		return ret;

	val |= ret & ~mask;

	return xcvr_drp_write(dev, drp_port, reg, val);
}

/***************************************************************************//**
* @brief xcvr_calc_cpll_config
*******************************************************************************/
int32_t xcvr_calc_cpll_config(xcvr_device *dev,
							  uint32_t refclk_khz,
							  uint32_t lane_rate_khz,
							  struct xcvr_cpll_config *conf,
							  uint32_t *out_div)
{
	uint32_t n1, n2, d, m;
	uint32_t vco_freq;
	uint32_t vco_min;
	uint32_t vco_max;

	switch (dev->type) {
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

	printf("%s: Failed to find matching dividers for %lu kHz rate\n",
			__func__, lane_rate_khz);

	return -1;
}

/***************************************************************************//**
* @brief xcvr_calc_qpll_config
*******************************************************************************/
int32_t xcvr_calc_qpll_config(xcvr_device *dev,
							  uint32_t refclk_khz,
							  uint32_t lane_rate_khz,
							  struct xcvr_qpll_config *conf,
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
			112, 120, 125, 150, 160, 0};

	switch (dev->type) {
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

	printf("%s: Failed to find matching dividers for %lu kHz rate\n",
			__func__, lane_rate_khz);

	return -1;
}

/***************************************************************************//**
* @brief xcvr_gtx2_cpll_write_config
*******************************************************************************/
int32_t xcvr_gtx2_cpll_write_config(xcvr_device *dev,
									uint32_t drp_port,
									const struct xcvr_cpll_config *conf)
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

	return xcvr_drp_update(dev, drp_port, CPLL_REFCLK_DIV_M_ADDR,
			CPLL_REFCLK_DIV_M_MASK | CPLL_FB_DIV_45_N1_MASK | CPLL_FBDIV_N2_MASK,
			val);
}

/***************************************************************************//**
* @brief xcvr_gth34_cpll_write_config
*******************************************************************************/
int32_t xcvr_gth34_cpll_write_config(xcvr_device *dev,
									 uint32_t drp_port,
									 const struct xcvr_cpll_config *conf)
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

	ret = xcvr_drp_update(dev, drp_port, 0x28,
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

	return xcvr_drp_update(dev, drp_port, 0x2a,
			0xf800, val);
}

/***************************************************************************//**
* @brief xcvr_cpll_write_config
*******************************************************************************/
int32_t xcvr_cpll_write_config(xcvr_device *dev,
							   uint32_t drp_port,
							   const struct xcvr_cpll_config *conf)
{
	switch (dev->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xcvr_gtx2_cpll_write_config(dev, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xcvr_gth34_cpll_write_config(dev, drp_port, conf);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_qpll_write_config
*******************************************************************************/
int32_t xcvr_gtx2_qpll_write_config(xcvr_device *dev,
									uint32_t drp_port,
									const struct xcvr_qpll_config *conf)
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
		printf("Invalid refclk divider: %lu\n", conf->refclk_div);
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
		printf("Invalid feedback divider: %lu\n", conf->fb_div);
		return -1;
	}

	if (conf->band)
		cfg0 = 0;
	else
		cfg0 = QPLL_CFG0_LOWBAND_MASK;

	ret = xcvr_drp_update(dev, drp_port, QPLL_CFG0_ADDR,
		QPLL_CFG0_LOWBAND_MASK, cfg0);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(dev, drp_port, QPLL_CFG1_ADDR,
		QPLL_REFCLK_DIV_M_MASK, cfg1);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(dev, drp_port, QPLL_FBDIV_N_ADDR,
		QPLL_FBDIV_N_MASK, fbdiv);
	if (ret < 0)
		return ret;

	ret = xcvr_drp_update(dev, drp_port, QPLL_FBDIV_RATIO_ADDR,
		QPLL_FBDIV_RATIO_MASK, fbdiv_ratio);
	if (ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
* @brief xcvr_gth34_qpll_write_config
*******************************************************************************/
int32_t xcvr_gth34_qpll_write_config(xcvr_device *dev,
									 uint32_t drp_port,
									 const struct xcvr_qpll_config *conf)
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
		printf("Invalid refclk divider: %lu\n", conf->refclk_div);
		return -1;
	}

	ret = xcvr_drp_update(dev, drp_port, QPLL_FBDIV(0),
		0xff, fbdiv);
	if (ret < 0)
		return ret;

	return xcvr_drp_update(dev, drp_port, QPLL_REFCLK_DIV(0),
		0xf80, refclk << 7);
}

/***************************************************************************//**
* @brief xcvr_qpll_write_config
*******************************************************************************/
int32_t xcvr_qpll_write_config(xcvr_device *dev,
							   uint32_t drp_port,
							   const struct xcvr_qpll_config *conf)
{
	switch (dev->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xcvr_gtx2_qpll_write_config(dev, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xcvr_gth34_qpll_write_config(dev, drp_port, conf);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_out_div_to_val
*******************************************************************************/
uint32_t xcvr_out_div_to_val(uint32_t out_div)
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
* @brief xcvr_gtx2_write_out_div
*******************************************************************************/
int32_t xcvr_gtx2_write_out_div(xcvr_device *dev,
								uint32_t drp_port,
								int32_t rx_out_div,
								int32_t tx_out_div)
{
	uint32_t val = 0;
	uint32_t mask = 0;

	if (tx_out_div >= 0) {
		val |= xcvr_out_div_to_val(tx_out_div) << OUT_DIV_TX_OFFSET;
		mask |= 0x7 << OUT_DIV_TX_OFFSET;
	}
	if (rx_out_div >= 0) {
		val |= xcvr_out_div_to_val(rx_out_div) << OUT_DIV_RX_OFFSET;
		mask |= 0x7 << OUT_DIV_RX_OFFSET;
	}

	return xcvr_drp_update(dev, drp_port, OUT_DIV_ADDR, mask, val);
}

/***************************************************************************//**
* @brief xcvr_gth34_write_out_div
*******************************************************************************/
int32_t xcvr_gth34_write_out_div(xcvr_device *dev,
								 uint32_t drp_port,
								 int32_t rx_out_div,
								 int32_t tx_out_div)
{
	int32_t ret;

	if (rx_out_div >= 0) {
		ret = xcvr_drp_update(dev, drp_port, 0x63, 0x7,
			xcvr_out_div_to_val(rx_out_div));
		if (ret)
			return ret;
	}
	if (tx_out_div >= 0) {
		ret = xcvr_drp_update(dev, drp_port, 0x7c, 0x700,
			xcvr_out_div_to_val(tx_out_div) << 8);
		if (ret)
			return ret;
	}

	return 0;
}

/***************************************************************************//**
* @brief xcvr_write_out_div
*******************************************************************************/
int32_t xcvr_write_out_div(xcvr_device *dev,
						   uint32_t drp_port,
						   int32_t rx_out_div,
						   int32_t tx_out_div)
{
	switch (dev->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xcvr_gtx2_write_out_div(dev, drp_port,
			rx_out_div, tx_out_div);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xcvr_gth34_write_out_div(dev, drp_port,
			rx_out_div, tx_out_div);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_configure_cdr
*******************************************************************************/
int32_t xcvr_gtx2_configure_cdr(xcvr_device *dev,
								uint32_t drp_port,
								uint32_t lane_rate,
								uint32_t out_div,
								uint8_t lpm_enable)
{
	uint32_t cfg0, cfg1, cfg2, cfg3, cfg4;

	cfg0 = 0x0020;
	cfg2 = 0x23FF;

	switch (dev->ppm) {
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

	if (dev->encoding == ENC_8B10B) {

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
					if (dev->ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					cfg1 = 0x1020;
				}
			} else { /* DFE */
				if (lane_rate  > 6600000) {
					if (dev->ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					if (dev->ppm == PM_1250)
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

	xcvr_drp_write(dev, drp_port, RXCDR_CFG0_ADDR, cfg0);
	xcvr_drp_write(dev, drp_port, RXCDR_CFG1_ADDR, cfg1);
	xcvr_drp_write(dev, drp_port, RXCDR_CFG2_ADDR, cfg2);
	xcvr_drp_write(dev, drp_port, RXCDR_CFG3_ADDR, cfg3);
	xcvr_drp_update(dev, drp_port, RXCDR_CFG4_ADDR, RXCDR_CFG4_MASK, cfg4);

	return 0;
}

/***************************************************************************//**
* @brief xcvr_gth3_configure_cdr
*******************************************************************************/
int32_t xcvr_gth3_configure_cdr(xcvr_device *dev,
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
int32_t xcvr_configure_cdr(xcvr_device *dev,
						   uint32_t drp_port,
						   uint32_t lane_rate,
						   uint32_t out_div,
						   uint8_t lpm_enable)
{
	switch (dev->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xcvr_gtx2_configure_cdr(dev, drp_port, lane_rate,
			out_div, lpm_enable);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xcvr_gth3_configure_cdr(dev, drp_port, out_div);
	default:
		return -1;
	}
}

/***************************************************************************//**
* @brief xcvr_write_rx_clk25_div
*******************************************************************************/
int32_t xcvr_write_rx_clk25_div(xcvr_device *dev,
								uint32_t drp_port,
								uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return -1;

	div--;

	switch (dev->type) {
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

	return xcvr_drp_update(dev, drp_port, reg, mask, div);
}

/***************************************************************************//**
* @brief xcvr_write_tx_clk25_div
*******************************************************************************/
int32_t xcvr_write_tx_clk25_div(xcvr_device *dev,
								uint32_t drp_port,
								uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return -1;

	div--;

	switch (dev->type) {
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

	return xcvr_drp_update(dev, drp_port, reg, mask, div);
}

/***************************************************************************//**
* @brief xcvr_clk_set_rate
*******************************************************************************/
int32_t xcvr_clk_set_rate(xcvr_device *dev,
						  uint32_t rate,
						  uint32_t parent_rate)
{
	struct xcvr_cpll_config cpll_conf;
	struct xcvr_qpll_config qpll_conf;
	uint32_t out_div, clk25_div;
	uint32_t i;
	int32_t ret;

	clk25_div = DIV_ROUND_CLOSEST(parent_rate, 25000);

	if (dev->cpll_enable)
		ret = xcvr_calc_cpll_config(dev, parent_rate, rate,
				&cpll_conf, &out_div);
	else
		ret = xcvr_calc_qpll_config(dev, parent_rate, rate,
				&qpll_conf, &out_div);
	if (ret < 0)
		return ret;

	if (dev->cpll_enable)
		ret = xcvr_cpll_write_config(dev,
			XCVR_DRP_PORT_CHANNEL_BCAST, &cpll_conf);
	else
		ret = xcvr_qpll_write_config(dev,
			XCVR_DRP_PORT_COMMON, &qpll_conf);
	if (ret < 0)
		return ret;

	for (i = 0; i < dev->num_lanes; i++) {
		ret = xcvr_write_out_div(dev,
			XCVR_DRP_PORT_CHANNEL(i),
			dev->tx_enable ? -1 : out_div,
			dev->tx_enable ? out_div : -1);
		if (ret < 0)
			return ret;
	}

	if (!dev->tx_enable) {
		ret = xcvr_configure_cdr(dev,
			XCVR_DRP_PORT_CHANNEL_BCAST, rate, out_div,
			dev->lpm_enable);
		if (ret < 0)
			return ret;

		ret = xcvr_write_rx_clk25_div(dev,
			XCVR_DRP_PORT_CHANNEL_BCAST, clk25_div);
	} else {
		ret = xcvr_write_tx_clk25_div(dev,
			XCVR_DRP_PORT_CHANNEL_BCAST, clk25_div);
	}
	if (ret < 0)
		return ret;

	dev->lane_rate_khz = rate;

	return 0;
}

/***************************************************************************//**
* @brief xcvr_configure_lpm_dfe_mode
*******************************************************************************/
int32_t xcvr_configure_lpm_dfe_mode(xcvr_device *dev,
									uint32_t drp_port,
									uint8_t lpm)
{
	switch (dev->type) {
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		/*
		 * TODO: UltraScale FPGAs Transceivers Wizard should be used for
		 *	 generating these settings
		 */
		break;
	case XILINX_XCVR_TYPE_S7_GTX2:
		if (lpm)
			xcvr_drp_write(dev, drp_port, 0x029, 0x0104);
		else
			xcvr_drp_write(dev, drp_port, 0x029, 0x0954);
		break;
	}

	return 0;
}

/***************************************************************************//**
 * @brief xcvr_setup
 *******************************************************************************/
int32_t xcvr_setup(mykonosDevice_t *myk_dev)
{
	xcvr_device	rx_xcvr;
	xcvr_device	tx_xcvr;
	xcvr_device	rx_os_xcvr;
	uint32_t	synth_conf;
	uint32_t	status;
	int32_t		timeout;
	int32_t		ret = 0;

	rx_xcvr.base_addr = XPAR_AXI_AD9371_RX_XCVR_BASEADDR;
	synth_conf = xcvr_read(&rx_xcvr, XCVR_REG_SYNTH);
	rx_xcvr.tx_enable = (synth_conf >> 8) & 1;
	rx_xcvr.num_lanes = synth_conf & 0xff;
	rx_xcvr.type = (synth_conf >> 16) & 0xf;
	switch (rx_xcvr.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		break;
	default:
		printf("Unknown transceiver type: %d\n", rx_xcvr.type);
		return -1;
	}
	rx_xcvr.ref_rate_khz = myk_dev->clocks->deviceClock_kHz;
	rx_xcvr.lane_rate_khz = myk_dev->rx->rxProfile->iqRate_kHz * 40;
	rx_xcvr.cpll_enable = 1;
	rx_xcvr.lpm_enable = 1;
	rx_xcvr.encoding = ENC_8B10B;
	rx_xcvr.ppm = PM_200;

	tx_xcvr.base_addr = XPAR_AXI_AD9371_TX_XCVR_BASEADDR;
	synth_conf = xcvr_read(&tx_xcvr, XCVR_REG_SYNTH);
	tx_xcvr.tx_enable = (synth_conf >> 8) & 1;
	tx_xcvr.num_lanes = synth_conf & 0xff;
	tx_xcvr.type = (synth_conf >> 16) & 0xf;
	switch (tx_xcvr.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		break;
	default:
		printf("Unknown transceiver type: %d\n", tx_xcvr.type);
		return -1;
	}
	tx_xcvr.ref_rate_khz = myk_dev->clocks->deviceClock_kHz;
	tx_xcvr.lane_rate_khz = myk_dev->tx->txProfile->iqRate_kHz * 20;
	tx_xcvr.cpll_enable = 0;
	tx_xcvr.lpm_enable = 0;
	tx_xcvr.encoding = ENC_8B10B;
	tx_xcvr.ppm = PM_200;

	rx_os_xcvr.base_addr = XPAR_AXI_AD9371_RX_OS_XCVR_BASEADDR;
	synth_conf = xcvr_read(&rx_os_xcvr, XCVR_REG_SYNTH);
	rx_os_xcvr.tx_enable = (synth_conf >> 8) & 1;
	rx_os_xcvr.num_lanes = synth_conf & 0xff;
	rx_os_xcvr.type = (synth_conf >> 16) & 0xf;
	switch (rx_os_xcvr.type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		break;
	default:
		printf("Unknown transceiver type: %d\n", rx_os_xcvr.type);
		return -1;
	}
	rx_os_xcvr.ref_rate_khz = myk_dev->clocks->deviceClock_kHz;
	rx_os_xcvr.lane_rate_khz = myk_dev->obsRx->orxProfile->iqRate_kHz * 20;
	rx_os_xcvr.cpll_enable = 1;
	rx_os_xcvr.lpm_enable = 1;
	rx_os_xcvr.encoding = ENC_8B10B;
	rx_os_xcvr.ppm = PM_200;

	xcvr_write(&rx_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(&rx_xcvr, XCVR_REG_CONTROL,
			XCVR_LPM_DFE_N | XCVR_OUTCLK_SEL(3));
	xcvr_configure_lpm_dfe_mode(&rx_xcvr,
			XCVR_DRP_PORT_CHANNEL_BCAST, rx_xcvr.lpm_enable);
	xcvr_clk_set_rate(&rx_xcvr,
			rx_xcvr.lane_rate_khz, rx_xcvr.ref_rate_khz);
	xcvr_write(&rx_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	xcvr_write(&tx_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(&tx_xcvr, XCVR_REG_CONTROL,
			XCVR_SYSCLK_SEL(3) | XCVR_OUTCLK_SEL(3));
	xcvr_clk_set_rate(&tx_xcvr,
			tx_xcvr.lane_rate_khz, tx_xcvr.ref_rate_khz);
	xcvr_write(&tx_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	xcvr_write(&rx_os_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(&rx_os_xcvr, XCVR_REG_CONTROL,
			XCVR_LPM_DFE_N | XCVR_OUTCLK_SEL(3));
	xcvr_configure_lpm_dfe_mode(&rx_os_xcvr,
			XCVR_DRP_PORT_CHANNEL_BCAST, rx_os_xcvr.lpm_enable);
	xcvr_clk_set_rate(&rx_os_xcvr,
			rx_os_xcvr.lane_rate_khz, rx_os_xcvr.ref_rate_khz);
	xcvr_write(&rx_os_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	mdelay(1);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		status = xcvr_read(&rx_xcvr, XCVR_REG_STATUS);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("RX_XCVR (%s) initialization failed\n", adxcvr_gt_names[rx_xcvr.type]);
		ret--;
	} else
		printf("RX_XCVR (%s) initialization OK\n", adxcvr_gt_names[rx_xcvr.type]);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		status = xcvr_read(&tx_xcvr, XCVR_REG_STATUS);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("TX_XCVR (%s) initialization failed\n", adxcvr_gt_names[tx_xcvr.type]);
		ret--;
	} else
		printf("TX_XCVR (%s) initialization OK\n", adxcvr_gt_names[tx_xcvr.type]);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		status = xcvr_read(&rx_os_xcvr, XCVR_REG_STATUS);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("RX_OS_XCVR (%s) initialization failed\n", adxcvr_gt_names[rx_os_xcvr.type]);
		ret--;
	} else
		printf("RX_OS_XCVR (%s) initialization OK\n", adxcvr_gt_names[rx_os_xcvr.type]);

	return ret;
}

/***************************************************************************//**
 * @brief jesd_write
 *******************************************************************************/
int32_t jesd_write(jesd_device *dev,
				   uint32_t reg_addr,
				   uint32_t reg_val)
{
	Xil_Out32((dev->base_addr + reg_addr), reg_val);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_read
 *******************************************************************************/
int32_t jesd_read(jesd_device *dev,
				  uint32_t reg_addr,
				  uint32_t *reg_val)
{
	*reg_val = Xil_In32(dev->base_addr + reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_setup
 *******************************************************************************/
int32_t jesd_setup(mykonosDevice_t *myk_dev)
{
	jesd_device	rx_jesd;
	jesd_device	tx_jesd;
	jesd_device	rx_os_jesd;
	uint32_t octets_per_frame;
	uint32_t octets_per_multiframe;

	rx_jesd.base_addr = RX_JESD_BASEADDR;
	tx_jesd.base_addr = TX_JESD_BASEADDR;
	rx_os_jesd.base_addr = RX_OS_JESD_BASEADDR;

	octets_per_frame = 0x4;
	octets_per_multiframe = octets_per_frame * myk_dev->rx->framer->K;
	jesd_write(&rx_jesd, JESD204_REG_LINK_CONF0,
			((octets_per_frame-1) << 16) | (octets_per_multiframe - 1));
	jesd_write(&rx_jesd, JESD204_REG_LINK_DISABLE, 0x1);

	octets_per_frame = 0x2;
	octets_per_multiframe = octets_per_frame * myk_dev->tx->deframer->K;
	jesd_write(&tx_jesd, JESD204_REG_LINK_CONF0,
			((octets_per_frame-1) << 16) | (octets_per_multiframe - 1));
	jesd_write(&tx_jesd, JESD204_REG_LINK_DISABLE, 0x1);

	octets_per_frame = 0x2;
	octets_per_multiframe = octets_per_frame * myk_dev->obsRx->framer->K;
	jesd_write(&rx_os_jesd, JESD204_REG_LINK_CONF0,
			((octets_per_frame-1) << 16) | (octets_per_multiframe - 1));
	jesd_write(&rx_os_jesd, JESD204_REG_LINK_DISABLE, 0x1);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_tx_enable
 *******************************************************************************/
int32_t jesd_tx_enable(mykonosDevice_t *myk_dev)
{
	jesd_device	tx_jesd;

	tx_jesd.base_addr = TX_JESD_BASEADDR;

	jesd_write(&tx_jesd, JESD204_REG_LINK_DISABLE, 0x0);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_rx_enable
 *******************************************************************************/
int32_t jesd_rx_enable(mykonosDevice_t *myk_dev)
{
	jesd_device	rx_jesd;
	jesd_device	rx_os_jesd;

	rx_jesd.base_addr = RX_JESD_BASEADDR;
	rx_os_jesd.base_addr = RX_OS_JESD_BASEADDR;

	jesd_write(&rx_jesd, JESD204_REG_LINK_DISABLE, 0x0);
	jesd_write(&rx_os_jesd, JESD204_REG_LINK_DISABLE, 0x0);

	return 0;
}
