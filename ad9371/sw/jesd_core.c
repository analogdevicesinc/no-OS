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

#define XCVR_REG_CM_SEL			0x0040

#define XCVR_REG_CM_CONTROL		0x0044
#define XCVR_CM_WR				(1 << 28)
#define XCVR_CM_ADDR(x)			(((x) & 0xFFF) << 16)
#define XCVR_CM_WDATA(x)		(((x) & 0xFFFF) << 0)

#define XCVR_REG_CM_STATUS		0x0048
#define XCVR_CM_BUSY			(1 << 16)
#define XCVR_CM_RDATA(x)		(((x) & 0xFFFF) << 0)

#define XCVR_REG_CH_SEL			0x0060

#define XCVR_REG_CH_CONTROL		0x0064
#define XCVR_CH_WR				(1 << 28)
#define XCVR_CH_ADDR(x)			(((x) & 0xFFF) << 16)
#define XCVR_CH_WDATA(x)		(((x) & 0xFFFF) << 0)

#define XCVR_REG_CH_STATUS		0x0068
#define XCVR_CH_BUSY			(1 << 16)
#define XCVR_CH_RDATA(x)		(((x) & 0xFFFF) << 0)

#define XCVR_BROADCAST			0xff

#define TXOUT_DIV_ADDR				0x88
#define TXOUT_DIV_MASK				0x70
#define TXOUT_DIV_OFFSET			0x4
#define TXOUT_DIV_WIDTH				0x3
#define TXOUT_DIV_DEFAULT			0x0

#define RXOUT_DIV_ADDR				0x88
#define RXOUT_DIV_MASK				0x7
#define RXOUT_DIV_OFFSET			0x0
#define RXOUT_DIV_WIDTH				0x3
#define RXOUT_DIV_DEFAULT			0x0

#define RXCDR_CFG0_ADDR				0xa8
#define RXCDR_CFG0_MASK				0xffff
#define RXCDR_CFG0_OFFSET			0x0
#define RXCDR_CFG0_WIDTH			0x10
#define RXCDR_CFG0_DEFAULT			0x0

#define RXCDR_CFG1_ADDR				0xa9
#define RXCDR_CFG1_MASK				0xffff
#define RXCDR_CFG1_OFFSET			0x0
#define RXCDR_CFG1_WIDTH			0x10
#define RXCDR_CFG1_DEFAULT			0x0

#define RXCDR_CFG2_ADDR				0xaa
#define RXCDR_CFG2_MASK				0xffff
#define RXCDR_CFG2_OFFSET			0x0
#define RXCDR_CFG2_WIDTH			0x10
#define RXCDR_CFG2_DEFAULT			0x0

#define RXCDR_CFG3_ADDR				0xab
#define RXCDR_CFG3_MASK				0xffff
#define RXCDR_CFG3_OFFSET			0x0
#define RXCDR_CFG3_WIDTH			0x10
#define RXCDR_CFG3_DEFAULT			0x0

#define RXCDR_CFG4_ADDR				0xac
#define RXCDR_CFG4_MASK				0xff
#define RXCDR_CFG4_OFFSET			0x0
#define RXCDR_CFG4_WIDTH			0x8
#define RXCDR_CFG4_DEFAULT			0x0

#define RX_DFE_LPM_CFG_ADDR			0x29
#define RX_DFE_LPM_CFG_MASK			0xffff
#define RX_DFE_LPM_CFG_OFFSET		0x0
#define RX_DFE_LPM_CFG_WIDTH		0x10
#define RX_DFE_LPM_CFG_DEFAULT		0x0

#define QPLL_CFG0_ADDR				0x32
#define QPLL_CFG0_MASK				0xffff
#define QPLL_CFG0_BAND_MASK			0x40
#define QPLL_CFG0_OFFSET			0x0
#define QPLL_CFG0_WIDTH				0x10
#define QPLL_CFG0_DEFAULT			0x0

#define QPLL_CFG1_ADDR				0x33
#define QPLL_CFG1_MASK				0x7ff
#define QPLL_CFG1_OFFSET			0x0
#define QPLL_CFG1_WIDTH				0xb
#define QPLL_CFG1_DEFAULT			0x0

#define QPLL_REFCLK_DIV_M_ADDR		0x33
#define QPLL_REFCLK_DIV_M_MASK		0xf800
#define QPLL_REFCLK_DIV_M_OFFSET	0xb
#define QPLL_REFCLK_DIV_M_WIDTH		0x5
#define QPLL_REFCLK_DIV_M_DEFAULT	0x0

#define QPLL_FBDIV_N_ADDR			0x36
#define QPLL_FBDIV_N_MASK			0x3ff
#define QPLL_FBDIV_N_OFFSET			0x0
#define QPLL_FBDIV_N_WIDTH			0xa
#define QPLL_FBDIV_N_DEFAULT		0x0

#define QPLL_FBDIV_RATIO_ADDR		0x37
#define QPLL_FBDIV_RATIO_MASK		0x40
#define QPLL_FBDIV_RATIO_OFFSET		0x6
#define QPLL_FBDIV_RATIO_WIDTH		0x1
#define QPLL_FBDIV_RATIO_DEFAULT	0x0

#define CPLL_CFG0_ADDR				0x5c
#define CPLL_CFG0_MASK				0xff00
#define CPLL_CFG0_OFFSET			0x8
#define CPLL_CFG0_WIDTH				0x8
#define CPLL_CFG0_DEFAULT			0x0

#define CPLL_CFG1_ADDR				0x5d
#define CPLL_CFG1_MASK				0xffff
#define CPLL_CFG1_OFFSET			0x0
#define CPLL_CFG1_WIDTH				0x10
#define CPLL_CFG1_DEFAULT			0x0

#define CPLL_REFCLK_DIV_M_ADDR		0x5e
#define CPLL_REFCLK_DIV_M_MASK		0x1f00
#define CPLL_REFCLK_DIV_M_OFFSET	0x8
#define CPLL_REFCLK_DIV_M_WIDTH		0x5
#define CPLL_REFCLK_DIV_M_DEFAULT	0x0

#define CPLL_FB_DIV_45_N1_ADDR		0x5e
#define CPLL_FB_DIV_45_N1_MASK		0x80
#define CPLL_FB_DIV_45_N1_OFFSET	0x7
#define CPLL_FB_DIV_45_N1_WIDTH		0x1
#define CPLL_FB_DIV_45_N1_DEFAULT	0x0

#define CPLL_FBDIV_N2_ADDR			0x5e
#define CPLL_FBDIV_N2_MASK			0x7f
#define CPLL_FBDIV_N2_OFFSET		0x0
#define CPLL_FBDIV_N2_WIDTH			0x7
#define CPLL_FBDIV_N2_DEFAULT		0x0

#define ENC_8B10B					810

#define JESD_REG_TRX_VERSION				0x00
#define JESD_REG_TRX_RESET					0x04
#define JESD_REG_TRX_ILA_SUPPORT			0x08
#define JESD_REG_TRX_SCRAMBLING				0x0c
#define JESD_REG_TRX_SYSREF_HANDLING		0x10
#define JESD_REG_TX_ILA_MULTIFRAMES			0x14
#define JESD_REG_TRX_TEST_MODES				0x18
#define JESD_REG_RX_LINK_ERROR_STATUS		0x1c
#define JESD_REG_TRX_OCTETS_PER_FRAME		0x20
#define JESD_REG_TRX_FRAMES_PER_MULTIFRAME	0x24
#define JESD_REG_TRX_LANES_IN_USE			0x28
#define JESD_REG_TRX_SUBCLASS_MODE			0x2c
#define JESD_REG_TRX_SYNC_STATUS			0x38

/***************************************************************************//**
 * @brief xcvr_write
 *******************************************************************************/
int32_t xcvr_write(xcvr_device dev,
				   uint32_t reg_addr,
				   uint32_t reg_val)
{
	Xil_Out32((dev.base_addr + reg_addr), reg_val);

	return 0;
}

/***************************************************************************//**
 * @brief xcvr_read
 *******************************************************************************/
int32_t xcvr_read(xcvr_device dev,
				  uint32_t reg_addr,
				  uint32_t *reg_val)
{
	*reg_val = Xil_In32(dev.base_addr + reg_addr);

	return 0;
}

/***************************************************************************//**
* @brief xcvr_drp_read
*******************************************************************************/
int32_t xcvr_drp_read(xcvr_device dev,
					  uint32_t reg_addr,
					  uint32_t *reg_data)
{
	int8_t ch_sel;
	int8_t timeout = 20;
	uint32_t val;

	switch (reg_addr) {
	case QPLL_CFG0_ADDR:
	case QPLL_CFG1_ADDR:
	case QPLL_FBDIV_N_ADDR:
	case QPLL_FBDIV_RATIO_ADDR:
		ch_sel = 0;
		break;
	default:
		ch_sel = 1;
	}

	xcvr_write(dev, ch_sel ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL, XCVR_BROADCAST);
	xcvr_write(dev, ch_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL, XCVR_CM_ADDR(reg_addr));
	xcvr_write(dev, ch_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL,
				 ch_sel ? XCVR_CH_ADDR(reg_addr) : XCVR_CM_ADDR(reg_addr));
	do {
		xcvr_read(dev, ch_sel ? XCVR_REG_CH_STATUS : XCVR_REG_CM_STATUS, &val);
		if (val & (ch_sel ? XCVR_CH_BUSY : XCVR_CM_BUSY)) {
			mdelay(1);
			continue;
		}
		*reg_data = ch_sel ? XCVR_CH_RDATA(val) : XCVR_CM_RDATA(val);
		return 0;
	} while (timeout--);

	printf("%s: Timeout!\n", __func__);

	return -1;
}

/***************************************************************************//**
* @brief xcvr_drp_write
*******************************************************************************/
int32_t xcvr_drp_write(xcvr_device dev,
					   uint32_t reg_addr,
					   uint32_t reg_data)
{
	int8_t ch_sel;
	int8_t timeout = 20;
	uint32_t val;
	uint32_t val_reg;

	switch (reg_addr) {
	case QPLL_CFG0_ADDR:
	case QPLL_CFG1_ADDR:
	case QPLL_FBDIV_N_ADDR:
	case QPLL_FBDIV_RATIO_ADDR:
		ch_sel = 0;
		break;
	default:
		ch_sel = 1;
	}

	xcvr_write(dev, ch_sel ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL, XCVR_BROADCAST);
	xcvr_write(dev, ch_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL,
			ch_sel ? (XCVR_CH_WR | XCVR_CH_ADDR(reg_addr) | XCVR_CH_WDATA(reg_data)) :
			(XCVR_CM_WR | XCVR_CM_ADDR(reg_addr) | XCVR_CM_WDATA(reg_data)));

	do {
		xcvr_read(dev, ch_sel ? XCVR_REG_CH_STATUS : XCVR_REG_CM_STATUS, &val);
		if (!(val & (ch_sel ? XCVR_CH_BUSY : XCVR_CM_BUSY))) {
			xcvr_drp_read(dev, reg_addr, &val_reg);
			if (val != val_reg)
				printf("%s: MISMATCH reg 0x%"PRIX32" val 0x%"PRIX32" != read 0x%"PRIX32"\n",
						__func__, reg_addr, val, val_reg);
			return 0;
		}
		mdelay(1);
	} while (timeout--);

	printf("%s: Timeout!\n", __func__);

	return -1;
}

/***************************************************************************//**
* @brief __xcvr_drp_writef
*******************************************************************************/
int32_t __xcvr_drp_writef(xcvr_device dev,
						  uint32_t reg,
						  uint32_t mask,
						  uint32_t offset,
						  uint32_t val)
{
	uint32_t tmp;
	uint32_t reg_data;
	int32_t ret;

	if (!mask)
		return -1;

	ret = xcvr_drp_read(dev, reg, &reg_data);
	if (ret < 0)
		return ret;

	tmp = reg_data;

	tmp &= ~mask;
	tmp |= ((val << offset) & mask);

	return xcvr_drp_write(dev, reg, tmp);
}

/***************************************************************************//**
* @brief adxcvr_drp_writef
*******************************************************************************/
#define xcvr_drp_writef(st, reg, mask, val) \
	__xcvr_drp_writef  (st, reg, mask, find_first_bit(mask), val)

/***************************************************************************//**
* @brief xcvr_calc_cpll_settings
*******************************************************************************/
int32_t xcvr_calc_cpll_settings(xcvr_device dev,
								uint32_t refclk_kHz,
								uint32_t laneRate_kHz,
								uint32_t *refclk_div, uint32_t *out_div,
								uint32_t *fbdiv_45, uint32_t *fbdiv)
{
	uint32_t n1, n2, d, m;
	uint32_t pllFreq_kHz;

	/* Possible Xilinx GTX PLL parameters for Virtex 7 CPLL.  Find one that works for the desired laneRate. */
	/* Attribute encoding, DRP encoding */
	const uint8_t N1[][2] = {{5, 1}, {4, 0} };
	const uint8_t N2[][2] = {{5, 3}, {4, 2}, {3, 1}, {2, 0}, {1, 16} };
	const uint8_t D[][2] = {{1, 0}, {2, 1}, {4, 2}, {8, 3} };
	const uint8_t M[][2] = {{1, 16}, {2, 0} };

	for (m = 0; m < ARRAY_SIZE(M); m++) {
		for (d = 0; d < ARRAY_SIZE(D); d++) {
			for (n1 = 0; n1 < ARRAY_SIZE(N1); n1++) {
				for (n2 = 0; n2 < ARRAY_SIZE(N2); n2++) {
					pllFreq_kHz = refclk_kHz * N1[n1][0] * N2[n2][0] / M[m][0];

					if ((pllFreq_kHz > 3300000) || (pllFreq_kHz < 1600000)) /* GTH 3.75 GHz */
						continue;

					if ((pllFreq_kHz * 2 / D[d][0]) == laneRate_kHz) {
						if (refclk_div && out_div && fbdiv_45 && fbdiv) {
							*refclk_div = M[m][1];
							*out_div = D[d][1];
							*fbdiv_45 = N1[n1][1];
							*fbdiv = N2[n2][1];
						}

						return laneRate_kHz;
					}
				}
			}
		}
	}

	printf("%s: Failed to find matching dividers for %lu kHz rate\n",
		__func__, laneRate_kHz);

	return -1;
}

/***************************************************************************//**
* @brief xcvr_calc_qpll_settings
*******************************************************************************/
int32_t xcvr_calc_qpll_settings(xcvr_device dev,
								uint32_t refclk_kHz,
								uint32_t laneRate_kHz,
								uint32_t *refclk_div, uint32_t *out_div,
								uint32_t *fbdiv, uint32_t *fbdiv_ratio,
								uint32_t *lowband)
{
	/* Calculate the FPGA GTX PLL settings M, D, N1, N2 */
	uint32_t n, d, m;
	uint32_t pllVcoFreq_kHz;
	uint32_t pllOutFreq_kHz;

	/* Possible Xilinx GTX QPLL parameters for Virtex 7 QPLL.  Find one that works for the desired laneRate. */
	/* Attribute encoding, DRP encoding */
	const uint16_t N[][2] = {{16, 32}, {20, 48}, {32, 96}, {40, 128},
			     {64, 224}, {66, 320}, {80, 288}, {100, 368} };
	const uint8_t D[][2] = {{1, 0}, {2, 1}, {4, 2}, {8, 3}, {16, 4} };
	const uint8_t M[][2] = {{1, 16}, {2, 0}, {3, 1}, {4, 2} };
	uint8_t _lowBand = 0;

	for (m = 0; m < ARRAY_SIZE(M); m++) {
		for (d = 0; d < ARRAY_SIZE(D); d++) {
			for (n = 0; n < ARRAY_SIZE(N); n++) {

				pllVcoFreq_kHz = refclk_kHz * N[n][0] / M[m][0];
				pllOutFreq_kHz = pllVcoFreq_kHz / 2;

				if ((pllVcoFreq_kHz >= 5930000) && (pllVcoFreq_kHz <= 8000000)) {
					/* low band = 5.93G to 8.0GHz VCO */
					_lowBand = 1;
				} else if ((pllVcoFreq_kHz >= 9800000) && (pllVcoFreq_kHz <= 12500000)) {
					/* high band = 9.8G to 12.5GHz VCO */
					_lowBand = 0;
				} else {
					continue; /* if Pll out of range, not valid case, keep trying */
				}

				if ((pllOutFreq_kHz * 2 / D[d][0]) == laneRate_kHz) {
					if (refclk_div && out_div && fbdiv_ratio && fbdiv && lowband) {
						*refclk_div = M[m][1];
						*out_div = D[d][1];
						*fbdiv = N[n][1];
						*fbdiv_ratio = (N[n][0] == 66) ? 0 : 1;
					}
					if (lowband)
						*lowband = _lowBand;

					return laneRate_kHz;
				}

			}
		}
	}

	printf("%s: Failed to find matching dividers for %lu kHz rate\n",
		__func__, laneRate_kHz);

	return -1;
}

/***************************************************************************//**
* @brief xcvr_gth_rxcdr_settings
*******************************************************************************/
int32_t xcvr_gth_rxcdr_settings(xcvr_device dev,
								uint32_t rxout_div)
{
	uint16_t cfg0, cfg1, cfg2, cfg3, cfg4 = 0;

	if (dev.tx_enable)
		return 0; /* Do Nothing */

	switch (dev.ppm) {
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

	if (dev.encoding == ENC_8B10B) {
		cfg1 = 0xC208;
		cfg3 = 0x07FE;
		cfg3 = 0x0020;

		switch (rxout_div) {
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
	}

	xcvr_drp_write(dev, RXCDR_CFG0_ADDR, cfg0);
	xcvr_drp_write(dev, RXCDR_CFG1_ADDR, cfg1);
	xcvr_drp_write(dev, RXCDR_CFG2_ADDR, cfg2);
	xcvr_drp_write(dev, RXCDR_CFG3_ADDR, cfg3);
	xcvr_drp_write(dev, RXCDR_CFG4_ADDR, cfg4);

	return 0;
}

/***************************************************************************//**
* @brief xcvr_rxcdr_settings
*******************************************************************************/
int32_t xcvr_rxcdr_settings(xcvr_device dev,
							uint32_t rxout_div)
{
	u16 cfg0, cfg1, cfg2, cfg3, cfg4;

	if (dev.tx_enable)
		return 0; /* Do Nothing */

	if (dev.gth_enable)
		return xcvr_gth_rxcdr_settings(dev, rxout_div);

	cfg2 = 0x23FF;
	cfg0 = 0x0020;

	switch (dev.ppm) {
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

	if (dev.encoding == ENC_8B10B) {
		cfg4 = 0x03;

		switch (rxout_div) {
		case 0: /* 1 */
			cfg1 = 0x1040;
			break;
		case 1: /* 2 */
			cfg1 = 0x1020;
			break;
		case 2: /* 4 */
			cfg1 = 0x1010;
			break;
		case 3: /* 8 */
			cfg1 = 0x1008;
			break;
		default:
			return -1;
		}
	} else {
		if (dev.lane_rate_khz > 6600000 && rxout_div == 1)
			cfg4 = 0x0B;
		else
			cfg4 = 0x03;

		switch (rxout_div) {
		case 0: /* 1 */
			if (dev.lpm_enable) {
				if (dev.lane_rate_khz  > 6600000) {
					if (dev.ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					cfg1 = 0x1020;
				}
			} else { /* DFE */
				if (dev.lane_rate_khz  > 6600000) {
					if (dev.ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					if (dev.ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x2040;
				}
			}
			break;
		case 1: /* 2 */
			cfg1 = 0x4020;
			break;
		case 2: /* 4 */
			cfg1 = 0x4010;
			break;
		case 3: /* 8 */
			cfg1 = 0x4008;
			break;
		default:
			return -1;
		}
	}

	xcvr_drp_write(dev, RXCDR_CFG0_ADDR, cfg0);
	xcvr_drp_write(dev, RXCDR_CFG1_ADDR, cfg1);
	xcvr_drp_write(dev, RXCDR_CFG2_ADDR, cfg2);
	xcvr_drp_write(dev, RXCDR_CFG3_ADDR, cfg3);
	xcvr_drp_writef(dev, RXCDR_CFG4_ADDR, RXCDR_CFG4_MASK, cfg4);

	return 0;
}

/***************************************************************************//**
* @brief xcvr_clk_set_rate
*******************************************************************************/
int32_t xcvr_clk_set_rate(xcvr_device dev,
						  uint32_t rate,
						  uint32_t parent_rate)
{
	uint32_t refclk_div, out_div, fbdiv_45, fbdiv, fbdiv_ratio, lowband;
	int32_t ret, pll_done = 0;

	if (dev.cpll_enable)
		ret = xcvr_calc_cpll_settings(dev, parent_rate, rate,
							&refclk_div, &out_div,
							&fbdiv_45, &fbdiv);
	else
		ret = xcvr_calc_qpll_settings(dev, parent_rate, rate,
							&refclk_div, &out_div,
							&fbdiv, &fbdiv_ratio,
							&lowband);
	if (ret < 0)
		return ret;

	xcvr_write(dev, XCVR_REG_RESETN, 0);

	if (dev.cpll_enable) {
		xcvr_drp_writef(dev, CPLL_REFCLK_DIV_M_ADDR,
						CPLL_REFCLK_DIV_M_MASK |
						CPLL_FB_DIV_45_N1_MASK |
						CPLL_FBDIV_N2_MASK,
						(refclk_div << 8) | (fbdiv_45 << 7) | fbdiv);
	} else {
		if (!pll_done) {
			xcvr_drp_writef(dev, QPLL_CFG0_ADDR,
							QPLL_CFG0_BAND_MASK, lowband);

			xcvr_drp_writef(dev, QPLL_REFCLK_DIV_M_ADDR,
							QPLL_REFCLK_DIV_M_MASK, refclk_div);

			xcvr_drp_writef(dev, QPLL_FBDIV_N_ADDR,
							QPLL_FBDIV_N_MASK, fbdiv);

			xcvr_drp_writef(dev, QPLL_FBDIV_RATIO_ADDR,
							QPLL_FBDIV_RATIO_MASK, fbdiv_ratio);
			pll_done = 1;
		}
	}

	ret = xcvr_drp_writef(dev, RXOUT_DIV_ADDR,
			dev.tx_enable ? TXOUT_DIV_MASK : RXOUT_DIV_MASK, out_div);

	xcvr_rxcdr_settings(dev, out_div);

	xcvr_write(dev, XCVR_REG_RESETN, XCVR_RESETN);

	return ret;
}

/***************************************************************************//**
* @brief xcvr_set_lpm_dfe_mode
*******************************************************************************/
int32_t xcvr_set_lpm_dfe_mode(xcvr_device dev,
							  uint32_t lpm)
{
	if (dev.gth_enable) {
		if (lpm) {
			xcvr_drp_write(dev, 0x036, 0x0032);
			xcvr_drp_write(dev, 0x039, 0x1000);
			xcvr_drp_write(dev, 0x062, 0x1980);
		} else {
			xcvr_drp_write(dev, 0x036, 0x0002);
			xcvr_drp_write(dev, 0x039, 0x0000);
			xcvr_drp_write(dev, 0x062, 0x0000);
		}
	} else {
		if (lpm)
			xcvr_drp_write(dev, 0x029, 0x0104);
		else
			xcvr_drp_write(dev, 0x029, 0x0954);
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
	uint32_t	status;
	int32_t		timeout;
	int32_t		ret = 0;

	rx_xcvr.base_addr = XPAR_AXI_AD9371_RX_XCVR_BASEADDR;
	rx_xcvr.ref_rate_khz = myk_dev->clocks->deviceClock_kHz;
	rx_xcvr.lane_rate_khz = myk_dev->clocks->deviceClock_kHz * 40;
	rx_xcvr.cpll_enable = 1;
	rx_xcvr.tx_enable = 0;
	rx_xcvr.gth_enable = 0;
	rx_xcvr.encoding = ENC_8B10B;
	rx_xcvr.ppm = PM_200;
	rx_xcvr.lpm_enable = 1;

	tx_xcvr.base_addr = XPAR_AXI_AD9371_TX_XCVR_BASEADDR;
	tx_xcvr.ref_rate_khz = myk_dev->clocks->deviceClock_kHz;
	tx_xcvr.lane_rate_khz = myk_dev->tx->txProfile->iqRate_kHz * 20;
	tx_xcvr.cpll_enable = 0;
	tx_xcvr.tx_enable = 1;
	tx_xcvr.gth_enable = 0;
	tx_xcvr.encoding = ENC_8B10B;
	tx_xcvr.ppm = PM_200;
	tx_xcvr.lpm_enable = 0;

	rx_os_xcvr.base_addr = XPAR_AXI_AD9371_RX_OS_XCVR_BASEADDR;
	rx_os_xcvr.ref_rate_khz = myk_dev->clocks->deviceClock_kHz;
	rx_os_xcvr.lane_rate_khz = myk_dev->obsRx->orxProfile->iqRate_kHz * 20;
	rx_os_xcvr.cpll_enable = 1;
	rx_os_xcvr.tx_enable = 0;
	rx_os_xcvr.gth_enable = 0;
	rx_os_xcvr.encoding = ENC_8B10B;
	rx_os_xcvr.ppm = PM_200;
	rx_os_xcvr.lpm_enable = 1;

	xcvr_write(rx_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(rx_xcvr, XCVR_REG_CONTROL, XCVR_LPM_DFE_N | XCVR_OUTCLK_SEL(3));
	xcvr_set_lpm_dfe_mode(rx_xcvr, rx_xcvr.lpm_enable);
	xcvr_clk_set_rate(rx_xcvr, rx_xcvr.lane_rate_khz, rx_xcvr.ref_rate_khz);
	xcvr_write(rx_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	xcvr_write(tx_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(tx_xcvr, XCVR_REG_CONTROL, XCVR_SYSCLK_SEL(3) | XCVR_OUTCLK_SEL(3));
	xcvr_clk_set_rate(tx_xcvr, tx_xcvr.lane_rate_khz, tx_xcvr.ref_rate_khz);
	xcvr_write(tx_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	xcvr_write(rx_os_xcvr, XCVR_REG_RESETN, 0);
	xcvr_write(rx_os_xcvr, XCVR_REG_CONTROL, XCVR_LPM_DFE_N | XCVR_OUTCLK_SEL(3));
	xcvr_set_lpm_dfe_mode(rx_os_xcvr, rx_os_xcvr.lpm_enable);
	xcvr_clk_set_rate(rx_os_xcvr, rx_os_xcvr.lane_rate_khz, rx_os_xcvr.ref_rate_khz);
	xcvr_write(rx_os_xcvr, XCVR_REG_RESETN, XCVR_RESETN);

	mdelay(1);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		xcvr_read(rx_xcvr, XCVR_REG_STATUS, &status);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("RX_XCVR initialization failed\n");
		ret--;
	} else
		printf("RX_XCVR initialization OK\n");

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		xcvr_read(tx_xcvr, XCVR_REG_STATUS, &status);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("TX_XCVR initialization failed\n");
		ret--;
	} else
		printf("TX_XCVR initialization OK\n");

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		xcvr_read(rx_os_xcvr, XCVR_REG_STATUS, &status);
		if (status == XCVR_STATUS)
			break;
	}
	if (status == 0) {
		printf("RX_OS_XCVR initialization failed\n");
		ret--;
	} else
		printf("RX_OS_XCVR initialization OK\n");

	return ret;
}

/***************************************************************************//**
 * @brief jesd_write
 *******************************************************************************/
int32_t jesd_write(jesd_device dev,
				   uint32_t reg_addr,
				   uint32_t reg_val)
{
	Xil_Out32((dev.base_addr + reg_addr), reg_val);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_read
 *******************************************************************************/
int32_t jesd_read(jesd_device dev,
				  uint32_t reg_addr,
				  uint32_t *reg_val)
{
	*reg_val = Xil_In32(dev.base_addr + reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_setup
 *******************************************************************************/
int32_t jesd_setup(void)
{
	jesd_device	rx_jesd;
	jesd_device	tx_jesd;
	jesd_device	rx_os_jesd;

	rx_jesd.base_addr = RX_JESD_BASEADDR;
	tx_jesd.base_addr = TX_JESD_BASEADDR;
	rx_os_jesd.base_addr = RX_OS_JESD_BASEADDR;

	jesd_write(rx_jesd, JESD_REG_TRX_RESET, 0x01);
	jesd_write(rx_jesd, JESD_REG_TRX_ILA_SUPPORT, 0x01);
	jesd_write(rx_jesd, JESD_REG_TRX_SCRAMBLING, 0x01);
	jesd_write(rx_jesd, JESD_REG_TRX_SYSREF_HANDLING, 0x00);
	jesd_write(rx_jesd, JESD_REG_TX_ILA_MULTIFRAMES, 0x1f);
	jesd_write(rx_jesd, JESD_REG_TRX_OCTETS_PER_FRAME, 0x03);
	jesd_write(rx_jesd, JESD_REG_TRX_FRAMES_PER_MULTIFRAME, 0x1f);
	jesd_write(rx_jesd, JESD_REG_TRX_SUBCLASS_MODE, 0x01);

	jesd_write(tx_jesd, JESD_REG_TRX_RESET, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_ILA_SUPPORT, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_SCRAMBLING, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_SYSREF_HANDLING, 0x00);
	jesd_write(tx_jesd, JESD_REG_TX_ILA_MULTIFRAMES, 0x1f);
	jesd_write(tx_jesd, JESD_REG_TRX_OCTETS_PER_FRAME, 0x01);
	jesd_write(tx_jesd, JESD_REG_TRX_FRAMES_PER_MULTIFRAME, 0x1f);
	jesd_write(tx_jesd, JESD_REG_TRX_SUBCLASS_MODE, 0x01);

	jesd_write(rx_os_jesd, JESD_REG_TRX_RESET, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_ILA_SUPPORT, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_SCRAMBLING, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_SYSREF_HANDLING, 0x00);
	jesd_write(rx_os_jesd, JESD_REG_TX_ILA_MULTIFRAMES, 0x1f);
	jesd_write(rx_os_jesd, JESD_REG_TRX_OCTETS_PER_FRAME, 0x01);
	jesd_write(rx_os_jesd, JESD_REG_TRX_FRAMES_PER_MULTIFRAME, 0x1f);
	jesd_write(rx_os_jesd, JESD_REG_TRX_SUBCLASS_MODE, 0x01);

	return 0;
}
