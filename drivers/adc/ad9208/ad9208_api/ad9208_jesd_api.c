// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208_jesd_api.c
 *
 * \brief Contains AD9208 APIs for ADC JESD Configuration and Control
 *
 * Release 1.0.x
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#include "ad9208_api.h"
#include "ad9208_reg.h"
#include "api_errors.h"

#define K_MIN 4
#define K_MAX 32
#define LANE_MIN 1
#define LANE_MAX 8
#define CS_MAX   3
#define N_MIN 7
#define N_MAX 16
#define CF_DEFAULT 0
#define LANE_RATE_MIN_MBPS 390
#define LANE_RATE_MAX_MBPS 16000

typedef enum {
	AD9208_CB_LOW = 0,
	AD9208_CB_OVR_RANGE = 1,
	AD9208_CB_SIGNAL_MON = 2,
	AD9208_CB_FAST_DETECT = 3,
	AD9208_CB_SYSREF = 5,
} ad9208_control_bit_sel;

/*DataSheet Table 26 */
static struct adi_reg_data ADI_REC_SERDES_INIT_TBL[] = {
	{0x1228, 0x4F},
	{0x1228, 0x0F},
	{0x1222, 0x00},
	{0x1222, 0x04},
	{0x1222, 0x00},
	{0x1262, 0x08},
	{0x1262, 0x00}
};

typedef struct {
	uint64_t slr_lwr_thres;
	uint64_t slr_upr_thres;
	uint8_t vco_cfg;
} jesd_serdes_pll_cfg;

/*JESD Config Note F*/
static jesd_serdes_pll_cfg ADI_REC_SERDES_PLL_CFG[] = {
	{390, 781, 0xD}, /* UNDEF */
	{781, 1687, 0x9}, /* UNDEF */
	{1687, 3375, 0x5},
	{3375, 6750, 0x1},
	{6750, 13500, 0x0},
	{13500, 16000, 0x3}
};

static int get_jesd_serdes_vco_cfg(uint64_t slr_mbps, uint8_t *vco_cfg)
{
	/*Transport layer Parameter Ranges Table 22 */
	int i = 0x0;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ADI_REC_SERDES_PLL_CFG); i++) {
		if ((slr_mbps >= ADI_REC_SERDES_PLL_CFG[i].slr_lwr_thres) &&
		    (slr_mbps < ADI_REC_SERDES_PLL_CFG[i].slr_upr_thres)) {
			*vco_cfg = ADI_REC_SERDES_PLL_CFG[i].vco_cfg;
			return API_ERROR_OK;
		}
	}
	return API_ERROR_INVALID_PARAM;
}

static int check_jesd_params_range(jesd_param_t jesd_param)
{
	/*Transport layer Parameter Ranges Table 22 */
	if ((jesd_param.jesd_L != 1) &&
	    (jesd_param.jesd_L != 2) &&
	    (jesd_param.jesd_L != 4) && (jesd_param.jesd_L != 8)) {
		/*printf("API:AD9208:Err: Invalid JESD L \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	if ((jesd_param.jesd_M != 1) &&
	    (jesd_param.jesd_M != 2) &&
	    (jesd_param.jesd_M != 4) && (jesd_param.jesd_M != 8)) {
		/*printf("API:AD9208:Err: Invalid JESD M \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	if ((jesd_param.jesd_F != 1) &&
	    (jesd_param.jesd_F != 2) &&
	    (jesd_param.jesd_F != 4) && (jesd_param.jesd_F != 8)) {

		/*printf("API:AD9208:Err: Invalid JESD F \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	if ((jesd_param.jesd_N < N_MIN) || (jesd_param.jesd_N > N_MAX)) {

		/*printf("API:AD9208:Err: Invalid JESD N \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	if ((jesd_param.jesd_K < K_MIN) || (jesd_param.jesd_K > K_MAX) ||
	    (jesd_param.jesd_K % 4 != 0)) {

		/*printf("API:AD9208:Err: Invalid JESD K \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	if (jesd_param.jesd_CS > CS_MAX) {

		/*printf("API:AD9208:Err: Invalid JESD CS \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	if (jesd_param.jesd_CF > CF_DEFAULT) {

		/*printf("API:AD9208:Err: Invalid JESD CF \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	if ((jesd_param.jesd_NP != 8) && (jesd_param.jesd_NP != 16)) {

		/*printf("API:AD9208:Err: Invalid JESD NP \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}

	return API_ERROR_OK;
}

int ad9208_jesd_enable_link(ad9208_handle_t *h, uint8_t en)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (en > 1)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_JESD_LINK_CTRL1_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	if (en) {
		err = ad9208_register_write_tbl(h,
						&ADI_REC_SERDES_INIT_TBL[0],
						NO_OS_ARRAY_SIZE
						(ADI_REC_SERDES_INIT_TBL));
		if (err != API_ERROR_OK)
			return err;
	}
	tmp_reg &= ~AD9208_JESD_LINK_PDN;
	tmp_reg |= en ? 0 : AD9208_JESD_LINK_PDN;
	err = ad9208_register_write(h, AD9208_JESD_LINK_CTRL1_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_jesd_set_if_config(ad9208_handle_t *h,
			      jesd_param_t jesd_param,
			      uint64_t *lane_rate_kbps)
{
	int err;
	uint8_t tmp_reg, dcm, vco_cfg;
	uint64_t fout, slr, slr_mbps;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	err = check_jesd_params_range(jesd_param);
	if (err != API_ERROR_OK)
		return err;

	/*Determine DCM and Fout */
	ad9208_get_decimation(h, &dcm);
	if (err != API_ERROR_OK)
		return err;

	if (h->adc_clk_freq_hz != 0)
		fout = NO_OS_DIV_U64(h->adc_clk_freq_hz, dcm);
	else {
		/*printf("API:AD9208: JESD :INVALID: CLK FREQ \r\n"); */
		return API_ERROR_INVALID_PARAM;
	}
	/*Calculate Lane Rate */
	slr = (((jesd_param.jesd_M * jesd_param.jesd_N) * (10)) * fout);
	slr = NO_OS_DIV_U64(NO_OS_DIV_U64(slr, 8), jesd_param.jesd_L);
	slr_mbps = NO_OS_DIV_U64(slr, 1000000);

	if ((slr_mbps > LANE_RATE_MAX_MBPS) || (slr_mbps < LANE_RATE_MIN_MBPS)) {
		/*printf("API:AD9208: JESD :INVALID: SLR :%lld \r\n", slr_mbps); */
		return API_ERROR_INVALID_PARAM;
	}

	/*CFG SERDES PLL for SLR */
	err = get_jesd_serdes_vco_cfg(slr_mbps, &vco_cfg);
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_read(h, AD9208_JESD_SERDES_PLL_CFG_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= AD9208_JESD_SLR_CTRL(ALL);
	tmp_reg |= AD9208_JESD_SLR_CTRL(vco_cfg);
	err = ad9208_register_write(h, AD9208_JESD_SERDES_PLL_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Set NOF Converters */
	err = ad9208_register_write(h,
				    AD9208_JESD_M_CFG_REG,
				    (jesd_param.jesd_M - 1));
	if (err != API_ERROR_OK)
		return err;

	/*Set Resolution and Sample and control bits */
	tmp_reg = (AD9208_JESD_CS(jesd_param.jesd_CS) |
		   (AD9208_JESD_N(jesd_param.jesd_N - 1)));
	err = ad9208_register_write(h, AD9208_JESD_CS_N_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Set NP */
	err = ad9208_register_read(h, AD9208_JESD_SCV_NP_CFG_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD9208_JESD_NP(ALL));
	tmp_reg |= AD9208_JESD_NP(jesd_param.jesd_NP - 1);
	err = ad9208_register_write(h, AD9208_JESD_SCV_NP_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Octets per Frame (F) and Frames per Multiframe (k) */
	err = ad9208_register_write(h,
				    AD9208_JESD_F_CFG_REG,
				    (AD9208_JESD_F(jesd_param.jesd_F - 1)));
	if (err != API_ERROR_OK)
		return err;
	err = ad9208_register_write(h,
				    AD9208_JESD_K_CFG_REG,
				    (AD9208_JESD_K(jesd_param.jesd_K - 1)));
	if (err != API_ERROR_OK)
		return err;
	/*Set Lanes */
	err = ad9208_register_read(h, AD9208_JESD_L_SCR_CFG_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_JESD_LANES(ALL);
	tmp_reg |= AD9208_JESD_LANES(jesd_param.jesd_L - 1);
	err = ad9208_register_write(h, AD9208_JESD_L_SCR_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	if (lane_rate_kbps != NULL)
		*lane_rate_kbps = NO_OS_DIV_U64(slr, 1000);

	return API_ERROR_OK;
}

int ad9208_jesd_get_cfg_param(ad9208_handle_t *h, jesd_param_t *jesd_param)
{

	int err, i;
	uint8_t tmp_reg[AD9208_JESD_CFG_REG_OFFSET];

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (jesd_param == NULL)
		return API_ERROR_INVALID_PARAM;

	/*Read All the JESD CFG Registers */
	for (i = 0; i < AD9208_JESD_CFG_REG_OFFSET; i++) {
		err =
			ad9208_register_read(h, (AD9208_JESD_L_SCR_CFG_REG + i),
					     &tmp_reg[i]);
		if (err != API_ERROR_OK)
			return err;
	}
	jesd_param->jesd_L = (AD9208_JESD_LANES(tmp_reg[0]) + 1);
	jesd_param->jesd_F = (AD9208_JESD_F(tmp_reg[1]) + 1);
	jesd_param->jesd_K = (AD9208_JESD_K(tmp_reg[2]) + 1);
	jesd_param->jesd_M = (AD9208_JESD_M(tmp_reg[3]) + 1);
	jesd_param->jesd_CS = ((tmp_reg[4] & 0xC) >> 6);
	jesd_param->jesd_N = (AD9208_JESD_N(tmp_reg[4]) + 1);
	jesd_param->jesd_NP = (AD9208_JESD_NP(tmp_reg[4]) + 1);
	jesd_param->jesd_S = (AD9208_JESD_S(tmp_reg[5]));

	jesd_param->jesd_HD = (tmp_reg[6] & AD9208_JESD_HD) ? 1 : 0;
	jesd_param->jesd_CF = (AD9208_JESD_CF(tmp_reg[6]));

	/*Read All the JESD CFG Registers */
	for (i = 0; i < AD9208_JESD_ID_CFG_REG_OFFSET; i++) {
		err =
			ad9208_register_read(h, (AD9208_JESD_DID_CFG_REG + i),
					     &tmp_reg[i]);
		if (err != API_ERROR_OK)
			return err;
	}

	jesd_param->jesd_DID = tmp_reg[0];
	jesd_param->jesd_BID = AD9208_JESD_BID(tmp_reg[1]);
	jesd_param->jesd_LID0 = AD9208_JESD_LID0(tmp_reg[2]);

	return API_ERROR_OK;
}

int ad9208_jesd_enable_scrambler(ad9208_handle_t *h, uint8_t en)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (en > 1)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_JESD_L_SCR_CFG_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_JESD_SCR_EN;
	tmp_reg |= en ? AD9208_JESD_SCR_EN : 0;
	err = ad9208_register_write(h, AD9208_JESD_L_SCR_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;

}

int ad9208_jesd_set_lane_xbar(ad9208_handle_t *h,
			      uint8_t logical_lane, uint8_t physical_lane)
{
	int err;
	uint8_t tmp_reg_val;
	uint16_t tmp_reg_addr;
	uint8_t tmp_nibble;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((physical_lane > (LANE_MAX - 1)) || (logical_lane > LANE_MAX - 1))
		return API_ERROR_INVALID_PARAM;

	switch (physical_lane) {
	case 0:
	case 1:
		tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG;
		break;
	case 2:
	case 3:
		tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG + 1;
		break;
	case 4:
	case 5:
		tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG + 3;
		break;
	case 6:
	case 7:
		tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG + 4;
		break;
	default:
		return API_ERROR_INVALID_PARAM;
	}

	tmp_nibble = (physical_lane % 2) ? 1 : 0;

	err = ad9208_register_read(h, tmp_reg_addr, &tmp_reg_val);
	if (err != API_ERROR_OK)
		return err;
	if (tmp_nibble == 0) {
		tmp_reg_val &= (~AD9208_JESD_XBAR_LN_EVEN(ALL));
		tmp_reg_val |= AD9208_JESD_XBAR_LN_EVEN(logical_lane);
	} else {
		tmp_reg_val &= (~AD9208_JESD_XBAR_LN_ODD(ALL));
		tmp_reg_val |= AD9208_JESD_XBAR_LN_ODD(logical_lane);
	}
	err = ad9208_register_write(h, tmp_reg_addr, tmp_reg_val);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_jesd_get_lane_xbar(ad9208_handle_t *h, uint8_t *phy_log_map)
{
	int err, i;
	uint8_t tmp_reg[AD9208_JESD_XBAR_CFG_REG_OFFSET];

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (phy_log_map == NULL)
		return API_ERROR_INVALID_PARAM;

	for (i = 0; i < AD9208_JESD_XBAR_CFG_REG_OFFSET; i++) {
		err =
			ad9208_register_read(h, AD9208_JESD_XBAR_CFG_REG + i,
					     &tmp_reg[i]);
		if (err != API_ERROR_OK)
			return err;
	}
	phy_log_map[0] = tmp_reg[0] & AD9208_JESD_XBAR_LN_EVEN(ALL);
	phy_log_map[1] = (tmp_reg[0] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
	phy_log_map[2] = tmp_reg[1] & AD9208_JESD_XBAR_LN_EVEN(ALL);
	phy_log_map[3] = (tmp_reg[1] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
	phy_log_map[4] = tmp_reg[3] & AD9208_JESD_XBAR_LN_EVEN(ALL);
	phy_log_map[5] = (tmp_reg[3] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
	phy_log_map[6] = tmp_reg[4] & AD9208_JESD_XBAR_LN_EVEN(ALL);
	phy_log_map[7] = (tmp_reg[4] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
	return API_ERROR_OK;
}

int ad9208_jesd_get_pll_status(ad9208_handle_t *h, uint8_t *pll_status)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (pll_status == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_JESD_SERDES_PLL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	*pll_status = tmp_reg;

	return API_ERROR_OK;
}

int ad9208_jesd_subclass_set(ad9208_handle_t *h, uint8_t subclass)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (subclass >= JESD_SUBCLASS_INVALID)
		return API_ERROR_INVALID_PARAM;
	err = ad9208_register_read(h, AD9208_JESD_SCV_NP_CFG_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_JESD_SUBCLASS(ALL);
	tmp_reg |= AD9208_JESD_SUBCLASS(subclass);
	err = ad9208_register_write(h, AD9208_JESD_SCV_NP_CFG_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_jesd_syref_mode_set(ad9208_handle_t *h,
			       jesd_sysref_mode_t mode, uint8_t sysref_count)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (mode >= SYSREF_MON)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_SYSREF_CTRL_0_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_SYSREF_MODE_SEL(ALL);
	tmp_reg |= AD9208_SYSREF_MODE_SEL(mode);
	err = ad9208_register_write(h, AD9208_SYSREF_CTRL_0_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	err = ad9208_register_read(h, AD9208_SYSREF_CTRL_1_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_SYSREF_NSHOT_IGNORE(ALL);
	if (mode == SYSREF_ONESHOT)
		tmp_reg |= AD9208_SYSREF_NSHOT_IGNORE(sysref_count);
	else
		tmp_reg |= AD9208_SYSREF_NSHOT_IGNORE(0x0);
	err = ad9208_register_write(h, AD9208_SYSREF_CTRL_1_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

/**
 * \brief     Set SYSREF Signal Capture settings
 *
 * \param h                Pointer to the AD9208 device reference handle.
 * \param sysref_edge_sel  Set the transition on which SYSREF is valid. Valid values are:
 *                          0 LOW to HIGH transition
 *                          1 HIGH to LOW transition
 * \param clk_edge_sel     Set the Sysref Capture clock Edge. Valid values are:
 *                          0 Rising clock Edge
 *                          1 Falling Clock Edge
 * \param neg_window_skew   Skew sample clock by which captured sysref is ignored.
 *                          Skew set in clock cycles. Valid range 0-3
 * \param pos_window_skew   Skew sample clock by which captured sysref is ignored.
 *                          Skew set in clock cycles. Valid range 0-3
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_syref_config_set(ad9208_handle_t *h,
				 uint8_t sysref_edge_sel, uint8_t clk_edge_sel,
				 uint8_t neg_window_skew,
				 uint8_t pos_window_skew)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((sysref_edge_sel > 1) || (clk_edge_sel > 1) ||
	    (neg_window_skew > 0x3) || (pos_window_skew > 3))
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_SYSREF_CTRL_0_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD9208_SYSREF_TRANSITION_SEL(ALL);
	tmp_reg &= ~AD9208_SYSREF_CLK_EDGE_SEL(ALL);
	tmp_reg |= AD9208_SYSREF_CLK_EDGE_SEL(clk_edge_sel);
	tmp_reg |= AD9208_SYSREF_TRANSITION_SEL(sysref_edge_sel);

	err = ad9208_register_write(h, AD9208_SYSREF_CTRL_0_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	tmp_reg = AD9208_SYSREF_WIN_NEG(neg_window_skew) |
		  AD9208_SYSREF_WIN_POS(pos_window_skew);
	err = ad9208_register_write(h, AD9208_SYSREF_CTRL_2_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int ad9208_jesd_syref_status_get(ad9208_handle_t *h,
				 uint8_t *hold_status, uint8_t *setup_status,
				 uint8_t *phase_status)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((hold_status == NULL) ||
	    (setup_status == NULL) || (phase_status == NULL))
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_SYSREF_STAT_0_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	*hold_status = ((tmp_reg & 0xF0) >> 4);
	*setup_status = ((tmp_reg & 0x0F) >> 0);
	err = ad9208_register_read(h, AD9208_SYSREF_STAT_1_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	*phase_status = ((tmp_reg & 0xF) >> 0);

	return API_ERROR_OK;
}

int ad9208_jesd_sysref_timestamp_set(ad9208_handle_t *h,
				     uint8_t timestamp_en, uint8_t control_bit,
				     uint8_t delay)
{
	int err;
	uint8_t tmp_reg, tmp_nibble;
	uint16_t tmp_addr;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((timestamp_en > 0x1) ||
	    (control_bit > 2) || (delay > AD9208_SYSREF_TS_DELAY(ALL)))
		return API_ERROR_INVALID_PARAM;

	if (timestamp_en == 0x1) {
		err = ad9208_register_write(h, AD9208_CHIP_SYNC_MODE_REG,
					    AD9208_SYNC_TS_ENABLE);
		if (err != API_ERROR_OK)
			return err;

		if (err != API_ERROR_OK)
			return err;
		switch (control_bit) {
		case 0:
			tmp_addr = AD9208_OP_MODE_CTRL_1_REG;
			tmp_nibble = 0;
			break;
		case 1:
			tmp_addr = AD9208_OP_MODE_CTRL_1_REG;
			tmp_nibble = 1;
			break;
		case 2:
			tmp_addr = AD9208_OP_MODE_CTRL_2_REG;
			tmp_nibble = 0;
			break;
		default:
			return API_ERROR_INVALID_PARAM;
		}

		err = ad9208_register_read(h, tmp_addr, &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		tmp_reg &=
			((AD9208_OP_CONV_CTRL_BIT_SEL(ALL)) << (tmp_nibble * 4));
		tmp_reg |=
			((AD9208_OP_CONV_CTRL_BIT_SEL(AD9208_CB_SYSREF)) <<
			 (tmp_nibble * 4));
		err = ad9208_register_write(h, tmp_addr, tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		err = ad9208_register_write(h, AD9208_SYSREF_CTRL_3_REG,
					    AD9208_SYSREF_TS_DELAY(delay));
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}

/**
 * \brief     Set JESD LMFC Offset
 *
 * \param h       Pointer to the AD9208 device reference handle.
 * \param offset  Adjust the LMFC phase offset in frame clock cycles.
 *                For F = 1, only 4-frame shifts are valid
 *                For F = 2, Only 2-frame shifts are valid
 *                In all other cases for F, 1 frame shift are valid
 *                Refer to the AD9208 Datasheet for more details.
 *
 * \retval API_ERROR_OK API Completed Successfully
 * \retval API_ERROR_INVALID_HANDLE_PTR Invalid reference handle.
 * \retval API_ERROR_INVALID_XFER_PTR SPI Access Failed
 * \retval API_ERROR_SPI_XFER SPI XFER Function Failed
 * \retval API_ERROR_INVALID_PARAM    Invalid Parameter
 */
int ad9208_jesd_syref_lmfc_offset_set(ad9208_handle_t *h, uint8_t offset)
{
	int err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (offset > AD9208_JESD_LMFC_OFFSET(ALL))
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_JESD_LMFC_OFFSET_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= AD9208_JESD_LMFC_OFFSET(ALL);
	tmp_reg |= AD9208_JESD_LMFC_OFFSET(offset);
	err = ad9208_register_write(h, AD9208_JESD_LMFC_OFFSET_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}
