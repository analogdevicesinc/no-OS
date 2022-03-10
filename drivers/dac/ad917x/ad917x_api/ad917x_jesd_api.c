// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad917x_jesd_api.c
 *
 * \brief Contains AD917x APIs for JESD configuration and control
 *
 * Release 1.1.X
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#include "AD917x.h"
#include "ad917x_reg.h"
#include "api_errors.h"
#include <stddef.h>


#define SERDES_PWRUP_DELAY 100000
#define LANE_MIN 1
#define LANE_INDEX_MAX (AD917X_JESD_NOF_LANES -1)
#define LINK_INDEX_MAX (AD917X_JESD_NOF_LINKS -1)
#define LINK_INDEX(x)  ( ((x) == 0xFF) ? (x) : NO_OS_BIT(x))
#define SYNCOUTB_INDEX_MAX (AD917X_JESD_NOF_SYNCOUTB-1)
#define SYNCOUTB_INDEX(x)  (((x)==0xFF) ? (x) : NO_OS_BIT(x))
#define K_MAX 32
#define M_DEFAULT 2
#define N_DEFAULT 16
#define NP_DEFAULT 16
#define S_MIN 1
#define S_MAX 4
#define CF_DEFAULT 0
#define CS_DEFAULT 0
#define HD_DEFAULT 0
#define LANE_RATE_MIN 750
#define LANE_RATE_MAX 12500
#define INTERPOLATION_MIN 1
#define DP_INTERPOLATION_MAX 12
#define CH_INTERPOLATION_MAX 8
#define DAC_CLK_FREQ_MIN 850
#define DAC_CLK_FREQ_MAX 6000
#define SYSREF_JITTER_WIN_MAX 28
#define INTPL_MODE_INVALID    25
#define JESD_MODE_INVALID     22
#define AD917X_LMFC_VAR_MAX   0xC

/*DataSheet Table 48 */
static struct adi_reg_data ADI_REC_EQ_INIT_TBL[] = {
	{0x240, 0xAA}, /*ADI INTERNAL Equaliser Settings*/
	{0x241, 0xAA}, /*ADI INTERNAL Equaliser Settings*/
	{0x242, 0x55}, /*ADI INTERNAL Equaliser Settings*/
	{0x243, 0x55}, /*ADI INTERNAL Equaliser Settings*/
	{0x244, 0x1F}, /*ADI INTERNAL Equaliser Settings*/
	{0x245, 0x1F}, /*ADI INTERNAL Equaliser Settings*/
	{0x246, 0x1F}, /*ADI INTERNAL Equaliser Settings*/
	{0x247, 0x1F}, /*ADI INTERNAL Equaliser Settings*/
	{0x248, 0x1F}, /*ADI INTERNAL Equaliser Settings*/
	{0x249, 0x1F}, /*ADI INTERNAL Equaliser Settings*/
	{0x24A, 0x1F}, /*ADI INTERNAL Equaliser Settings*/
	{0x24B, 0x1F} /*ADI INTERNAL Equaliser Settings*/
};

/*Engineering Sample DataSheet Table 48 */
static struct adi_reg_data ADI_REC_ES_SERDES_INIT_TBL_1[] = {
	{0x200, 0x01},  /*Power Down Serdes Blocks*/
	{0x210, 0x16}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x216, 0x05}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x212, 0xFF}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x212, 0x00}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x210, 0x87}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x210, 0x87}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x216, 0x11}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x01}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x00}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x200, 0x00}  /*Power up Serdes Blocks*/
};

static struct adi_reg_data ADI_REC_ES_SERDES_INIT_TBL_2[] = {
	{0x210, 0x86}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x216, 0x40}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x01}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x00}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x210, 0x86}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x210, 0x00}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x01}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x00}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x210, 0x87}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x216, 0x01}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x01}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x213, 0x00}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x280, 0x05}, /*ADI INTERNAL Init Serdes PLL Settings*/
	{0x280, 0x01} /*Start SERDES PLL and Init SERDES PLL CAL*/
};

static int32_t jesd_get_link_count(ad917x_handle_t *h, uint8_t *link_count)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (link_count == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_JESD_MODE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD917X_LINK_PAGE(ALL));
	*link_count = (tmp_reg & AD917X_LINK_MODE) ? 2 : 1;
	return API_ERROR_OK;
}

static int32_t jesd_set_link(ad917x_handle_t *h, int32_t link)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (link > 1)
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_JESD_RX_CTL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD917X_LINK_PAGE(ALL));
	tmp_reg |= AD917X_LINK_PAGE(link);
	return ad917x_register_write(h, AD917X_JESD_RX_CTL_REG, tmp_reg);
}

int32_t ad917x_jesd_config_datapath(ad917x_handle_t *h, uint8_t dual_en,
				    uint8_t jesd_mode, uint8_t ch_intpl, uint8_t dp_intpl)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (dual_en > 1)
		return API_ERROR_INVALID_PARAM;

	if (jesd_mode >= JESD_MODE_INVALID)
		return API_ERROR_INVALID_PARAM;
	if ((ch_intpl < INTERPOLATION_MIN) ||
	    (ch_intpl > CH_INTERPOLATION_MAX))
		return API_ERROR_INVALID_PARAM;
	if ((dp_intpl < INTERPOLATION_MIN) ||
	    (dp_intpl > DP_INTERPOLATION_MAX))
		return API_ERROR_INVALID_PARAM;

	/*Disable Links Prior to configuration*/
	err = ad917x_register_read(h, AD917X_JESD_RX_CTL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD917X_LINK_0_EN | AD917X_LINK_1_EN);
	err = ad917x_register_write(h, AD917X_JESD_RX_CTL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	/*Configure JESD Mode */
	err = ad917x_register_read(h, AD917X_JESD_MODE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD917X_JESD_MODE(ALL) | AD917X_LINK_MODE);
	tmp_reg |= AD917X_JESD_MODE(jesd_mode);
	tmp_reg |= ((dual_en == 1) ? AD917X_LINK_MODE : 0);
	err = ad917x_register_write(h, AD917X_JESD_MODE_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Configure Interpolation Mode */
	tmp_reg = 0x0;
	tmp_reg |= AD917X_CH_INTERP_MODE(ch_intpl) | AD917X_DP_INTERP_MODE(dp_intpl);
	err = ad917x_register_write(h, AD917X_INTERP_MODE_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Check Readback for Valid Configuration*/
	err = ad917x_register_read(h, AD917X_JESD_MODE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	if (tmp_reg & AD917X_JESD_MODE_INVALID)
		return API_ERROR_INVALID_PARAM;

	return API_ERROR_OK;
}

int32_t ad917x_jesd_get_cfg_status(ad917x_handle_t *h, uint8_t *cfg_valid)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (cfg_valid == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_JESD_MODE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	*cfg_valid = (tmp_reg & AD917X_JESD_MODE_INVALID) ? 0 : 1;

	return API_ERROR_OK;
}

int32_t ad917x_jesd_get_cfg_param(ad917x_handle_t *h,
				  jesd_param_t *jesd_param)
{
	int32_t err;
	uint8_t tmp_reg[AD917X_JESD_PARAM_REG_LEN];

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (jesd_param == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read_block(h,
					 AD917X_JESD_PARAM_REG_BASE, &tmp_reg[0], AD917X_JESD_PARAM_REG_LEN);
	if (err != API_ERROR_OK)
		return err;
	jesd_param->jesd_L = AD917X_JESD_L_GET(tmp_reg);
	jesd_param->jesd_F = AD917X_JESD_F_GET(tmp_reg);
	jesd_param->jesd_K = AD917X_JESD_K_GET(tmp_reg);
	jesd_param->jesd_M = AD917X_JESD_M_GET(tmp_reg);
	jesd_param->jesd_N = AD917X_JESD_N_GET(tmp_reg);
	jesd_param->jesd_NP = AD917X_JESD_NP_GET(tmp_reg);
	jesd_param->jesd_S = AD917X_JESD_S_GET(tmp_reg);
	jesd_param->jesd_JESDV = AD917X_JESD_V_GET(tmp_reg);
	jesd_param->jesd_HD = AD917X_JESD_HD_GET(tmp_reg);
	jesd_param->jesd_DID =  AD917X_JESD_DID_GET(tmp_reg);
	jesd_param->jesd_BID =  AD917X_JESD_BID_GET(tmp_reg);

	/*Fixed Parameters*/
	jesd_param->jesd_CF = CF_DEFAULT;
	jesd_param->jesd_CS = CS_DEFAULT;

	return API_ERROR_OK;
}

int32_t ad917x_jesd_set_sysref_enable(ad917x_handle_t *h, uint8_t en)
{
	int32_t err;
	uint8_t tmp_reg = 0x0;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (en > 1)
		return API_ERROR_INVALID_PARAM;
	err = ad917x_register_read(h, AD917X_SYSREF_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD917X_SYSREF_PD);
	tmp_reg |= ((en == 1) ? 0 : AD917X_SYSREF_PD);
	err = ad917x_register_write(h, AD917X_SYSREF_CTRL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	/*Enable Periodic Rst_en for Subclass 0 and Subclass 1*/
	err = ad917x_register_read(h, AD917X_SYSREF_ROTATION_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	err = ad917x_register_write(h, AD917X_SYSREF_ROTATION_REG,
				    (tmp_reg |
				     AD917X_SYNC_RSV_EN |
				     AD917X_PERIODIC_RST_EN |
				     AD917X_ROTATION_MODE(0x1)));
	if (err != API_ERROR_OK)
		return err;
	/*Set JESD  Subclass 0 and Subclass 1 setting*/
	err = ad917x_register_read(h, AD917X_JESD_ILS_NP_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg = ((en == 1) ? (tmp_reg | AD917X_JESD_JESDV) :
		   (tmp_reg & (~AD917X_JESD_JESDV)));
	err = ad917x_register_write(h, AD917X_JESD_ILS_NP_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int32_t ad917x_jesd_get_sysref_enable(ad917x_handle_t *h, uint8_t *en)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (en == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_SYSREF_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	*en = !(tmp_reg & AD917X_SYSREF_PD);

	return API_ERROR_OK;
}

int32_t ad917x_jesd_set_scrambler_enable(ad917x_handle_t *h, uint8_t en)
{
	int32_t err;
	uint8_t tmp_reg, i, link_count;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (en > 0x1)
		return API_ERROR_INVALID_PARAM;
	err = jesd_get_link_count(h, &link_count);
	if (err != API_ERROR_OK)
		return err;
	for (i = 0; i < link_count; i++) {
		err = jesd_set_link(h, i);
		if (err != API_ERROR_OK)
			return err;

		err = ad917x_register_read(h, AD917X_JESD_ILS_SCR_L_REG, &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		tmp_reg &= ~AD917X_JESD_SCR;
		tmp_reg |= ((en == 1) ? AD917X_JESD_SCR : 0);
		err = ad917x_register_write(h, AD917X_JESD_ILS_SCR_L_REG, tmp_reg);
		if (err != API_ERROR_OK)
			return err;
	}
	err = jesd_set_link(h, 0);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}


int32_t ad917x_jesd_enable_datapath(ad917x_handle_t *h,
				    uint8_t lanes_msk, uint8_t run_cal, uint8_t en)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (h->delay_us == NULL)
		return API_ERROR_INVALID_DELAYUS_PTR;

	if ((en > 1) || (run_cal > 1))
		return API_ERROR_INVALID_PARAM;

	if (en == 0x1) {
		/*Enable JESD Block*/
		err = ad917x_register_write(h, AD917X_DIG_RESET_REG, AD917X_DIG_PATH_PDN(0));
		if (err != API_ERROR_OK)
			return err;

		if (run_cal == 0x1) {
			/*Calibrate SERDES PHY Termination Block*/
			/*Initialise Equaliser*/
			ad917x_register_write_tbl(h, &ADI_REC_EQ_INIT_TBL[0],
						  NO_OS_ARRAY_SIZE(ADI_REC_EQ_INIT_TBL));
		}
		/*Power Down Any unused Lanes*/
		tmp_reg = ~lanes_msk;
		err = ad917x_register_write(h, AD917X_PHY_PD_REG, tmp_reg);
		if (err != API_ERROR_OK)
			return err;



		/*Engineering Sample Silicon Configuration*/
		/*Config SERDES PLL with ADI RECOMMENDED Settings*/
		err = ad917x_register_write_tbl(h, &ADI_REC_ES_SERDES_INIT_TBL_1[0],
						NO_OS_ARRAY_SIZE(ADI_REC_ES_SERDES_INIT_TBL_1));
		if (err != API_ERROR_OK)
			return err;
		err = h->delay_us(h->user_data, SERDES_PWRUP_DELAY);
		if (err != API_ERROR_OK)
			return err;
		/*Power Up Serdes*/
		err = ad917x_register_write(h,
					    AD917X_MASTER_PD_REG, AD917X_SERDES_PDN(0));
		if (err != API_ERROR_OK)
			return err;;

		/*Config SERDES PLL with ADI RECOMMENDED Settings*/
		err = ad917x_register_write_tbl(h, &ADI_REC_ES_SERDES_INIT_TBL_2[0],
						NO_OS_ARRAY_SIZE(ADI_REC_ES_SERDES_INIT_TBL_2));
		if (err != API_ERROR_OK)
			return err;

		/*Enable SERDES PLL*/
		err = ad917x_register_read(h, AD917X_PLL_EN_CTRL_REG, &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		tmp_reg |= AD917X_SERDES_PLL_STARTUP;
		err = ad917x_register_write(h, AD917X_PLL_EN_CTRL_REG, tmp_reg);
		if (err != API_ERROR_OK)
			return err;
	} else {

		/*TODO: Procedure to disable Reset JESD IF*/
		/*Disable SERDES PLL*/
		err = ad917x_register_read(h, AD917X_PLL_EN_CTRL_REG, &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		tmp_reg &= ~(AD917X_SERDES_PLL_STARTUP);
		err = ad917x_register_write(h, AD917X_PLL_EN_CTRL_REG, tmp_reg);
		if (err != API_ERROR_OK)
			return err;

		/*Power DOWN Serdes*/
		err = ad917x_register_write(h,
					    AD917X_MASTER_PD_REG, AD917X_SERDES_PDN(1));
		if (err != API_ERROR_OK)
			return err;;
		/*Power Down ALL Lanes*/
		err = ad917x_register_write(h, AD917X_PHY_PD_REG, 0xFF);
		if (err != API_ERROR_OK)
			return err;

		/*Disable JESD Block*/
		err = ad917x_register_write(h,
					    AD917X_DIG_RESET_REG, AD917X_DIG_PATH_PDN(1));
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}

int32_t ad917x_jesd_set_syncoutb_enable(ad917x_handle_t *h,
					jesd_syncoutb_t syncoutb, uint8_t en)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((en > 1) ||
	    ((syncoutb > SYNCOUTB_INDEX_MAX) && (syncoutb != SYNCOUTB_ALL)))
		return API_ERROR_INVALID_PARAM;
	err = ad917x_register_read(h, AD917X_GEN_PD_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	switch (syncoutb) {
	case SYNCOUTB_0:
		tmp_reg = ((en == 1) ? (tmp_reg & (~AD917X_SYNCOUTB_0_PD)) :
			   (tmp_reg | (AD917X_SYNCOUTB_0_PD)));
		break;
	case SYNCOUTB_1:
		tmp_reg = ((en == 1) ? (tmp_reg & (~AD917X_SYNCOUTB_1_PD)) :
			   (tmp_reg | (AD917X_SYNCOUTB_1_PD)));
		break;
	case SYNCOUTB_ALL:
		tmp_reg = ((en == 1) ? (~(SYNCOUTB_INDEX(syncoutb))) :
			   SYNCOUTB_INDEX(syncoutb));
		break;
	default:
		return API_ERROR_INVALID_PARAM;
	}

	return ad917x_register_write(h, AD917X_GEN_PD_REG, tmp_reg);
}

int32_t ad917x_jesd_get_pll_status(ad917x_handle_t *h, uint8_t *pll_status)
{
	int32_t err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (pll_status == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_PLL_STATUS_REG, pll_status);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int32_t ad917x_jesd_set_lane_xbar(ad917x_handle_t *h,
				  uint8_t logical_lane, uint8_t physical_lane)
{
	int32_t err;
	uint8_t tmp_reg_val;
	uint16_t tmp_reg_addr;
	uint8_t tmp_nibble;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((physical_lane > (LANE_INDEX_MAX)) || (logical_lane > LANE_INDEX_MAX))
		return API_ERROR_INVALID_PARAM;

	tmp_reg_addr = (AD917X_JESD_XBAR_LANE_REG + (logical_lane / 2));
	tmp_nibble = ((logical_lane % 2 > 0) ? 1 : 0);

	err = ad917x_register_read(h, tmp_reg_addr, &tmp_reg_val);
	if (err != API_ERROR_OK)
		return err;
	if (tmp_nibble == 0) {
		tmp_reg_val &= ~AD917X_XBAR_LANE_EVEN(ALL);
		tmp_reg_val |= AD917X_XBAR_LANE_EVEN(physical_lane);
	} else {
		tmp_reg_val &= ~AD917X_XBAR_LANE_ODD(ALL);
		tmp_reg_val |= AD917X_XBAR_LANE_ODD(physical_lane);
	}
	err = ad917x_register_write(h, tmp_reg_addr, tmp_reg_val);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int32_t ad917x_jesd_invert_lane(ad917x_handle_t *h,
				uint8_t logical_lane, uint8_t invert)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((logical_lane > LANE_INDEX_MAX) || (invert > 1))
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_JESD_INVERT_LANE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	if (invert)
		tmp_reg |= AD917X_JESD_INVERT_LANE(logical_lane);
	else
		tmp_reg &= ~AD917X_JESD_INVERT_LANE(logical_lane);

	err = ad917x_register_write(h, AD917X_JESD_INVERT_LANE_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int32_t ad917x_jesd_get_lane_xbar(ad917x_handle_t *h, uint8_t *phy_log_map)
{
	int32_t i, err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (phy_log_map == NULL)
		return API_ERROR_INVALID_PARAM;

	for (i = 0; i < AD917X_JESD_NOF_LANES / 2; i++) {

		err = ad917x_register_read(h, AD917X_JESD_XBAR_LANE_REG + i, &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		phy_log_map[(2 * i)] = tmp_reg & AD917X_XBAR_LANE_EVEN(ALL);
		phy_log_map[(2 * i) + 1] = ((tmp_reg & AD917X_XBAR_LANE_ODD(ALL)) >> 3);
	}

	return API_ERROR_OK;
}

int32_t ad917x_jesd_enable_link(ad917x_handle_t *h,
				jesd_link_t link, uint8_t en)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if ((en > 0x1) || ((link > (LINK_INDEX_MAX) && (link != JESD_LINK_ALL))))
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_JESD_RX_CTL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	if (en)
		tmp_reg |= AD917X_LINK_EN(LINK_INDEX(link));
	else
		tmp_reg &= ~AD917X_LINK_EN(LINK_INDEX(link));
	err = ad917x_register_write(h, AD917X_JESD_RX_CTL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int32_t ad917x_jesd_get_link_status(ad917x_handle_t *h, jesd_link_t link,
				    ad917x_jesd_link_stat_t *link_status)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (link_status == NULL)
		return API_ERROR_INVALID_PARAM;

	err = jesd_set_link(h, link);
	if (err != API_ERROR_OK)
		return err;

	err = ad917x_register_read(h, AD917X_JESD_CODE_GRP_SYNC_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	link_status->code_grp_sync_stat = tmp_reg;

	err = ad917x_register_read(h, AD917X_JESD_FRAME_SYNC_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	link_status->frame_sync_stat = tmp_reg;

	err = ad917x_register_read(h, AD917X_JESD_GOOD_CHECKSUM_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	link_status->good_checksum_stat = tmp_reg;

	err = ad917x_register_read(h, AD917X_JESD_INIT_LANE_SYNC_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	link_status->init_lane_sync_stat = tmp_reg;

	return API_ERROR_OK;
}


int32_t ad917x_jesd_set_lmfc_delay(ad917x_handle_t *h, jesd_link_t link,
				   uint8_t delay, uint8_t var)
{
	int32_t err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((delay > AD917X_JESD_LMFC_DELAY(ALL)) ||
	    (var > AD917X_LMFC_VAR_MAX))
		return API_ERROR_INVALID_PARAM;
	if ((link > (LINK_INDEX_MAX)) && (link != JESD_LINK_ALL))
		return API_ERROR_INVALID_PARAM;

	if ((link == JESD_LINK_0) || (link == JESD_LINK_ALL)) {
		err = ad917x_register_write(h, AD917X_JESD_LMFC_DELAY0_REG,
					    AD917X_JESD_LMFC_DELAY(delay));
		if (err != API_ERROR_OK)
			return err;
		err = ad917x_register_write(h, AD917X_JESD_LMFC_VAR0_REG,
					    AD917X_JESD_LMFC_VAR(var));
		if (err != API_ERROR_OK)
			return err;

	}

	if ((link == JESD_LINK_1) || (link == JESD_LINK_ALL)) {
		err = ad917x_register_write(h, AD917X_JESD_LMFC_DELAY1_REG,
					    AD917X_JESD_LMFC_DELAY(delay));
		if (err != API_ERROR_OK)
			return err;
		err = ad917x_register_write(h, AD917X_JESD_LMFC_VAR1_REG,
					    AD917X_JESD_LMFC_VAR(var));
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}
