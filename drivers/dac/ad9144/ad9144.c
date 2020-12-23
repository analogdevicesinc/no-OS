/***************************************************************************//**
 * @file ad9144.c
 * @brief Implementation of AD9144 Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ad9144.h"
#include "error.h"

struct ad9144_jesd204_link_mode {
	uint8_t id;
	uint8_t M;
	uint8_t L;
	uint8_t S;
	uint8_t F;
};

static const struct ad9144_jesd204_link_mode ad9144_jesd204_link_modes[] = {
	/* ID, M, L, S, F */
	{  0, 4, 8, 1, 1 },
	{  1, 4, 8, 2, 2 },
	{  2, 4, 4, 1, 2 },
	{  3, 4, 2, 1, 4 },
	{  4, 2, 4, 1, 1 },
	{  5, 2, 4, 2, 2 },
	{  6, 2, 2, 1, 2 },
	{  7, 1, 1, 1, 4 },
	{  9, 1, 2, 1, 1 },
	{ 10, 1, 1, 1, 2 },
};

/***************************************************************************//**
 * @brief ad9144_spi_read
 *******************************************************************************/
int32_t ad9144_spi_read(struct ad9144_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = spi_write_and_read(dev->spi_desc,
				 buf,
				 3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
 * @brief ad9144_spi_write
 *******************************************************************************/
int32_t ad9144_spi_write(struct ad9144_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = spi_write_and_read(dev->spi_desc,
				 buf,
				 3);

	return ret;
}

/***************************************************************************//**
 * @brief ad9144_spi_check_status
 *******************************************************************************/
int32_t ad9144_spi_check_status(struct ad9144_dev *dev,
				uint16_t reg_addr,
				uint8_t reg_mask,
				uint8_t exp_reg_data)
{
	uint16_t timeout = 0;
	uint8_t status = 0;
	do {
		ad9144_spi_read(dev,
				reg_addr,
				&status);
		if ((status & reg_mask) == exp_reg_data) {
			return 0;
		} else {
			timeout++;
			mdelay(1);
		}
	} while(timeout < 100);

	return -1;
}

struct ad9144_reg_seq {
	uint16_t reg;
	uint16_t val;
};

int32_t ad9144_spi_write_seq(struct ad9144_dev *dev,
			     const struct ad9144_reg_seq *seq, uint32_t num)
{
	int32_t ret = 0;

	while (num) {
		ret |= ad9144_spi_write(dev, seq->reg, seq->val);
		num--;
		seq++;
	}

	return 0;
}

/*
 * Required device configuration as per table 16 from the AD9144
 * datasheet Rev B.
 */
static const struct ad9144_reg_seq ad9144_required_device_config[] = {
	{ 0x12d, 0x8b },
	{ 0x146, 0x01 },
	{ 0x2a4, 0xff },
	{ 0x232, 0xff },
	{ 0x333, 0x01 },
};

/*
 * Optimal settings for the SERDES PLL, as per table 39 of the AD9144 datasheet.
 */
static const struct ad9144_reg_seq ad9144_optimal_serdes_settings[] = {
	{ 0x284, 0x62 },
	{ 0x285, 0xc9 },
	{ 0x286, 0x0e },
	{ 0x287, 0x12 },
	{ 0x28a, 0x7b },
	{ 0x28b, 0x00 },
	{ 0x290, 0x89 },
	{ 0x294, 0x24 },
	{ 0x296, 0x03 },
	{ 0x297, 0x0d },
	{ 0x299, 0x02 },
	{ 0x29a, 0x8e },
	{ 0x29c, 0x2a },
	{ 0x29f, 0x78 },
	{ 0x2a0, 0x06 },
};

int32_t ad9144_setup_jesd204_link(struct ad9144_dev *dev,
				  const struct ad9144_init_param *init_param)
{
	const struct ad9144_jesd204_link_mode *link_mode = NULL;
	unsigned int lane_mask;
	unsigned int val;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(ad9144_jesd204_link_modes); i++) {
		if (ad9144_jesd204_link_modes[i].id == init_param->jesd204_mode) {
			link_mode = &ad9144_jesd204_link_modes[i];
			break;
		}
	}

	if (!link_mode)
		return -1;

	lane_mask = (1 << link_mode->L) - 1;

	ad9144_spi_write(dev, REG_ILS_DID, 0x00);
	ad9144_spi_write(dev, REG_ILS_BID, 0x00);
	ad9144_spi_write(dev, REG_ILS_LID0, 0x00);

	val = link_mode->L - 1;
	if (init_param->jesd204_scrambling)
		val |= 0x80;
	ad9144_spi_write(dev, REG_ILS_SCR_L, val);

	val = link_mode->F - 1;
	ad9144_spi_write(dev, REG_ILS_F, val);
	ad9144_spi_write(dev, REG_ILS_K, 0x1f);

	val = link_mode->M - 1;
	ad9144_spi_write(dev, REG_ILS_M, val);
	ad9144_spi_write(dev, REG_ILS_CS_N, 0x0f); // 16 bits per sample

	val = 0x0f; // 16 bits per sample
	if (init_param->jesd204_subclass == 1)
		val |= 0x20;
	ad9144_spi_write(dev, REG_ILS_NP, val);

	val = link_mode->S - 1;
	val |= 0x20; /* JESD204 version B */
	ad9144_spi_write(dev, REG_ILS_S, val);

	val = link_mode->F == 1 ? 0x80 : 0x00;
	ad9144_spi_write(dev, REG_ILS_HD_CF, val);

	ad9144_spi_write(dev, REG_LANEDESKEW, lane_mask);
	ad9144_spi_write(dev, REG_CTRLREG1, link_mode->F);
	ad9144_spi_write(dev, REG_LANEENABLE, lane_mask);

	/*
	 * Length of the SYNC~ error pulse in PCLK cycles. According to the
	 * JESD204 standard the pulse length should be two frame clock cycles.
	 *
	 * 1 PCLK cycle = 4 octets
	 *   => SYNC~ pulse length = 2 * octets_per_frame / 4
	 */
	switch (link_mode->F) {
	case 1:
		/* 0.5 PCLK cycles */
		val = 0x0;
		break;
	case 2:
		/* 1 PCLK cycle */
		val = 0x1;
		break;
	default:
		/* 2 PCLK cycles */
		val = 0x2;
		break;
	}
	ad9144_spi_write(dev, REG_SYNCB_GEN_1, val << 4);

	dev->num_converters = link_mode->M;
	dev->num_lanes = link_mode->L;

	return 0;
}

/*
 * PLL fixed register writes according to table 17 of the
 * AD9144 datasheet Rev. C.
 */
static const struct ad9144_reg_seq ad9144_pll_fixed_writes[] = {
	{ 0x87, 0x62 },
	{ 0x88, 0xc9 },
	{ 0x89, 0x0e },
	{ 0x8a, 0x12 },
	{ 0x8d, 0x7b },
	{ 0x1b0, 0x00 },
	{ 0x1b9, 0x24 },
	{ 0x1bc, 0x0d },
	{ 0x1be, 0x02 },
	{ 0x1bf, 0x8e },
	{ 0x1c0, 0x2a },
	{ 0x1c1, 0x2a },
	{ 0x1c4, 0x7e },
};

static int32_t ad9144_pll_setup(struct ad9144_dev *dev,
				const struct ad9144_init_param *init_param)
{
	uint32_t fref, fdac;
	uint32_t lo_div_mode;
	uint32_t ref_div_mode = 0;
	uint8_t vco_param[3];
	uint32_t bcount;
	uint32_t fvco;
	int32_t ret;

	fref = init_param->pll_ref_frequency_khz;
	fdac = init_param->pll_dac_frequency_khz;

	if (fref > 1000000 || fref < 35000)
		return -1;

	if (fdac > 2800000 || fdac < 420000)
		return -1;

	if (fdac >= 1500000)
		lo_div_mode = 1;
	else if (fdac >= 750000)
		lo_div_mode = 2;
	else
		lo_div_mode = 3;

	while (fref > 80000) {
		ref_div_mode++;
		fref /= 2;
	}

	fvco = fdac << (lo_div_mode + 1);
	bcount = fdac / (2 * fref);
	if (bcount < 6) {
		bcount *= 2;
		ref_div_mode++;
	}

	if (fvco < 6300000) {
		vco_param[0] = 0x08;
		vco_param[1] = 0x03;
		vco_param[2] = 0x07;
	} else if (fvco < 7250000) {
		vco_param[0] = 0x09;
		vco_param[1] = 0x03;
		vco_param[2] = 0x06;
	} else {
		vco_param[0] = 0x09;
		vco_param[1] = 0x13;
		vco_param[2] = 0x06;
	}

	ad9144_spi_write_seq(dev, ad9144_pll_fixed_writes,
			     ARRAY_SIZE(ad9144_pll_fixed_writes));

	ad9144_spi_write(dev, REG_DACLOGENCNTRL, lo_div_mode);
	ad9144_spi_write(dev, REG_DACLDOCNTRL1, ref_div_mode);
	ad9144_spi_write(dev, REG_DACINTEGERWORD0, bcount);

	ad9144_spi_write(dev, REG_DACPLLT5, vco_param[0]);
	ad9144_spi_write(dev, REG_DACPLLTB, vco_param[1]);
	ad9144_spi_write(dev, REG_DACPLLT18, vco_param[2]);

	ad9144_spi_write(dev, REG_DACPLLCNTRL, 0x10);

	ret = ad9144_spi_check_status(dev, REG_DACPLLSTATUS, 0x22, 0x22);
	if (ret == -1)
		printf("%s : DAC PLL NOT locked!.\n", __func__);

	return ret;
}

int32_t ad9144_set_nco(struct ad9144_dev *dev, int32_t f_carrier_khz,
		       int16_t phase)
{
	uint32_t modulation_type, phase_offset;
	bool sel_sideband = false;
	uint8_t i, reg;
	uint64_t ftw;
	int32_t ret;

	if (phase > abs(180))
		return FAILURE;
	if (f_carrier_khz < 0) {
		f_carrier_khz *= -1;
		sel_sideband = true;
	}

	if ((uint32_t) f_carrier_khz >= dev->sample_rate_khz / 2) {
		/* No modulation */
		modulation_type = MODULATION_TYPE(0);
	} else if (dev->sample_rate_khz == (uint32_t) f_carrier_khz * 4) {
		/* Coarse − f DAC /4 */
		modulation_type = MODULATION_TYPE(2);
	} else if (dev->sample_rate_khz == (uint32_t) f_carrier_khz * 8) {
		/* Coarse − f DAC /8 */
		modulation_type = MODULATION_TYPE(3);
	} else {
		/* NCO Fine Modulation */
		modulation_type = MODULATION_TYPE(1);
	}
	ret = ad9144_spi_read(dev, REG_DATAPATH_CTRL, &reg);
	if (ret != SUCCESS)
		return ret;
	reg = (reg & ~MODULATION_TYPE_MASK) | modulation_type;
	if (sel_sideband)
		reg |= SEL_SIDEBAND;
	else
		reg &= ~SEL_SIDEBAND;

	ret = ad9144_spi_write(dev, REG_DATAPATH_CTRL, reg);
	if (ret != SUCCESS)
		return ret;

	ftw = ((1ULL << 48) / dev->sample_rate_khz * f_carrier_khz);
	for (i = 0; i < 6; i++) {
		ret = ad9144_spi_write(dev, REG_FTW0 + i,
				       (ftw >> (8 * i)) & 0xFF);
		if (ret != SUCCESS)
			return ret;
	}

	phase_offset = (phase/180) * (1 << 15);
	ret = ad9144_spi_write(dev, REG_NCO_PHASE_OFFSET0, phase_offset & 0xFF);
	if (ret != SUCCESS)
		return ret;
	ret = ad9144_spi_write(dev, REG_NCO_PHASE_OFFSET1, (phase_offset >> 8) &
			       0xFF);
	if (ret != SUCCESS)
		return ret;

	if (modulation_type  == MODULATION_TYPE(1)) {
		ret = ad9144_spi_write(dev, REG_NCO_FTW_UPDATE, FTW_UPDATE_REQ);
		if (ret != SUCCESS)
			return ret;
	}

	return ret;
}

/***************************************************************************//**
 * @brief ad9144_setup
********************************************************************************/
int32_t ad9144_setup(struct ad9144_dev **device,
		     const struct ad9144_init_param *init_param)
{
	uint32_t serdes_plldiv;
	uint32_t serdes_cdr;
	uint8_t chip_id;
	uint8_t scratchpad;
	uint32_t val;
	int32_t ret;
	struct ad9144_dev *dev;

	dev = (struct ad9144_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret == -1)
		printf("%s : Device descriptor failed!\n", __func__);

	// reset
	ad9144_spi_write(dev, REG_SPI_INTFCONFA, SOFTRESET_M | SOFTRESET);
	ad9144_spi_write(dev, REG_SPI_INTFCONFA, init_param->spi3wire ? 0x00 : 0x18);
	mdelay(1);

	ad9144_spi_read(dev, REG_SPI_PRODIDL, &chip_id);
	if(chip_id != AD9144_CHIP_ID) {
		printf("%s : Invalid CHIP ID (0x%x).\n", __func__, chip_id);
		return -1;
	}

	ad9144_spi_write(dev, REG_SPI_SCRATCHPAD, 0xAD);
	ad9144_spi_read(dev, REG_SPI_SCRATCHPAD, &scratchpad);
	if(scratchpad != 0xAD) {
		printf("%s : scratchpad read-write failed (0x%x)!\n", __func__,
		       scratchpad);
		return -1;
	}

	// power-up and dac initialization
	ad9144_spi_write(dev, REG_PWRCNTRL0, 0x00);	// dacs - power up everything
	ad9144_spi_write(dev, REG_CLKCFG0, 0x00);	// clocks - power up everything
	ad9144_spi_write(dev, REG_SYSREF_ACTRL0,
			 0x00);	// sysref - power up/falling edge

	// required device configurations
	ad9144_spi_write_seq(dev, ad9144_required_device_config,
			     ARRAY_SIZE(ad9144_required_device_config));
	ad9144_spi_write_seq(dev, ad9144_optimal_serdes_settings,
			     ARRAY_SIZE(ad9144_optimal_serdes_settings));

	if (init_param->pll_enable)
		ad9144_pll_setup(dev, init_param);

	// digital data path

	switch (init_param->interpolation) {
	case 2:
		val = 0x01;
		break;
	case 4:
		val = 0x03;
		break;
	case 8:
		val = 0x04;
		break;
	default:
		val = 0x00;
		break;
	}

	ad9144_spi_write(dev, REG_INTERP_MODE, val);
	ad9144_spi_write(dev, REG_DATA_FORMAT, 0x00);	// 2's complement

	// transport layer

	ad9144_spi_write(dev, REG_MASTER_PD, 0x00);	// phy - power up
	ad9144_spi_write(dev, REG_PHY_PD, 0x00);	// phy - power up
	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_0, 0x00);	// single link - link 0
	ad9144_setup_jesd204_link(dev, init_param);

	// physical layer

	if (init_param->lane_rate_kbps < 2880000) {
		serdes_cdr = 0x0a;
		serdes_plldiv = 0x06;
	} else if (init_param->lane_rate_kbps < 5750000) {
		serdes_cdr = 0x08;
		serdes_plldiv = 0x05;
	} else {
		serdes_cdr = 0x28;
		serdes_plldiv = 0x04;
	}

	dev->sample_rate_khz = init_param->lane_rate_kbps / 40 *
			       dev->num_lanes * 2 /
			       dev->num_converters;

	ad9144_spi_write(dev, REG_DEV_CONFIG_9, 0xb7);		// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_10, 0x87);		// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_11, 0xb7);		// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_12, 0x87);		// jesd termination
	ad9144_spi_write(dev, REG_TERM_BLK1_CTRLREG0,
			 0x01);	// input termination calibration
	ad9144_spi_write(dev, REG_TERM_BLK2_CTRLREG0,
			 0x01);	// input termination calibration
	ad9144_spi_write(dev, REG_SERDES_SPI_REG, 0x01);	// pclk == qbd master clock
	ad9144_spi_write(dev, REG_CDR_OPERATING_MODE_REG_0, serdes_cdr);
	ad9144_spi_write(dev, REG_CDR_RESET, 0x00);	// cdr reset
	ad9144_spi_write(dev, REG_CDR_RESET, 0x01);	// cdr reset
	ad9144_spi_write(dev, REG_REF_CLK_DIVIDER_LDO, serdes_plldiv);
	ad9144_spi_write(dev, REG_SYNTH_ENABLE_CNTRL, 0x01);	// enable serdes pll
	ad9144_spi_write(dev, REG_SYNTH_ENABLE_CNTRL,
			 0x05);	// enable serdes calibration
	mdelay(20);

	ret = ad9144_spi_check_status(dev, REG_PLL_STATUS, 0x01, 0x01);
	if (ret == -1)
		printf("%s : PLL NOT locked!.\n", __func__);

	ad9144_spi_write(dev, REG_EQ_BIAS_REG, 0x62);	// equalizer

	// data link layer

	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_1, 0x01);	// subclass-1
	ad9144_spi_write(dev, REG_LMFC_DELAY_0, 0x00);	// lmfc delay
	ad9144_spi_write(dev, REG_LMFC_DELAY_1, 0x00);	// lmfc delay
	ad9144_spi_write(dev, REG_LMFC_VAR_0, 0x0a);	// receive buffer delay
	ad9144_spi_write(dev, REG_LMFC_VAR_1, 0x0a);	// receive buffer delay
	ad9144_spi_write(dev, REG_SYNC_CTRL, 0x01);	// sync-oneshot mode
	ad9144_spi_write(dev, REG_SYNC_CTRL, 0x81);	// sync-enable
	ad9144_spi_write(dev, REG_SYNC_CTRL, 0xc1);	// sysref-armed
	ad9144_spi_write(dev, REG_XBAR_LN_0_1,
			 SRC_LANE0(init_param->jesd204_lane_xbar[0]) |
			 SRC_LANE1(init_param->jesd204_lane_xbar[1]));
	ad9144_spi_write(dev, REG_XBAR_LN_2_3,
			 SRC_LANE2(init_param->jesd204_lane_xbar[2]) |
			 SRC_LANE3(init_param->jesd204_lane_xbar[3]));
	ad9144_spi_write(dev, REG_XBAR_LN_4_5,
			 SRC_LANE4(init_param->jesd204_lane_xbar[4]) |
			 SRC_LANE5(init_param->jesd204_lane_xbar[5]));
	ad9144_spi_write(dev, REG_XBAR_LN_6_7,
			 SRC_LANE6(init_param->jesd204_lane_xbar[6]) |
			 SRC_LANE7(init_param->jesd204_lane_xbar[7]));
	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_0, 0x01);	// enable link

	// dac calibration
	ad9144_dac_calibrate(dev);

	*device = dev;

	return ret;
}

int32_t ad9144_dac_calibrate(struct ad9144_dev *dev)
{
	uint32_t dac_mask;
	unsigned int i;
	int ret;

	dac_mask = (1 << dev->num_converters) - 1;

	/*
	 * DAC calibration sequence as per table 86 AD9144 datasheet Rev C.
	 */
	ad9144_spi_write(dev, REG_CAL_CLKDIV, 0x38);	// set calibration clock to 1m
	ad9144_spi_write(dev, REG_CAL_INIT, 0xa2);	// use isb reference of 38 to set cal
	ad9144_spi_write(dev, REG_CAL_INDX, dac_mask);	// select all active DACs
	ad9144_spi_write(dev, REG_CAL_CTRL, 0x01);	// single cal enable
	ad9144_spi_write(dev, REG_CAL_CTRL, 0x03);	// single cal start
	mdelay(10);

	for (i = 0; i < dev->num_converters; i++) {
		ad9144_spi_write(dev, REG_CAL_INDX, BIT(i));	// read dac-i

		ret = ad9144_spi_check_status(dev, REG_CAL_CTRL, 0xc0, 0x80);
		if (ret == -1)
			printf("%s: dac-%d calibration failed!\n", __func__, i);
	}

	ad9144_spi_write(dev, REG_CAL_CLKDIV, 0x30);	// turn off cal clock

	return 0;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad9144_setup().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9144_remove(struct ad9144_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief ad9144_status - return the status of the JESD interface
 *******************************************************************************/
int32_t ad9144_status(struct ad9144_dev *dev)
{

	uint8_t status = 0;
	int32_t ret = 0;
	uint32_t lane_mask;

	lane_mask = (1 << dev->num_lanes) - 1;

	// check for jesd status on all lanes
	// failures on top are 100% guaranteed to make subsequent status checks fail

	ad9144_spi_read(dev, REG_CODEGRPSYNCFLG, &status);
	if (status != lane_mask) {
		printf("%s : CGS NOT received (%x)!.\n", __func__, status);
		ret = -1;
	}
	ad9144_spi_read(dev, REG_INITLANESYNCFLG, &status);
	if (status != lane_mask) {
		printf("%s : ILAS NOT received (%x)!.\n", __func__, status);
		ret = -1;
	}
	ad9144_spi_read(dev, REG_FRAMESYNCFLG, &status);
	if (status != lane_mask) {
		printf("%s : framer OUT OF SYNC (%x)!.\n", __func__, status);
		ret = -1;
	}
	ad9144_spi_read(dev, REG_GOODCHKSUMFLG, &status);
	if (status != lane_mask) {
		printf("%s : check-sum MISMATCH (%x)!.\n", __func__, status);
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
 * @brief ad9144_short_pattern_test
 *******************************************************************************/
int32_t ad9144_short_pattern_test(struct ad9144_dev *dev,
				  const struct ad9144_init_param *init_param)
{

	uint32_t dac = 0;
	uint32_t sample = 0;
	uint8_t status = 0;
	int32_t ret = 0;

	for (dac = 0; dac < dev->num_converters; dac++) {
		for (sample = 0; sample < 4; sample++) {
			ad9144_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x00));
			ad9144_spi_write(dev, REG_SHORT_TPL_TEST_2,
					 (init_param->stpl_samples[dac][sample]>>8));
			ad9144_spi_write(dev, REG_SHORT_TPL_TEST_1,
					 (init_param->stpl_samples[dac][sample]>>0));
			ad9144_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x01));
			ad9144_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x03));
			ad9144_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x01));

			ret = ad9144_spi_check_status(dev,
						      REG_SHORT_TPL_TEST_3,
						      0x01, 0x00);
			if (ret == -1)
				printf("%s : short-pattern-test mismatch (0x%x, 0x%x 0x%x, 0x%x)!.\n",
				       __func__, dac, sample,
				       init_param->stpl_samples[dac][sample],
				       status);
		}
	}
	return 0;
}

/***************************************************************************//**
 * @brief ad9144_datapath_prbs_test
 *******************************************************************************/
int32_t ad9144_datapath_prbs_test(struct ad9144_dev *dev,
				  const struct ad9144_init_param *init_param)
{

	uint8_t status = 0;
	int32_t ret = 0;


	ad9144_spi_write(dev, REG_PRBS, ((init_param->prbs_type << 2) | 0x03));
	ad9144_spi_write(dev, REG_PRBS, ((init_param->prbs_type << 2) | 0x01));
	mdelay(500);

	ad9144_spi_write(dev, REG_SPI_PAGEINDX, 0x01);
	ad9144_spi_read(dev, REG_PRBS, &status);
	if ((status & 0xc0) != 0xc0) {
		printf("%s : PRBS OUT OF SYNC (%x)!.\n", __func__, status);
		ret = -1;
	}
	ad9144_spi_read(dev, REG_PRBS_ERROR_I, &status);
	if (status != 0x00) {
		printf("%s : PRBS I channel ERRORS (%x)!.\n", __func__,
		       status);
		ret = -1;
	}
	ad9144_spi_read(dev, REG_PRBS_ERROR_Q, &status);
	if (status != 0x00) {
		printf("%s : PRBS Q channel ERRORS (%x)!.\n", __func__,
		       status);
		ret = -1;
	}

	return ret;
}

