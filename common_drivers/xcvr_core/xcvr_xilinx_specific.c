/***************************************************************************//**
* @file xcvr_xilinx_specific.c
* @brief Implementation of XCVR Driver.
* @author DBogdan (dragos.bogdan@analog.com)
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
#ifdef XILINX

#include "xcvr_xilinx_specific.h"
#include "xcvr_core.h"
#include "platform_drivers.h"

#define ARRAY_SIZE(ar)					(sizeof(ar)/sizeof(ar[0]))

/***************************************************************************//**
 * @brief find_first_bit
*******************************************************************************/
uint32_t find_first_bit(uint32_t word)
{
	int32_t num = 0;

	if ((word & 0xffff) == 0) {
			num += 16;
			word >>= 16;
	}
	if ((word & 0xff) == 0) {
			num += 8;
			word >>= 8;
	}
	if ((word & 0xf) == 0) {
			num += 4;
			word >>= 4;
	}
	if ((word & 0x3) == 0) {
			num += 2;
			word >>= 2;
	}
	if ((word & 0x1) == 0)
			num += 1;
	return num;
}

/*******************************************************************************
* @brief xcvr_drp_write
*******************************************************************************/
int32_t xcvr_drp_read(xcvr_core core,
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

	xcvr_write(core, ch_sel ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL, XCVR_BROADCAST);
	xcvr_write(core, ch_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL,
				 XCVR_CM_ADDR(reg_addr));
	xcvr_write(core, ch_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL,
				 ch_sel ? XCVR_CH_ADDR(reg_addr) : XCVR_CM_ADDR(reg_addr));
	do {
		xcvr_read(core, ch_sel ?
						  XCVR_REG_CH_STATUS : XCVR_REG_CM_STATUS, &val);
		if (val & (ch_sel ? XCVR_CH_BUSY : XCVR_CM_BUSY)) {
			mdelay(1);
			continue;
		}
#ifdef DEBUG
		ad_printf("%s: reg 0x%X val 0x%X\n",
				__func__, reg_addr, val & 0xFFFF);
#endif
		*reg_data = ch_sel ? XCVR_CH_RDATA(val) : XCVR_CM_RDATA(val);
		return 0;
	} while (timeout--);

	ad_printf("%s: Timeout!\n", __func__);

	return -1;
}

/*******************************************************************************
* @brief xcvr_drp_write
*******************************************************************************/
int32_t xcvr_drp_write(xcvr_core core,
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

	xcvr_write(core, ch_sel ? XCVR_REG_CH_SEL : XCVR_REG_CM_SEL,
			XCVR_BROADCAST);
	xcvr_write(core, ch_sel ? XCVR_REG_CH_CONTROL : XCVR_REG_CM_CONTROL,
			ch_sel ? (XCVR_CH_WR | XCVR_CH_ADDR(reg_addr) | XCVR_CH_WDATA(reg_data)) :
			(XCVR_CM_WR | XCVR_CM_ADDR(reg_addr) | XCVR_CM_WDATA(reg_data)));

	do {
		xcvr_read(core, ch_sel ? XCVR_REG_CH_STATUS :
				XCVR_REG_CM_STATUS, &val);
		if (!(val & (ch_sel ? XCVR_CH_BUSY : XCVR_CM_BUSY))) {
			xcvr_drp_read(core, reg_addr, &val_reg);
			if (val != val_reg)
				ad_printf("%s: MISMATCH reg 0x%X val 0x%X != read 0x%X\n",
						__func__, reg_addr, val, val_reg);
			return 0;
		}
		mdelay(1);
	} while (timeout--);

	ad_printf("%s: Timeout!\n", __func__);

	return -1;
}

/*******************************************************************************
* @brief xcvr_drp_writef
*******************************************************************************/
int32_t __xcvr_drp_writef(xcvr_core core,
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

	ret = xcvr_drp_read(core, reg, &reg_data);
	if (ret < 0)
		return ret;

	tmp = reg_data;

	tmp &= ~mask;
	tmp |= ((val << offset) & mask);

	return xcvr_drp_write(core, reg, tmp);
}

#define xcvr_drp_writef(st, reg, mask, val) \
	__xcvr_drp_writef  (st, reg, mask, find_first_bit(mask), val)

/*******************************************************************************
* @brief xcvr_calc_cpll_settings
*******************************************************************************/
int32_t xcvr_calc_cpll_settings(xcvr_core core,
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
#ifdef DEBUG
						ad_printf("%s: M %d, D %d, N1 %d, N2 %d\n",
							__func__, M[m][0], D[d][0],
							N1[n1][0], N2[n2][0]);
#endif
						return laneRate_kHz;
					}
				}
			}
		}
	}

	ad_printf("%s: Failed to find matching dividers for %lu kHz rate\n",
		__func__, laneRate_kHz);

	return -1;
}

/*******************************************************************************
* @brief xcvr_calc_qpll_settings
*******************************************************************************/
int32_t xcvr_calc_qpll_settings(xcvr_core core,
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
#ifdef DEBUG
					ad_printf("%s: M %d, D %d, N %d, ratio %d, lowband %d\n",
						__func__, M[m][0], D[d][0],
						N[n][0], (N[n][0] == 66) ? 0 : 1,
						_lowBand);
#endif
					return laneRate_kHz;
				}

			}
		}
	}

	ad_printf("%s: Failed to find matching dividers for %lu kHz rate\n",
		__func__, laneRate_kHz);

	return -1;
}

/*******************************************************************************
* @brief xcvr_gth_rxcdr_settings
*******************************************************************************/
int32_t xcvr_gth_rxcdr_settings(xcvr_core core, uint32_t rxout_div)
{
	uint16_t cfg0, cfg1, cfg2, cfg3, cfg4 = 0;

  cfg2 = 0;

	if (!core.rx_tx_n)
		return 0; /* Do Nothing */

	switch (core.ppm) {
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

	if (core.encoding == ENC_8B10B) {

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
		ad_printf("%s: GTH PRBS CDR not implemented\n", __func__);
	}

	xcvr_drp_write(core, RXCDR_CFG0_ADDR, cfg0);
	xcvr_drp_write(core, RXCDR_CFG1_ADDR, cfg1);
	xcvr_drp_write(core, RXCDR_CFG2_ADDR, cfg2);
	xcvr_drp_write(core, RXCDR_CFG3_ADDR, cfg3);
	xcvr_drp_write(core, RXCDR_CFG4_ADDR, cfg4);

	return 0;
}

/*******************************************************************************
* @brief xcvr_rxcdr_settings
*******************************************************************************/
int32_t xcvr_rxcdr_settings(xcvr_core core,
							  uint32_t rxout_div)
{
	u16 cfg0, cfg1, cfg2, cfg3, cfg4;

	if (!core.rx_tx_n)
		return 0; /* Do Nothing */

	if (core.gth_enable)
		return xcvr_gth_rxcdr_settings(core, rxout_div);

	cfg2 = 0x23FF;
	cfg0 = 0x0020;

	switch (core.ppm) {
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

	if (core.encoding == ENC_8B10B) {

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
		if (core.lane_rate_kbps > 6600000 && rxout_div == 1)
			cfg4 = 0x0B;
		else
			cfg4 = 0x03;

		switch (rxout_div) {
		case 0: /* 1 */
			if (core.lpm_enable) {
				if (core.lane_rate_kbps  > 6600000) {
					if (core.ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					cfg1 = 0x1020;
				}
			} else { /* DFE */
				if (core.lane_rate_kbps  > 6600000) {
					if (core.ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					if (core.ppm == PM_1250)
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

	xcvr_drp_write(core, RXCDR_CFG0_ADDR, cfg0);
	xcvr_drp_write(core, RXCDR_CFG1_ADDR, cfg1);
	xcvr_drp_write(core, RXCDR_CFG2_ADDR, cfg2);
	xcvr_drp_write(core, RXCDR_CFG3_ADDR, cfg3);
	xcvr_drp_writef(core, RXCDR_CFG4_ADDR, RXCDR_CFG4_MASK, cfg4);

	return 0;
}

/*******************************************************************************
* @brief xcvr_rxcdr_settings
*******************************************************************************/
int32_t xcvr_clk_set_rate(xcvr_core core,
							uint32_t rate,
							uint32_t parent_rate)
{
	uint32_t refclk_div, out_div, fbdiv_45, fbdiv, fbdiv_ratio, lowband;
	int32_t ret, pll_done = 0;

#ifdef DEBUG
	ad_printf("%s: Rate %lu Hz Parent Rate %lu Hz\n",
		__func__, rate, parent_rate);
#endif

	if (core.cpll_enable)
		ret = xcvr_calc_cpll_settings(core, parent_rate, rate,
						     &refclk_div, &out_div,
						     &fbdiv_45, &fbdiv);
	else
		ret = xcvr_calc_qpll_settings(core, parent_rate, rate,
						     &refclk_div, &out_div,
						     &fbdiv, &fbdiv_ratio,
						     &lowband);
	if (ret < 0)
		return ret;

	xcvr_write(core, XCVR_REG_RESETN, 0);

	if (core.cpll_enable) {
		xcvr_drp_writef(core, CPLL_REFCLK_DIV_M_ADDR,
						  CPLL_REFCLK_DIV_M_MASK |
						  CPLL_FB_DIV_45_N1_MASK |
						  CPLL_FBDIV_N2_MASK,
						  (refclk_div << 8) | (fbdiv_45 << 7) | fbdiv);
	} else {
		if (!pll_done) {
			xcvr_drp_writef(core, QPLL_CFG0_ADDR,
							  QPLL_CFG0_BAND_MASK, lowband);

			xcvr_drp_writef(core, QPLL_REFCLK_DIV_M_ADDR,
							  QPLL_REFCLK_DIV_M_MASK, refclk_div);

			xcvr_drp_writef(core, QPLL_FBDIV_N_ADDR,
							  QPLL_FBDIV_N_MASK, fbdiv);

			xcvr_drp_writef(core, QPLL_FBDIV_RATIO_ADDR,
							  QPLL_FBDIV_RATIO_MASK, fbdiv_ratio);
			pll_done = 1;
		}
	}

	ret = xcvr_drp_writef(core, RXOUT_DIV_ADDR,
			core.rx_tx_n ? RXOUT_DIV_MASK : TXOUT_DIV_MASK, out_div);

	xcvr_rxcdr_settings(core, out_div);

	xcvr_write(core, XCVR_REG_RESETN, XCVR_RESETN);

	return ret;
}



/*******************************************************************************
* @brief xcvr_set_lpm_dfe_mode
*******************************************************************************/
int32_t xcvr_set_lpm_dfe_mode(xcvr_core core,	uint32_t lpm)
{
	if (core.gth_enable) {
		if (lpm) {
			xcvr_drp_write(core, 0x036, 0x0032);
			xcvr_drp_write(core, 0x039, 0x1000);
			xcvr_drp_write(core, 0x062, 0x1980);
		} else {
			xcvr_drp_write(core, 0x036, 0x0002);
			xcvr_drp_write(core, 0x039, 0x0000);
			xcvr_drp_write(core, 0x062, 0x0000);
		}
	} else {
		if (lpm)
			xcvr_drp_write(core, 0x029, 0x0104);
		else
			xcvr_drp_write(core, 0x029, 0x0954);
	}

	return 0;
}
#endif
