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
#include "no_os_util.h"
#include "no_os_error.h"
#include "axi_adxcvr.h"
#include "xilinx_transceiver.h"
#include "no_os_print_log.h"

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

#define GTH34_SYSCLK_QPLL1		2

#define GTX_RX_PRBS_ERR_CNT		0x15c /* 16-bit */
#define GTH3_RX_PRBS_ERR_CNT	0x15E /* 32-bit */
#define GTH4_RX_PRBS_ERR_CNT	0x25E /* 32-bit also applied for GTY */

#define GTH34_QPLL_FBDIV(xcvr, x)	\
	(0x14 + xilinx_xcvr_qpll_sel((xcvr), (x)) * 0x80)
#define GTH34_QPLL_REFCLK_DIV(xcvr, x)	\
	(0x18 + xilinx_xcvr_qpll_sel((xcvr), (x)) * 0x80)
#define GTY4_QPLL_CLKOUT_RATE(xcvr, x)	\
	(0x0E + xilinx_xcvr_qpll_sel((xcvr), (x)) * 0x80)

/*******************************************************************************
 * @brief Read data from a dynamic reconfiguration port (DRP).
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP to read data from.
 * @param reg - DRP address.
 * @param val - Read value.
 *
 * @return ret - Result of the reading operation (0 - success, negative
 *               value for failure).
 *******************************************************************************/
static int xilinx_xcvr_drp_read(struct xilinx_xcvr *xcvr,
				uint32_t drp_port, uint32_t reg, uint32_t *val)
{
	int ret;

	ret = adxcvr_drp_read(xcvr->ad_xcvr, drp_port, reg, val);
	if (ret) {
		pr_err("%s: Failed to read reg %ld-%#06lx: %d\n",
		       __func__, drp_port, reg, ret);

		return -1;
	}

	return ret;
}

/*******************************************************************************
 * @brief Write data to a dynamic reconfiguration port (DRP).
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP to write data to.
 * @param reg - DRP address.
 * @param val - Value to be written.
 *
 * @return ret - Result of the writing operation (0 - success, negative
 *               value for failure).
 *******************************************************************************/
static int xilinx_xcvr_drp_write(struct xilinx_xcvr *xcvr,
				 uint32_t drp_port, uint32_t reg, uint32_t val)
{
	uint32_t read_val;
	int ret;

	pr_debug("%s: drp_port: %ld, reg %#06lx, val %#06lx. \n",
		 __func__, drp_port, reg, val);

	ret = adxcvr_drp_write(xcvr->ad_xcvr, drp_port, reg, val);
	if (ret) {
		pr_err("%s: Failed to write reg %ld-%#06lx: %d\n",
		       __func__, drp_port, reg, ret);
		return ret;
	}

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, reg, &read_val);
	if (ret) {
		pr_err("%s: Failed to check reg %ld-%#06lx: %d\n",
		       __func__, drp_port, reg, ret);
		return ret;
	}

	if (read_val != val)
		pr_err("%s: read-write mismatch: reg %#06lx,"
		       "val %#06lx, expected val %#06lx.\n",
		       __func__, reg, read_val, val);

	return 0;
}

/*******************************************************************************
 * @brief Update data of a dynamic reconfiguration port (DRP).
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP where data is updated.
 * @param reg - DRP address.
 * @param mask - The mask to be applied.
 * @param val - Value to be written.
 *
 * @return ret - Result of the writing operation (0 - success, negative
 *               value for failure).
 *******************************************************************************/
int xilinx_xcvr_drp_update(struct xilinx_xcvr *xcvr, uint32_t drp_port,
			   uint32_t reg, uint32_t mask, uint32_t val)
{
	uint32_t read_val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, reg, &read_val);
	if (ret < 0)
		return ret;

	val |= read_val & ~mask;

	return xilinx_xcvr_drp_write(xcvr, drp_port, reg, val);
}


/*******************************************************************************
 * @brief Configure Clock Data Recovery for GTH3 transceiver type.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param out_div - Output clock divider.
 *
 * @return ret - Result of the configuration operation (0 - success,
 *               negative value for failure).
*******************************************************************************/
static int xilinx_xcvr_gth3_configure_cdr(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, uint32_t out_div)
{
	/*
	 * TODO: UltraScale FPGAs Transceivers Wizard should be used for
	 *	 generating these settings
	 */

	return 0;
}

/*******************************************************************************
 * @brief Configure Clock Data Recovery for GTX2 transceiver type.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param lane_rate - Line rate (kHz).
 * @param out_div - Output clock divider.
 * @param lpm_enable - LPM enable.
 *
 * @return ret - Result of the cfg. operation (0 - success, negative
 *               value for failure).
*******************************************************************************/
static int xilinx_xcvr_gtx2_configure_cdr(struct xilinx_xcvr *xcvr,
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
		return -EINVAL;
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
			return -EINVAL;
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
			return -EINVAL;
		}
	}

	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG0_ADDR, cfg0);
	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG1_ADDR, cfg1);
	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG2_ADDR, cfg2);
	xilinx_xcvr_drp_write(xcvr, drp_port, RXCDR_CFG3_ADDR, cfg3);
	xilinx_xcvr_drp_update(xcvr, drp_port, RXCDR_CFG4_ADDR,
			       RXCDR_CFG4_MASK, cfg4);

	return 0;
}

/*******************************************************************************
 * @brief Configure Clock Data Recovery. THe clock data recovery (CDR) circuit in
 * each transceiver extracts the recovered clock and data from an incoming data
 * stream.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param lane_rate - Line rate (kHz).
 * @param out_div - Output clock divider.
 * @param lpm_enable - LPM enable.
 *
 * @return ret - Result of the cfrg. operation (0 - success, negative
 *               value for failure).
*******************************************************************************/
int xilinx_xcvr_configure_cdr(struct xilinx_xcvr *xcvr,
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
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Selection of Low-Power mode (LPM) or Decision Feedback Equalization (DFE).
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param lpm - LPM enable.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_configure_lpm_dfe_mode(struct xilinx_xcvr *xcvr,
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

	return 0;
}

/*******************************************************************************
 * @brief Selection of QPLL clock.
 *
 * @param xcvr - The device structure.
 * @param sys_clk_sel - QPLL0 (3) / QPLL1 (2) selection.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static unsigned int xilinx_xcvr_qpll_sel(struct xilinx_xcvr *xcvr,
		uint32_t sys_clk_sel)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		if (sys_clk_sel == GTH34_SYSCLK_QPLL1)
			return 1;
	/* fall-through */
	default:
		return 0;
	}
}

/*******************************************************************************
 * @brief Setup of CPLL vco range.
 *
 * @param xcvr - The device structure.
 * @param vco_max - Frequency upper limit (kHz).
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static void xilinx_xcvr_setup_cpll_vco_range(struct xilinx_xcvr *xcvr,
		uint32_t *vco_max)
{
	if  ((xcvr->type == XILINX_XCVR_TYPE_US_GTH3) |
	     (xcvr->type == XILINX_XCVR_TYPE_US_GTH4)) {
		if ((xcvr->voltage < 850))
			*vco_max = 4250000;
		else if ((xcvr->speed_grade / 10) == 1)
			*vco_max = 4250000;
	}

	if (xcvr->type == XILINX_XCVR_TYPE_US_GTY4)
		if ((xcvr->speed_grade / 10) == 1)
			*vco_max = 4250000;
}


/*******************************************************************************
 * @brief Setup QPLL nominal operating ranges.
 *
 * @param xcvr - The device structure.
 * @param vco0_min - Frequency lower limit for vco0 (kHz).
 * @param vco0_max - Frequency upper limit for vco0 (kHz).
 * @param vco1_min - Frequency lower limit for vco1 (kHz).
 * @param vco1_max - Frequency upper limit for vco1 (kHz).
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static void xilinx_xcvr_setup_qpll_vco_range(struct xilinx_xcvr *xcvr,
		uint32_t *vco0_min,
		uint32_t *vco0_max,
		uint32_t *vco1_min,
		uint32_t *vco1_max)
{
	if (xcvr->type == XILINX_XCVR_TYPE_S7_GTX2) {
		if ((xcvr->family == AXI_FPGA_FAMILY_KINTEX))
			if ((xcvr->dev_package == AXI_FPGA_DEV_FB) |
			    (xcvr->dev_package == AXI_FPGA_DEV_RF) |
			    (xcvr->dev_package == AXI_FPGA_DEV_FF))
				*vco0_max = 6600000;
		if ((xcvr->speed_grade / 10) == 2)
			*vco1_max = 10312500;
	}
}

/*******************************************************************************
 * @brief Get CPLL nominal operating ranges.
 *
 * @param xcvr - The device structure.
 * @param vco_min - Frequency lower limit for vco (kHz).
 * @param vco_max - Frequency upper limit for vco (kHz).
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_get_cpll_vco_ranges(struct xilinx_xcvr *xcvr,
		uint32_t *vco_min,
		uint32_t *vco_max)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		*vco_min = 1600000;
		*vco_max = 3300000;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		*vco_min = 2000000;
		*vco_max = 6250000;
		break;
	default:
		return -EINVAL;
	}

	if (AXI_PCORE_VER_MAJOR(xcvr->version) > 0x10)
		xilinx_xcvr_setup_cpll_vco_range(xcvr, vco_max);

	if (xcvr->vco0_min)
		*vco_min = xcvr->vco0_min;

	if (xcvr->vco0_max)
		*vco_max = xcvr->vco0_max;

	return 0;
}

/*******************************************************************************
 * @brief Calculate CPLL configuration.
 *
 * @param xcvr - The device structure.
 * @param refclk_khz - Reference clock (kHz).
 * @param lane_rate_khz - Line rate (kHz).
 * @param conf - CPLL configuration values.
 * @param out_div - Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_calc_cpll_config(struct xilinx_xcvr *xcvr,
				 uint32_t refclk_khz,
				 uint32_t lane_rate_khz,
				 struct xilinx_xcvr_cpll_config *conf, uint32_t *out_div)
{
	uint32_t n1, n2, d, m;
	uint32_t vco_freq;
	uint32_t vco_min;
	uint32_t vco_max;
	int ret;

	ret = xilinx_xcvr_get_cpll_vco_ranges(xcvr, &vco_min, &vco_max);
	if (ret)
		return ret;

	/**
	 * Ref: https://www.xilinx.com/support/documentation/user_guides/ug476_7Series_Transceivers.pdf
	 * Page: 48
	 *    Vco_Freq = (RefClk * n1 * n2) / m
	 *    LineRate = (Vco_Freq * 2) / d
	 */
	for (m = 1; m <= 2; m++) {
		for (d = 1; d <= 8; d <<= 1) {
			for (n1 = 5; n1 >= 4; n1--) {
				for (n2 = 5; n2 >= 1; n2--) {
					vco_freq = (refclk_khz * n1 * n2) / m;

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

	pr_debug("CPLL: failed to find setting for lane rate %lu kHz with reference clock %lu kHz\n",
		 lane_rate_khz, refclk_khz);

	return -EINVAL;
}

/*******************************************************************************
 * @brief Get QPLL nominal operating ranges.
 *
 * @param xcvr - The device structure.
 * @param sys_clk_sel - QPLL0 (3) / QPLL1 (2) selection.
 * @param vco0_min - Frequency lower limit for vco0 (kHz).
 * @param vco0_max - Frequency upper limit for vco0 (kHz).
 * @param vco1_min - Frequency lower limit for vco1 (kHz).
 * @param vco1_max - Frequency upper limit for vco1 (kHz).
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_get_qpll_vco_ranges(struct xilinx_xcvr *xcvr,
		uint32_t sys_clk_sel,
		uint32_t *vco0_min, uint32_t *vco0_max,
		uint32_t *vco1_min, uint32_t *vco1_max)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		*vco0_min = 5930000;
		*vco0_max = 8000000;
		*vco1_min = 9800000;
		*vco1_max = 12500000;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		if (xilinx_xcvr_qpll_sel(xcvr, sys_clk_sel)) {
			*vco0_min = 8000000;
			*vco0_max = 13000000;
		} else {
			*vco0_min = 9800000;
			*vco0_max = 16375000;
		}
		*vco1_min = *vco0_min;
		*vco1_max = *vco0_max;
		break;
	default:
		return -EINVAL;
	}

	if (AXI_PCORE_VER_MAJOR(xcvr->version) > 0x10)
		xilinx_xcvr_setup_qpll_vco_range(xcvr,
						 vco0_min, vco0_max,
						 vco1_min, vco1_max);

	if (xcvr->vco0_min)
		*vco0_min = xcvr->vco0_min;

	if (xcvr->vco0_max)
		*vco0_max = xcvr->vco0_max;

	if (xcvr->vco1_min)
		*vco1_min = xcvr->vco1_min;

	if (xcvr->vco1_max)
		*vco1_max = xcvr->vco1_max;

	return 0;
}


/*******************************************************************************
 * @brief Calculate QPLL configuration.
 *
 * @param xcvr - The device structure.
 * @param sys_clk_sel - QPLL0 (3) / QPLL1 (2) selection.
 * @param refclk_khz - Reference clock (kHz).
 * @param lane_rate_khz - Line rate (kHz).
 * @param conf - QPLL configuration values.
 * @param out_div - Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_calc_qpll_config(struct xilinx_xcvr *xcvr, uint32_t sys_clk_sel,
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
	int ret;

	static const uint8_t N_gtx2[] = {16, 20, 32, 40, 64, 66, 80, 100, 0};
	static const uint8_t N_gth34[] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
					  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
					  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
					  56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
					  71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
					  86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
					  101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
					  113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
					  125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136,
					  137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148,
					  149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 0
					 };
	/* N_gty4: entire range is 16-160, can add more if required */
	static const uint8_t N_gty4[] = {16, 20, 32, 33, 40, 64, 66, 75, 80, 99,
					 100, 112, 120, 125, 132, 150, 160, 0
					};

	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		N = N_gtx2;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
		N = N_gth34;
		break;
	case XILINX_XCVR_TYPE_US_GTY4:
		N = N_gty4;
		break;
	default:
		return -EINVAL;
	}

	ret = xilinx_xcvr_get_qpll_vco_ranges(xcvr, sys_clk_sel,
					      &vco0_min, &vco0_max,
					      &vco1_min, &vco1_max);
	if (ret)
		return ret;

	/**
	 * Ref: https://www.xilinx.com/support/documentation/user_guides/ug476_7Series_Transceivers.pdf
	 * Page: 55
	 *   Vco_Freq = (refclk_khz * n) / m
	 *   LineRate = Vco_Freq / d
	 *
	 * Make sure to not confuse Vco_Freq with fPLLClkout.
	 * fPLLClkout = (refclk_khz * n) / (m * 2), so technically Vco_Freq = 2 * fPLLClkout
	 * And the 2 is reduced in both equations.
	 */
	for (m = 1; m <= 4; m++) {
		for (d = 1; d <= 16; d <<= 1) {
			for (n = 0; N[n] != 0; n++) {
				vco_freq = (refclk_khz * N[n]) / m;

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
						conf->qty4_full_rate = 0;
					}

					if (out_div)
						*out_div = d;

					return 0;
				}

				if (xcvr->type != XILINX_XCVR_TYPE_US_GTY4)
					continue;

				/**
				 * Ref: https://www.xilinx.com/support/documentation/user_guides/ug578-ultrascale-gty-transceivers.pdf
				 * Page: 49
				 * For GTY4: LineRate = (2 * Vco_Freq) / d
				 * Try Full-rate
				 */
				if (refclk_khz / m / d == lane_rate_khz / 2 / N[n]) {

					if (conf) {
						conf->refclk_div = m;
						conf->fb_div = N[n];
						conf->band = band;
						conf->qty4_full_rate = 1;
					}

					if (out_div)
						*out_div = d;

					return 0;
				}
			}
		}
	}

	pr_debug("QPLL: failed to find setting for lane rate %lu kHz with reference clock %lu kHz\n",
		 lane_rate_khz, refclk_khz);

	return -EINVAL;
}

/*******************************************************************************
 * @brief Read CPLL configuration for GTH transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - CPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth34_cpll_read_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, 0x28, &val);
	if (ret)
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
	if (ret)
		return ret;

	if (val & 0xf800)
		conf->refclk_div = 1;
	else
		conf->refclk_div = 2;

	pr_debug("%s: cpll: fb_div_N1=%"PRIu32"\ncpll: fb_div_N2=%"PRIu32"\ncpll:"
		 " refclk_div=%"PRIu32"\n", __func__, conf->fb_div_N1,
		 conf->fb_div_N2, conf->refclk_div);

	return 0;
}

/*******************************************************************************
 * @brief Read CPLL configuration for GTX transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - CPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_gtx2_cpll_read_config(struct xilinx_xcvr *xcvr,
				      uint32_t drp_port, struct xilinx_xcvr_cpll_config *conf)
{
	uint32_t val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, CPLL_REFCLK_DIV_M_ADDR, &val);
	if (ret)
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

	return 0;
}

/*******************************************************************************
 * @brief Read CPLL configuration.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - CPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_cpll_read_config(struct xilinx_xcvr *xcvr,
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
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Write CPLL configuration for GTH transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - CPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth34_cpll_write_config(struct xilinx_xcvr *xcvr,
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
		return -EINVAL;
	}

	val <<= 8;

	switch (conf->fb_div_N1) {
	case 4:
		break;
	case 5:
		val |= CPLL_FB_DIV_45_N1_MASK;
		break;
	default:
		return -EINVAL;
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
		return -EINVAL;
	}

	val <<= 11;

	return xilinx_xcvr_drp_update(xcvr, drp_port, 0x2a,
				      0xf800, val);
}

/*******************************************************************************
 * @brief Write CPLL configuration for GTX transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - CPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gtx2_cpll_write_config(struct xilinx_xcvr *xcvr,
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
		return -EINVAL;
	}

	switch (conf->fb_div_N2) {
	case 1:
		val |= 0x10;
	/* fall-through */
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
		return -EINVAL;
	}

	switch (conf->refclk_div) {
	case 1:
		val |= CPLL_REFCLK_DIV_M_MASK;
		break;
	case 2:
		break;
	default:
		return -EINVAL;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port, CPLL_REFCLK_DIV_M_ADDR,
				      CPLL_REFCLK_DIV_M_MASK | CPLL_FB_DIV_45_N1_MASK | CPLL_FBDIV_N2_MASK,
				      val);
}

/*******************************************************************************
 * @brief Write CPLL configuration.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - CPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_cpll_write_config(struct xilinx_xcvr *xcvr,
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
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Calculate line rate for CPLL.
 *
 * @param xcvr - The device structure.
 * @param refclk_hz - Reference clock (kHz).
 * @param conf - CPLL configuration values.
 * @param out_div - Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_cpll_calc_lane_rate(struct xilinx_xcvr *xcvr,
				    uint32_t refclk_hz, const struct xilinx_xcvr_cpll_config *conf,
				    uint32_t out_div)
{
	if (conf->refclk_div == 0 || out_div == 0)
		return 0;

	return NO_OS_DIV_ROUND_CLOSEST_ULL((uint64_t)refclk_hz *
					   conf->fb_div_N1 * conf->fb_div_N2 * 2,
					   conf->refclk_div * out_div * 1000);
}

/*******************************************************************************
 * @brief Read QPLL configuration for GTH transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - QPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth34_qpll_read_config(struct xilinx_xcvr *xcvr,
		unsigned int sys_clk_sel, uint32_t drp_port,
		struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, GTY4_QPLL_CLKOUT_RATE(xcvr,
				   sys_clk_sel), &val);
	if (ret)
		return ret;

	conf->qty4_full_rate = val & 0x1;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port,
				   GTH34_QPLL_REFCLK_DIV(xcvr, sys_clk_sel), &val);
	if (ret)
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

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, GTH34_QPLL_FBDIV(xcvr, sys_clk_sel),
				   &val);
	if (ret)
		return ret;

	conf->fb_div = (val & 0xff) + 2;

	conf->band = 0;

	pr_debug("%s: qpll: fb_div=%"PRIu32", qpll: refclk_div=%"PRIu32"\n",
		 __func__, conf->fb_div, conf->refclk_div);

	return 0;
}


/*******************************************************************************
 * @brief Read QPLL configuration for GTX transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - QPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gtx2_qpll_read_config(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, struct xilinx_xcvr_qpll_config *conf)
{
	uint32_t val;
	int ret;

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, QPLL_CFG1_ADDR, &val);
	if (ret)
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
	if (ret)
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
	if (ret)
		return ret;

	if (val & QPLL_CFG0_LOWBAND_MASK)
		conf->band = 0;
	else
		conf->band = 1;

	return 0;
}

/*******************************************************************************
 * @brief Read QPLL configuration.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param sys_clk_sel  - QPLL0 (3) / QPLL1 (2) selection.
 * @param conf - QPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_qpll_read_config(struct xilinx_xcvr *xcvr,
				 uint32_t drp_port, uint32_t sys_clk_sel,
				 struct xilinx_xcvr_qpll_config *conf)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_qpll_read_config(xcvr, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_qpll_read_config(xcvr, sys_clk_sel, drp_port, conf);
	default:
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Write QPLL configuration for GTH transceiver.
 *
 * @param xcvr - The device structure.
 * @param sys_clk_sel - QPLL0 (3) / QPLL1 (2) selection.
 * @param drp_port - DRP selection.
 * @param conf - QPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth34_qpll_write_config(struct xilinx_xcvr *xcvr,
		uint32_t sys_clk_sel, uint32_t drp_port,
		const struct xilinx_xcvr_qpll_config *conf)
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
		pr_debug("%s: Invalid refclk divider: %"PRIu32"\n",
			 __func__, conf->refclk_div);
		return -EINVAL;
	}

	ret = xilinx_xcvr_drp_update(xcvr, drp_port,
				     GTH34_QPLL_FBDIV(xcvr, sys_clk_sel), 0xff, fbdiv);
	if (ret < 0)
		return ret;

	if (xcvr->type == XILINX_XCVR_TYPE_US_GTY4) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port,
					     GTY4_QPLL_CLKOUT_RATE(xcvr, sys_clk_sel),
					     0x1, conf->qty4_full_rate);
		if (ret < 0)
			return ret;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port,
				      GTH34_QPLL_REFCLK_DIV(xcvr, sys_clk_sel),
				      0xf80, refclk << 7);
}


/*******************************************************************************
 * @brief Write QPLL configuration for GTX transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param conf - QPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gtx2_qpll_write_config(struct xilinx_xcvr *xcvr,
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
		pr_debug("%s: Invalid refclk divider: %"PRIu32"\n",
			 __func__, conf->refclk_div);
		return -EINVAL;
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
		pr_debug("%s: Invalid feedback divider: %"PRIu32"\n",
			 __func__, conf->fb_div);
		return -EINVAL;
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

	return 0;
}

/*******************************************************************************
 * @brief Write QPLL configuration.
 *
 * @param xcvr - The device structure.
 * @param sys_clk_sel  - QPLL0 (3) / QPLL1 (2) selection.
 * @param drp_port - DRP selection.
 * @param conf - QPLL configuration values.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_qpll_write_config(struct xilinx_xcvr *xcvr,
				  uint32_t sys_clk_sel,
				  uint32_t drp_port, const struct xilinx_xcvr_qpll_config *conf)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return xilinx_xcvr_gtx2_qpll_write_config(xcvr, drp_port, conf);
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_qpll_write_config(xcvr, sys_clk_sel,
				drp_port, conf);
	default:
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Calculate line rate for QPLL.
 *
 * @param xcvr - The device structure.
 * @param refclk_hz - Reference clock (Hz).
 * @param conf - CPLL configuration values.
 * @param out_div - Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_qpll_calc_lane_rate(struct xilinx_xcvr *xcvr,
				    uint32_t refclk_hz, const struct xilinx_xcvr_qpll_config *conf,
				    uint32_t out_div)
{
	if (conf->refclk_div == 0 || out_div == 0)
		return 0;

	if (conf->qty4_full_rate == 1)
		refclk_hz *= 2;

	return NO_OS_DIV_ROUND_CLOSEST_ULL((uint64_t)refclk_hz * conf->fb_div,
					   conf->refclk_div * out_div * 1000);
}

/*******************************************************************************
 * @brief Read the Output clock divider for GTH transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_out_div - Rx Output clock divider.
 * @param rx_out_div - Tx Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth34_read_out_div(struct xilinx_xcvr *xcvr,
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

	return 0;
}

/*******************************************************************************
 * @brief Read the Output clock divider for GTX transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_out_div - Rx Output clock divider.
 * @param rx_out_div - Tx Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gtx2_read_out_div(struct xilinx_xcvr *xcvr,
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

	return 0;
}

/*******************************************************************************
 * @brief Read the Output clock divider.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_out_div - Rx Output clock divider.
 * @param rx_out_div - Tx Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_read_out_div(struct xilinx_xcvr *xcvr, uint32_t drp_port,
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
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Convert Output clock divider to value.
 *
 * @param out_div - Output clock divider.
 *
 * @return        - Value
*******************************************************************************/
static uint32_t xilinx_xcvr_out_div_to_val(uint32_t out_div)
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

/*******************************************************************************
 * @brief Write the Output clock divider for GTH transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_out_div - Rx Output clock divider.
 * @param rx_out_div - Tx Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth34_write_out_div(struct xilinx_xcvr *xcvr,
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

	return 0;
}

/*******************************************************************************
 * @brief Write the Output clock divider for GTX transceiver.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_out_div - Rx Output clock divider.
 * @param rx_out_div - Tx Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gtx2_write_out_div(struct xilinx_xcvr *xcvr,
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


/*******************************************************************************
 * @brief Write the Output clock divider.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_out_div - Rx Output clock divider.
 * @param rx_out_div - Tx Output clock divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_write_out_div(struct xilinx_xcvr *xcvr, uint32_t drp_port,
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
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Convert programmable divider ratio for GTY4 and GTH4 transceivers
 *        to value.
 *
 * @param div - Programmable divider ratio.
 *
 * @return - Value
*******************************************************************************/
static uint32_t xilinx_xcvr_gty4_gth4_prog_div_to_val(uint32_t div)
{
	switch (div) {
	case 0:
		return 32768;
	case 4:
		return 57432;
	case 5:
		return 57464;
	case 8:
		return 57408;
	case 10:
		return 57440;
	case 16:
		return 57410;
	case 17: /* This is 16.5 rounded to 17 */
		return 57880;
	case 20:
		return 57442;
	case 32:
		return 57414;
	case 33:
		return 57856;
	case 40:
		return 57415;
	case 64:
		return 57422;
	case 66:
		return 57858;
	case 80:
		return 57423;
	case 100:
		return 57455;
	case 128:
		return 24654;
	case 132:
		return 57862;
	default:
		return 32768; /* 0 disabled */
	}
}

/*******************************************************************************
 * @brief Convert programmable divider ratio for GTY3 and GTH3 transceivers
 *        to value.
 *
 * @param div - Programmable divider ratio.
 *
 * @return - Value
*******************************************************************************/
static uint32_t xilinx_xcvr_gty3_gth3_prog_div_to_val(uint32_t div)
{
	switch (div) {
	case 0:
		return 32768;
	case 4:
		return 57744;
	case 5:
		return 49648;
	case 8:
		return 57728;
	case 10:
		return 57760;
	case 16:
		return 57730;
	case 17: /* This is 16.5 rounded to 17 */
		return 49672;
	case 20:
		return 57762;
	case 32:
		return 57734;
	case 33:
		return 49800;
	case 40:
		return 57766;
	case 64:
		return 57742;
	case 66:
		return 50056;
	case 80:
		return 57743;
	case 100:
		return 57775;
	default:
		return 32768; /* 0 disabled */
	}
}

/*******************************************************************************
 * @brief Write RX/TX programmable divider ratio for GTH3 and GTY3 transceivers.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_prog_div - RX programmable divider ratio.
 * @param tx_prog_div - TX programmable divider ratio.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth3_gty3_write_progdiv_div(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, int32_t rx_prog_div, int32_t tx_prog_div)
{
	int ret;

	if (rx_prog_div >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0xC6, 0xFFFF,
					     xilinx_xcvr_gty3_gth3_prog_div_to_val(rx_prog_div));
		if (ret)
			return ret;
	}
	if (tx_prog_div >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0x3E, 0xFFFF,
					     xilinx_xcvr_gty3_gth3_prog_div_to_val(tx_prog_div));
		if (ret)
			return ret;
	}

	return 0;
}

/*******************************************************************************
 * @brief Write RX/TX programmable divider ratio for GTH4 and GTY4 transceivers.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_prog_div - RX programmable divider ratio.
 * @param tx_prog_div - TX programmable divider ratio.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth4_gty4_write_progdiv_div(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, int32_t rx_prog_div, int32_t tx_prog_div)
{
	int ret;

	if (rx_prog_div >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0xC6, 0xFFFF,
					     xilinx_xcvr_gty4_gth4_prog_div_to_val(rx_prog_div));
		if (ret)
			return ret;
	}
	if (tx_prog_div >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port,
					     (xcvr->type == XILINX_XCVR_TYPE_US_GTY4) ? 0x57 : 0x3E,
					     0xFFFF,
					     xilinx_xcvr_gty4_gth4_prog_div_to_val(tx_prog_div));
		if (ret)
			return ret;
	}

	return 0;
}

/*******************************************************************************
 * @brief Write RX/TX programmable divider ratio.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_prog_div - RX programmable divider ratio.
 * @param tx_prog_div - TX programmable divider ratio.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_write_prog_div(struct xilinx_xcvr *xcvr, uint32_t drp_port,
			       int32_t rx_prog_div, int32_t tx_prog_div)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_US_GTH3:
		return xilinx_xcvr_gth3_gty3_write_progdiv_div(xcvr, drp_port,
				rx_prog_div, tx_prog_div);
	case XILINX_XCVR_TYPE_US_GTY4:
	case XILINX_XCVR_TYPE_US_GTH4:
		return xilinx_xcvr_gth4_gty4_write_progdiv_div(xcvr, drp_port,
				rx_prog_div, tx_prog_div);
	default:
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Convert pre-divider to value.
 *
 * @param rate - Pre-divider.
 *
 * @return - Value
*******************************************************************************/
static uint32_t xilinx_xcvr_prog_div_rate_to_val(uint32_t rate)
{
	switch (rate) {
	case 1:
		return 1;
	case 2:
		return 0;
	default:
		return 0;
	}
}


/*******************************************************************************
 * @brief Write pre-divider ratio for GTY4 transceivers.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_rate - RX pre-divider.
 * @param tx_rate - TX pre-divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gty4_write_progdiv_div_rate(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, int32_t rx_rate, int32_t tx_rate)
{
	int ret;

	if (rx_rate >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0x103, 0x1,
					     xilinx_xcvr_prog_div_rate_to_val(rx_rate));
		if (ret)
			return ret;
	}
	if (tx_rate >= 0) {
		ret = xilinx_xcvr_drp_update(xcvr, drp_port, 0x105, 0x1,
					     xilinx_xcvr_prog_div_rate_to_val(tx_rate));
		if (ret)
			return ret;
	}

	return 0;
}

/*******************************************************************************
 * @brief Write pre-divider ratio.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param rx_rate - RX pre-divider.
 * @param tx_rate - TX pre-divider.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_write_prog_div_rate(struct xilinx_xcvr *xcvr,
				    uint32_t drp_port, int32_t rx_rate, int32_t tx_rate)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gty4_write_progdiv_div_rate(xcvr, drp_port,
				rx_rate, tx_rate);
	default:
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Write asynchronous gearbox enable for GTH34 transceivers.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param en - Enable
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
static int xilinx_xcvr_gth34_write_async_gearbox_en(struct xilinx_xcvr *xcvr,
		uint32_t drp_port, bool en)
{
	uint32_t mask;

	mask = NO_OS_BIT(13) | NO_OS_BIT(7); /* TXGEARBOX_EN | TXBUF_EN */

	return xilinx_xcvr_drp_update(xcvr, drp_port,
				      0x7c, mask, en ? mask : 0);
}

/*******************************************************************************
 * @brief Write asynchronous gearbox enable.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param en - Enable
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_write_async_gearbox_en(struct xilinx_xcvr *xcvr,
				       uint32_t drp_port, bool en)
{
	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		return 0;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		return xilinx_xcvr_gth34_write_async_gearbox_en(xcvr,
				drp_port, en);
	default:
		return -EINVAL;
	}
}

/*******************************************************************************
 * @brief Write RX_CLK25_DIV value (divider for RXPLLREFCLK_DIV1 to generate
 *        an internal clock used for some of the logic inside the RX
 *        transceiver part).
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param div - Divider value.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_write_rx_clk25_div(struct xilinx_xcvr *xcvr,
				   uint32_t drp_port, uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return -EINVAL;

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
		return -EINVAL;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port, reg, mask, div);
}

/*******************************************************************************
 * @brief Write TX_CLK25_DIV value (divider for TXPLLREFCLK_DIV1 to generate
 *        an internal clock used for some of the logic inside the TRX
 *        transceiver part).
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param div - Divider value.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_write_tx_clk25_div(struct xilinx_xcvr *xcvr,
				   uint32_t drp_port, uint32_t div)
{
	uint32_t reg, mask;

	if (div == 0 || div > 32)
		return -EINVAL;

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
		return -EINVAL;
	}

	return xilinx_xcvr_drp_update(xcvr, drp_port, reg, mask, div);
}

/*******************************************************************************
 * @brief Get PRBS generator test pattern control setting.
 *
 * @param xcvr - The device structure.
 * @param prbs - Pseudo-random bit sequence.
 * @param reverse_lu - Reverse enabled.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_prbsel_enc_get(struct xilinx_xcvr *xcvr, uint32_t prbs,
			       bool reverse_lu)
{
	const uint8_t gthy_prbs_lut[] = {0, 7, 9, 15, 23, 31};
	const uint8_t gtx_prbs_lut[] = {0, 7, 15, 23, 31};
	unsigned int i;

	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		if (reverse_lu && prbs < NO_OS_ARRAY_SIZE(gtx_prbs_lut))
			return gtx_prbs_lut[prbs];

		for (i = 0; i < NO_OS_ARRAY_SIZE(gtx_prbs_lut); i++)
			if (gtx_prbs_lut[i] == prbs)
				return i;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		if (reverse_lu && prbs < NO_OS_ARRAY_SIZE(gthy_prbs_lut))
			return gthy_prbs_lut[prbs];

		for (i = 0; i < NO_OS_ARRAY_SIZE(gthy_prbs_lut); i++)
			if (gthy_prbs_lut[i] == prbs)
				return i;
		break;
	default:
		return -EINVAL;
	}

	return -EINVAL;
}


/*******************************************************************************
 * @brief Get PRBS error counter value.
 *
 * @param xcvr - The device structure.
 * @param drp_port - DRP selection.
 * @param cnt - Counter value.
 *
 * @return ret - Result of the operation (0 - success, negative value
 *               for failure).
*******************************************************************************/
int xilinx_xcvr_prbs_err_cnt_get(struct xilinx_xcvr *xcvr,
				 uint32_t drp_port, uint32_t *cnt)
{
	uint32_t addr;
	uint32_t val, val2 = 0;
	int ret;

	switch (xcvr->type) {
	case XILINX_XCVR_TYPE_S7_GTX2:
		addr = GTX_RX_PRBS_ERR_CNT;
		break;
	case XILINX_XCVR_TYPE_US_GTH3:
		addr = GTH3_RX_PRBS_ERR_CNT;
		break;
	case XILINX_XCVR_TYPE_US_GTH4:
	case XILINX_XCVR_TYPE_US_GTY4:
		addr = GTH4_RX_PRBS_ERR_CNT;
		break;
	default:
		return -EINVAL;
	}

	ret = xilinx_xcvr_drp_read(xcvr, drp_port, addr, &val);
	if (ret)
		return ret;

	if (xcvr->type != XILINX_XCVR_TYPE_S7_GTX2) {
		ret = xilinx_xcvr_drp_read(xcvr, drp_port, addr + 1, &val2);
		if (ret)
			return ret;
	}

	*cnt = ((val2 & 0xFFFF) << 16) | (val & 0xFFFF);

	return 0;
}
