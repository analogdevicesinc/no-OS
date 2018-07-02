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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "platform_drivers.h"
#include "ad9144.h"

struct ad9144_jesd204_link_mode {
	uint8_t id;
	uint8_t M;
	uint8_t L;
	uint8_t F;
	uint8_t S;
};

static const struct ad9144_jesd204_link_mode ad9144_jesd204_link_modes[] = {
	/* ID, M, L, F, S */
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

	val = link_mode->F == 0 ? 0x80 : 0x00;
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
	ad9144_spi_write(dev, REG_SYSREF_ACTRL0, 0x00);	// sysref - power up/falling edge

	// required device configurations
	ad9144_spi_write_seq(dev, ad9144_required_device_config,
		ARRAY_SIZE(ad9144_required_device_config));
	ad9144_spi_write_seq(dev, ad9144_optimal_serdes_settings,
		ARRAY_SIZE(ad9144_optimal_serdes_settings));

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
	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_0, 0x01);	// single link - link 0
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

	ad9144_spi_write(dev, REG_DEV_CONFIG_9, 0xb7);		// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_10, 0x87);		// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_11, 0xb7);		// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_12, 0x87);		// jesd termination
	ad9144_spi_write(dev, REG_TERM_BLK1_CTRLREG0, 0x01);	// input termination calibration
	ad9144_spi_write(dev, REG_TERM_BLK2_CTRLREG0, 0x01);	// input termination calibration
	ad9144_spi_write(dev, REG_SERDES_SPI_REG, 0x01);	// pclk == qbd master clock
	ad9144_spi_write(dev, REG_CDR_OPERATING_MODE_REG_0, serdes_cdr);
	ad9144_spi_write(dev, REG_CDR_RESET, 0x00);	// cdr reset
	ad9144_spi_write(dev, REG_CDR_RESET, 0x01);	// cdr reset
	ad9144_spi_write(dev, REG_REF_CLK_DIVIDER_LDO, serdes_plldiv);
	ad9144_spi_write(dev, REG_SYNTH_ENABLE_CNTRL, 0x01);	// enable serdes pll
	ad9144_spi_write(dev, REG_SYNTH_ENABLE_CNTRL, 0x05);	// enable serdes calibration
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
	 * DAC calibration sequence as per table 86 AD9144 datasheet Rev B.
	 */
	ad9144_spi_write(dev, REG_CAL_CLKDIV, 0x38);	// set calibration clock to 1m
	ad9144_spi_write(dev, REG_CAL_INIT, 0xa6);	// use isb reference of 38 to set cal
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

