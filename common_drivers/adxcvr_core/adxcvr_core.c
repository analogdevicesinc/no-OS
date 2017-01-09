/***************************************************************************//**
* @file adxcvr.c
* @brief Implementation of ADXCVR Driver.
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
#include "adxcvr_core.h"

/******************************************************************************/
/************************ JESD204 Core Definitions ****************************/
/******************************************************************************/
#define JESD204_REG_TRX_VERSION					0x00
#define JESD204_REG_TRX_RESET					0x04
#define JESD204_REG_TRX_ILA_SUPPORT				0x08
#define JESD204_REG_TRX_SCRAMBLING				0x0c
#define JESD204_REG_TRX_SYSREF_HANDLING			0x10
#define JESD204_REG_TX_ILA_MULTIFRAMES			0x14
#define JESD204_REG_TRX_TEST_MODES				0x18
#define JESD204_REG_RX_LINK_ERROR_STATUS		0x1c
#define JESD204_REG_TRX_OCTETS_PER_FRAME		0x20
#define JESD204_REG_TRX_FRAMES_PER_MULTIFRAME	0x24
#define JESD204_REG_TRX_LANES_IN_USE			0x28
#define JESD204_REG_TRX_SUBCLASS_MODE			0x2c
#define JESD204_REG_TRX_SYNC_STATUS				0x38

/* JESD204_REG_TRX_RESET */
#define JESD204_TRX_GT_WDT_DIS					(1 << 16)
#define JESD204_TRX_RESET						(1 << 0)

/* JESD204_REG_TRX_ILA_SUPPORT */
#define JESD204_TRX_ILA_EN						(1 << 0)

/* JESD204_REG_TRX_SCRAMBLING */
#define JESD204_TRX_SCR_EN						(1 << 0)

/* JESD204_REG_RX_LINK_ERROR_STATUS */
#define JESD204_RX_LINK_LANE_ALIGN_ERR_ALARM	(1 << 31)
#define JESD204_RX_LINK_SYSREF_LMFC_ALARM		(1 << 30)
#define JESD204_RX_LINK_BUFF_OVF_ALARM			(1 << 29)
#define JESD204_RX_LINK_K_CH_ERR(l)				((1 << 2) << (3 * (l)))
#define JESD204_RX_LINK_DISP_ERR(l)				((1 << 1) << (3 * (l)))
#define JESD204_RX_LINK_NOT_IN_TBL_ERR(l)		((1 << 0) << (3 * (l)))

/* JESD204_REG_TRX_OCTETS_PER_FRAME */
#define JESD204_TRX_OCTETS_PER_FRAME(x)			(((x) - 1) & 0xff)

/* JESD204_REG_TRX_FRAMES_PER_MULTIFRAME */
#define JESD204_TRX_FRAMES_PER_MULTIFRAME(x)	(((x) - 1) & 0x1f)

/* JESD204_REG_TRX_SUBCLASS_MODE */
#define JESD204_TRX_SUBCLASS_MODE(x)			((x) & 0x3)

/* JESD204_REG_TRX_SYNC_STATUS */
#define JESD204_TRX_SYSREF_CAPTURED				(1 << 16)
#define JESD204_TRX_SYNC_ACHIEVED				(1 << 0)

/******************************************************************************/
/************************* ADXCVR Core Definitions ****************************/
/******************************************************************************/
#define PCORE_VER(major, minor, letter)	((major << 16) | (minor << 8) | letter)
#define PCORE_VER_MAJOR(version)		(version >> 16)
#define PCORE_VER_MINOR(version)		((version >> 8) & 0xff)
#define PCORE_VER_LETTER(version)		(version & 0xff)

#define ADXCVR_REG_VERSION				0x0000
#define ADXCVR_VERSION(x)				(((x) & 0xffffffff) << 0)
#define ADXCVR_VERSION_IS(x, y, z)		((x) << 16 | (y) << 8 | (z))
#define ADXCVR_VERSION_MAJOR(x)			((x) >> 16)

#define ADXCVR_REG_ID					0x0004

#define ADXCVR_REG_SCRATCH				0x0008

#define ADXCVR_REG_RESETN				0x0010
#define ADXCVR_RESETN					(1 << 0)

#define ADXCVR_REG_STATUS				0x0014
#define ADXCVR_STATUS					(1 << 0)

#define ADXCVR_REG_CONTROL				0x0020
#define ADXCVR_LPM_DFE_N				(1 << 12)
#define ADXCVR_RATE(x)					(((x) & 0x7) << 8)
#define ADXCVR_SYSCLK_SEL(x)			(((x) & 0x3) << 4)
#define ADXCVR_OUTCLK_SEL(x)			(((x) & 0x7) << 0)

#define ADXCVR_REG_CM_SEL				0x0040

#define ADXCVR_REG_CM_CONTROL			0x0044
#define ADXCVR_CM_WR					(1 << 28)
#define ADXCVR_CM_ADDR(x)				(((x) & 0xFFF) << 16)
#define ADXCVR_CM_WDATA(x)				(((x) & 0xFFFF) << 0)

#define ADXCVR_REG_CM_STATUS			0x0048
#define ADXCVR_CM_BUSY					(1 << 16)
#define ADXCVR_CM_RDATA(x)				(((x) & 0xFFFF) << 0)

#define ADXCVR_REG_CH_SEL				0x0060

#define ADXCVR_REG_CH_CONTROL			0x0064
#define ADXCVR_CH_WR					(1 << 28)
#define ADXCVR_CH_ADDR(x)				(((x) & 0xFFF) << 16)
#define ADXCVR_CH_WDATA(x)				(((x) & 0xFFFF) << 0)

#define ADXCVR_REG_CH_STATUS			0x0068
#define ADXCVR_CH_BUSY					(1 << 16)
#define ADXCVR_CH_RDATA(x)				(((x) & 0xFFFF) << 0)

#define ADXCVR_BROADCAST				0xff

#define TXOUT_DIV_ADDR					0x88
#define TXOUT_DIV_MASK					0x70
#define TXOUT_DIV_OFFSET				0x4
#define TXOUT_DIV_WIDTH					0x3
#define TXOUT_DIV_DEFAULT				0x0

#define RXOUT_DIV_ADDR					0x88
#define RXOUT_DIV_MASK					0x7
#define RXOUT_DIV_OFFSET				0x0
#define RXOUT_DIV_WIDTH					0x3
#define RXOUT_DIV_DEFAULT				0x0

#define RXCDR_CFG0_ADDR					0xa8
#define RXCDR_CFG0_MASK					0xffff
#define RXCDR_CFG0_OFFSET				0x0
#define RXCDR_CFG0_WIDTH				0x10
#define RXCDR_CFG0_DEFAULT				0x0

#define RXCDR_CFG1_ADDR					0xa9
#define RXCDR_CFG1_MASK					0xffff
#define RXCDR_CFG1_OFFSET				0x0
#define RXCDR_CFG1_WIDTH				0x10
#define RXCDR_CFG1_DEFAULT				0x0

#define RXCDR_CFG2_ADDR					0xaa
#define RXCDR_CFG2_MASK					0xffff
#define RXCDR_CFG2_OFFSET				0x0
#define RXCDR_CFG2_WIDTH				0x10
#define RXCDR_CFG2_DEFAULT				0x0

#define RXCDR_CFG3_ADDR					0xab
#define RXCDR_CFG3_MASK					0xffff
#define RXCDR_CFG3_OFFSET				0x0
#define RXCDR_CFG3_WIDTH				0x10
#define RXCDR_CFG3_DEFAULT				0x0

#define RXCDR_CFG4_ADDR					0xac
#define RXCDR_CFG4_MASK					0xff
#define RXCDR_CFG4_OFFSET				0x0
#define RXCDR_CFG4_WIDTH				0x8
#define RXCDR_CFG4_DEFAULT				0x0

#define RX_DFE_LPM_CFG_ADDR				0x29
#define RX_DFE_LPM_CFG_MASK				0xffff
#define RX_DFE_LPM_CFG_OFFSET			0x0
#define RX_DFE_LPM_CFG_WIDTH			0x10
#define RX_DFE_LPM_CFG_DEFAULT			0x0

#define QPLL_CFG0_ADDR					0x32
#define QPLL_CFG0_MASK					0xffff
#define QPLL_CFG0_BAND_MASK				0x40
#define QPLL_CFG0_OFFSET				0x0
#define QPLL_CFG0_WIDTH					0x10
#define QPLL_CFG0_DEFAULT				0x0

#define QPLL_CFG1_ADDR					0x33
#define QPLL_CFG1_MASK					0x7ff
#define QPLL_CFG1_OFFSET				0x0
#define QPLL_CFG1_WIDTH					0xb
#define QPLL_CFG1_DEFAULT				0x0

#define QPLL_REFCLK_DIV_M_ADDR			0x33
#define QPLL_REFCLK_DIV_M_MASK			0xf800
#define QPLL_REFCLK_DIV_M_OFFSET		0xb
#define QPLL_REFCLK_DIV_M_WIDTH			0x5
#define QPLL_REFCLK_DIV_M_DEFAULT		0x0

#define QPLL_FBDIV_N_ADDR				0x36
#define QPLL_FBDIV_N_MASK				0x3ff
#define QPLL_FBDIV_N_OFFSET				0x0
#define QPLL_FBDIV_N_WIDTH				0xa
#define QPLL_FBDIV_N_DEFAULT			0x0

#define QPLL_FBDIV_RATIO_ADDR			0x37
#define QPLL_FBDIV_RATIO_MASK			0x40
#define QPLL_FBDIV_RATIO_OFFSET			0x6
#define QPLL_FBDIV_RATIO_WIDTH			0x1
#define QPLL_FBDIV_RATIO_DEFAULT		0x0

#define CPLL_CFG0_ADDR					0x5c
#define CPLL_CFG0_MASK					0xff00
#define CPLL_CFG0_OFFSET				0x8
#define CPLL_CFG0_WIDTH					0x8
#define CPLL_CFG0_DEFAULT				0x0

#define CPLL_CFG1_ADDR					0x5d
#define CPLL_CFG1_MASK					0xffff
#define CPLL_CFG1_OFFSET				0x0
#define CPLL_CFG1_WIDTH					0x10
#define CPLL_CFG1_DEFAULT				0x0

#define CPLL_REFCLK_DIV_M_ADDR			0x5e
#define CPLL_REFCLK_DIV_M_MASK			0x1f00
#define CPLL_REFCLK_DIV_M_OFFSET		0x8
#define CPLL_REFCLK_DIV_M_WIDTH			0x5
#define CPLL_REFCLK_DIV_M_DEFAULT		0x0

#define CPLL_FB_DIV_45_N1_ADDR			0x5e
#define CPLL_FB_DIV_45_N1_MASK			0x80
#define CPLL_FB_DIV_45_N1_OFFSET		0x7
#define CPLL_FB_DIV_45_N1_WIDTH			0x1
#define CPLL_FB_DIV_45_N1_DEFAULT		0x0

#define CPLL_FBDIV_N2_ADDR				0x5e
#define CPLL_FBDIV_N2_MASK				0x7f
#define CPLL_FBDIV_N2_OFFSET			0x0
#define CPLL_FBDIV_N2_WIDTH				0x7
#define CPLL_FBDIV_N2_DEFAULT			0x0

#define ENC_8B10B						810

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

/***************************************************************************//**
* @brief jesd204b_read
*******************************************************************************/
int32_t jesd204b_read(jesd204_core core,
					  uint32_t reg_addr,
					  uint32_t *reg_data)
{
	*reg_data = Xil_In32((core.base_addr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_write
*******************************************************************************/
int32_t jesd204b_write(jesd204_core core,
					   uint32_t reg_addr,
					   uint32_t reg_data)
{
	Xil_Out32((core.base_addr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief xcvr_read
*******************************************************************************/
int32_t adxcvr_read(adxcvr_core core,
					uint32_t reg_addr,
					uint32_t *reg_data)
{
	*reg_data = Xil_In32((core.base_addr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief xcvr_write
*******************************************************************************/
int32_t adxcvr_write(adxcvr_core core,
					 uint32_t reg_addr,
					 uint32_t reg_data)
{
	Xil_Out32((core.base_addr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief adxcvr_drp_write
*******************************************************************************/
int32_t adxcvr_drp_read(adxcvr_core core,
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

	adxcvr_write(core, ch_sel ? ADXCVR_REG_CH_SEL : ADXCVR_REG_CM_SEL,
				 ADXCVR_BROADCAST);
	adxcvr_write(core, ch_sel ? ADXCVR_REG_CH_CONTROL : ADXCVR_REG_CM_CONTROL,
				 ADXCVR_CM_ADDR(reg_addr));
	adxcvr_write(core, ch_sel ? ADXCVR_REG_CH_CONTROL : ADXCVR_REG_CM_CONTROL,
				 ch_sel ? ADXCVR_CH_ADDR(reg_addr) : ADXCVR_CM_ADDR(reg_addr));
	do {
		adxcvr_read(core, ch_sel ?
						  ADXCVR_REG_CH_STATUS : ADXCVR_REG_CM_STATUS, &val);
		if (val & (ch_sel ? ADXCVR_CH_BUSY : ADXCVR_CM_BUSY)) {
			mdelay(1);
			continue;
		}
#ifdef DEBUG
		xil_printf("%s: reg 0x%X val 0x%X\n",
				__func__, reg_addr, val & 0xFFFF);
#endif
		*reg_data = ch_sel ? ADXCVR_CH_RDATA(val) : ADXCVR_CM_RDATA(val);
		return 0;
	} while (timeout--);

	xil_printf("%s: Timeout!\n", __func__);

	return -1;
}

/***************************************************************************//**
* @brief adxcvr_drp_write
*******************************************************************************/
int32_t adxcvr_drp_write(adxcvr_core core,
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

	adxcvr_write(core, ch_sel ? ADXCVR_REG_CH_SEL : ADXCVR_REG_CM_SEL,
			ADXCVR_BROADCAST);
	adxcvr_write(core, ch_sel ? ADXCVR_REG_CH_CONTROL : ADXCVR_REG_CM_CONTROL,
			ch_sel ? (ADXCVR_CH_WR | ADXCVR_CH_ADDR(reg_addr) | ADXCVR_CH_WDATA(reg_data)) :
			(ADXCVR_CM_WR | ADXCVR_CM_ADDR(reg_addr) | ADXCVR_CM_WDATA(reg_data)));

	do {
		adxcvr_read(core, ch_sel ? ADXCVR_REG_CH_STATUS :
				ADXCVR_REG_CM_STATUS, &val);
		if (!(val & (ch_sel ? ADXCVR_CH_BUSY : ADXCVR_CM_BUSY))) {
			adxcvr_drp_read(core, reg_addr, &val_reg);
			if (val != val_reg)
				xil_printf("%s: MISMATCH reg 0x%X val 0x%X != read 0x%X\n",
						__func__, reg_addr, val, val_reg);
			return 0;
		}
		mdelay(1);
	} while (timeout--);

	xil_printf("%s: Timeout!\n", __func__);

	return -1;
}

/***************************************************************************//**
* @brief adxcvr_drp_writef
*******************************************************************************/
int32_t __adxcvr_drp_writef(adxcvr_core core,
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

	ret = adxcvr_drp_read(core, reg, &reg_data);
	if (ret < 0)
		return ret;

	tmp = reg_data;

	tmp &= ~mask;
	tmp |= ((val << offset) & mask);

	return adxcvr_drp_write(core, reg, tmp);
}

#define adxcvr_drp_writef(st, reg, mask, val) \
	__adxcvr_drp_writef  (st, reg, mask, find_first_bit(mask), val)

/***************************************************************************//**
* @brief adxcvr_calc_cpll_settings
*******************************************************************************/
int32_t adxcvr_calc_cpll_settings(adxcvr_core core,
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
						xil_printf("%s: M %d, D %d, N1 %d, N2 %d\n",
							__func__, M[m][0], D[d][0],
							N1[n1][0], N2[n2][0]);
#endif
						return laneRate_kHz;
					}
				}
			}
		}
	}

	xil_printf("%s: Failed to find matching dividers for %lu kHz rate\n",
		__func__, laneRate_kHz);

	return -1;
}

/***************************************************************************//**
* @brief adxcvr_calc_qpll_settings
*******************************************************************************/
int32_t adxcvr_calc_qpll_settings(adxcvr_core core,
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
					xil_printf("%s: M %d, D %d, N %d, ratio %d, lowband %d\n",
						__func__, M[m][0], D[d][0],
						N[n][0], (N[n][0] == 66) ? 0 : 1,
						_lowBand);
#endif
					return laneRate_kHz;
				}

			}
		}
	}

	xil_printf("%s: Failed to find matching dividers for %lu kHz rate\n",
		__func__, laneRate_kHz);

	return -1;
}

/***************************************************************************//**
* @brief adxcvr_gth_rxcdr_settings
*******************************************************************************/
int32_t adxcvr_gth_rxcdr_settings(adxcvr_core core,
								  uint32_t rxout_div)
{
	uint16_t cfg0, cfg1, cfg2, cfg3, cfg4 = 0;

  cfg2 = 0;

	if (core.tx_enable)
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
		xil_printf("%s: GTH PRBS CDR not implemented\n", __func__);
	}

	adxcvr_drp_write(core, RXCDR_CFG0_ADDR, cfg0);
	adxcvr_drp_write(core, RXCDR_CFG1_ADDR, cfg1);
	adxcvr_drp_write(core, RXCDR_CFG2_ADDR, cfg2);
	adxcvr_drp_write(core, RXCDR_CFG3_ADDR, cfg3);
	adxcvr_drp_write(core, RXCDR_CFG4_ADDR, cfg4);

	return 0;
}

/***************************************************************************//**
* @brief adxcvr_rxcdr_settings
*******************************************************************************/
int32_t adxcvr_rxcdr_settings(adxcvr_core core,
							  uint32_t rxout_div)
{
	u16 cfg0, cfg1, cfg2, cfg3, cfg4;

	if (core.tx_enable)
		return 0; /* Do Nothing */

	if (core.gth_enable)
		return adxcvr_gth_rxcdr_settings(core, rxout_div);

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
		if (core.lane_rate_khz > 6600000 && rxout_div == 1)
			cfg4 = 0x0B;
		else
			cfg4 = 0x03;

		switch (rxout_div) {
		case 0: /* 1 */
			if (core.lpm_enable) {
				if (core.lane_rate_khz  > 6600000) {
					if (core.ppm == PM_1250)
						cfg1 = 0x1020;
					else
						cfg1 = 0x1040;
				} else {
					cfg1 = 0x1020;
				}
			} else { /* DFE */
				if (core.lane_rate_khz  > 6600000) {
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

	adxcvr_drp_write(core, RXCDR_CFG0_ADDR, cfg0);
	adxcvr_drp_write(core, RXCDR_CFG1_ADDR, cfg1);
	adxcvr_drp_write(core, RXCDR_CFG2_ADDR, cfg2);
	adxcvr_drp_write(core, RXCDR_CFG3_ADDR, cfg3);
	adxcvr_drp_writef(core, RXCDR_CFG4_ADDR, RXCDR_CFG4_MASK, cfg4);

	return 0;
}

/***************************************************************************//**
* @brief adxcvr_rxcdr_settings
*******************************************************************************/
int32_t adxcvr_clk_set_rate(adxcvr_core core,
							uint32_t rate,
							uint32_t parent_rate)
{
	uint32_t refclk_div, out_div, fbdiv_45, fbdiv, fbdiv_ratio, lowband;
	int32_t ret, pll_done = 0;

#ifdef DEBUG
	xil_printf("%s: Rate %lu Hz Parent Rate %lu Hz\n",
		__func__, rate, parent_rate);
#endif

	if (core.cpll_enable)
		ret = adxcvr_calc_cpll_settings(core, parent_rate, rate,
						     &refclk_div, &out_div,
						     &fbdiv_45, &fbdiv);
	else
		ret = adxcvr_calc_qpll_settings(core, parent_rate, rate,
						     &refclk_div, &out_div,
						     &fbdiv, &fbdiv_ratio,
						     &lowband);
	if (ret < 0)
		return ret;

	adxcvr_write(core, ADXCVR_REG_RESETN, 0);

	if (core.cpll_enable) {
		adxcvr_drp_writef(core, CPLL_REFCLK_DIV_M_ADDR,
						  CPLL_REFCLK_DIV_M_MASK |
						  CPLL_FB_DIV_45_N1_MASK |
						  CPLL_FBDIV_N2_MASK,
						  (refclk_div << 8) | (fbdiv_45 << 7) | fbdiv);
	} else {
		if (!pll_done) {
			adxcvr_drp_writef(core, QPLL_CFG0_ADDR,
							  QPLL_CFG0_BAND_MASK, lowband);

			adxcvr_drp_writef(core, QPLL_REFCLK_DIV_M_ADDR,
							  QPLL_REFCLK_DIV_M_MASK, refclk_div);

			adxcvr_drp_writef(core, QPLL_FBDIV_N_ADDR,
							  QPLL_FBDIV_N_MASK, fbdiv);

			adxcvr_drp_writef(core, QPLL_FBDIV_RATIO_ADDR,
							  QPLL_FBDIV_RATIO_MASK, fbdiv_ratio);
			pll_done = 1;
		}
	}

	ret = adxcvr_drp_writef(core, RXOUT_DIV_ADDR,
			core.tx_enable ? TXOUT_DIV_MASK : RXOUT_DIV_MASK, out_div);

	adxcvr_rxcdr_settings(core, out_div);

	adxcvr_write(core, ADXCVR_REG_RESETN, ADXCVR_RESETN);

	return ret;
}

/***************************************************************************//**
* @brief adxcvr_set_lpm_dfe_mode
*******************************************************************************/
int32_t adxcvr_set_lpm_dfe_mode(adxcvr_core core,
								uint32_t lpm)
{
	if (core.gth_enable) {
		if (lpm) {
			adxcvr_drp_write(core, 0x036, 0x0032);
			adxcvr_drp_write(core, 0x039, 0x1000);
			adxcvr_drp_write(core, 0x062, 0x1980);
		} else {
			adxcvr_drp_write(core, 0x036, 0x0002);
			adxcvr_drp_write(core, 0x039, 0x0000);
			adxcvr_drp_write(core, 0x062, 0x0000);
		}
	} else {
		if (lpm)
			adxcvr_drp_write(core, 0x029, 0x0104);
		else
			adxcvr_drp_write(core, 0x029, 0x0954);
	}

	return 0;
}

/***************************************************************************//**
* @brief jesd204_init
*******************************************************************************/
int32_t jesd204_init(jesd204_core core)
{

	jesd204b_write(core, JESD204_REG_TRX_RESET,
			JESD204_TRX_GT_WDT_DIS | JESD204_TRX_RESET);
	jesd204b_write(core, JESD204_REG_TRX_ILA_SUPPORT,
			JESD204_TRX_ILA_EN);
	jesd204b_write(core, JESD204_REG_TRX_SCRAMBLING,
			JESD204_TRX_SCR_EN);
	jesd204b_write(core, JESD204_REG_TRX_SYSREF_HANDLING,
			0);
	jesd204b_write(core, JESD204_REG_TRX_OCTETS_PER_FRAME,
			JESD204_TRX_OCTETS_PER_FRAME(core.octets_per_frame));
	jesd204b_write(core, JESD204_REG_TRX_FRAMES_PER_MULTIFRAME,
			JESD204_TRX_FRAMES_PER_MULTIFRAME(core.frames_per_multiframe));
	jesd204b_write(core, JESD204_REG_TRX_SUBCLASS_MODE,
			JESD204_TRX_SUBCLASS_MODE(core.subclass_mode));

	xil_printf("JESD204 initialization done.\n");
    return 0;
}

/***************************************************************************//**
* @brief jesd204 generate SYSREF if necessar
*******************************************************************************/
int32_t jesd204_gen_sysref(jesd204_core core)
{
    if ((core.sysref_type == INTERN) && (core.subclass_mode >= 1)) {

        // generate SYS_REF

        ad_gpio_set(core.gpio_sysref, 1);
        mdelay(10);
    }
    return 0;
}
/***************************************************************************//**
* @brief jesd204_read_status
*******************************************************************************/
int32_t jesd204_read_status(jesd204_core core)
{
	uint32_t status;
	uint32_t timeout;
	uint8_t link;
	int32_t ret;

	timeout = 100;
	do {
		mdelay(1);
		jesd204b_read(core, JESD204_REG_TRX_RESET, &status);
		status &= JESD204_TRX_RESET;
	} while ((timeout--) && (status == JESD204_TRX_RESET));

	if (status == JESD204_TRX_RESET) {
		xil_printf("jesd_status: jesd reset not completed!\n");
		return -1;
	}

	if (core.subclass_mode >= 1) {
	  timeout = 100;
	  do {
		mdelay(1);
		jesd204b_read(core, JESD204_REG_TRX_SYNC_STATUS, &status);
		status &= JESD204_TRX_SYSREF_CAPTURED;
	  } while ((timeout--) && (status != JESD204_TRX_SYSREF_CAPTURED));

	  if (status != JESD204_TRX_SYSREF_CAPTURED) {
		xil_printf("jesd_status: missing SYS_REF!\n");
		return -1;
	  } else
		xil_printf("SYNC STATUS: 0x%x\n", status);
	}

	timeout = 100;
	do {
		mdelay(1);
		jesd204b_read(core, JESD204_REG_TRX_SYNC_STATUS, &status);
		status &= JESD204_TRX_SYNC_ACHIEVED;
	} while ((timeout--) && (status != JESD204_TRX_SYNC_ACHIEVED));

	if (status != JESD204_TRX_SYNC_ACHIEVED) {
		xil_printf("jesd_status: Link SYNC not achieved!\n");
		return -1;
	}

	if (core.rx_tx_n == 0)
		return 0;

	jesd204b_read(core, JESD204_REG_RX_LINK_ERROR_STATUS, &status);
	for (link = 0; link < 8; link++) {
		if (status & JESD204_RX_LINK_K_CH_ERR(link)) {
			xil_printf("Link %d: K_CH_ERR\n", link);
			ret = -1;
		}
		if (status & JESD204_RX_LINK_DISP_ERR(link)) {
			xil_printf("Link %d: DISP_ERR\n", link);
			ret = -1;
		}
		if (status & JESD204_RX_LINK_NOT_IN_TBL_ERR(link)) {
			xil_printf("Link %d: NOT_IN_TBL_ERR\n", link);
			ret = -1;
		}
	}

	if (status & JESD204_RX_LINK_LANE_ALIGN_ERR_ALARM) {
		xil_printf("jesd_status: frame alignment error!\n");
		ret = -1;
	}

	if (status & JESD204_RX_LINK_SYSREF_LMFC_ALARM) {
		xil_printf("jesd_status: sysref alignment error!\n");
		ret = -1;
	}

	if (status & JESD204_RX_LINK_BUFF_OVF_ALARM) {
		xil_printf("jesd_status: receive buffer overflow error!\n");
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
* @brief xcvr_init
*******************************************************************************/
int32_t adxcvr_init(adxcvr_core core)
{
	uint32_t status;
	uint32_t timeout;

	adxcvr_write(core, ADXCVR_REG_RESETN, 0);

	adxcvr_write(core, ADXCVR_REG_CONTROL,
				 ((core.lpm_enable ? ADXCVR_LPM_DFE_N : 0) |
				  ADXCVR_SYSCLK_SEL(core.sys_clk_sel) |
				  ADXCVR_OUTCLK_SEL(core.out_clk_sel)));

	core.cpll_enable = (core.sys_clk_sel == 3) ? 0 : 1;

	core.encoding = ENC_8B10B;
	core.ppm = PM_200;

	if (!core.tx_enable)
		adxcvr_set_lpm_dfe_mode(core, core.lpm_enable);

	if (core.init_set_rate_enable)
		adxcvr_clk_set_rate(core, core.lane_rate_khz, core.ref_rate_khz);

	adxcvr_write(core, ADXCVR_REG_RESETN, ADXCVR_RESETN);

	timeout = 100;
	do {
		mdelay(1);
		adxcvr_read(core,ADXCVR_REG_STATUS, &status);
	} while ((timeout--) && (status == 0));

	if (status) {
		xil_printf("XCVR successfully initialized.\n");

		return 0;
	} else {
		xil_printf("XCVR initialization error.\n");

		return -1;
	}
}
