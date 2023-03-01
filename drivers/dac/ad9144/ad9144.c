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
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

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


struct ad9144_jesd204_priv {
	struct ad9144_dev *dev;
};

#define AD9144_MOD_TYPE_NONE		(0x0 << 2)
#define AD9144_MOD_TYPE_FINE		(0x1 << 2)
#define AD9144_MOD_TYPE_COARSE4		(0x2 << 2)
#define AD9144_MOD_TYPE_COARSE8		(0x3 << 2)
#define AD9144_MOD_TYPE_MASK		(0x3 << 2)

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

	ret = no_os_spi_write_and_read(dev->spi_desc,
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

	ret = no_os_spi_write_and_read(dev->spi_desc,
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
			no_os_mdelay(1);
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

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad9144_jesd204_link_modes); i++) {
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
			     NO_OS_ARRAY_SIZE(ad9144_pll_fixed_writes));

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

	if (phase < -180 || phase >= 180)
		return -1;
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
	if (ret != 0)
		return ret;
	reg = (reg & ~MODULATION_TYPE_MASK) | modulation_type;
	if (sel_sideband)
		reg |= SEL_SIDEBAND;
	else
		reg &= ~SEL_SIDEBAND;

	ret = ad9144_spi_write(dev, REG_DATAPATH_CTRL, reg);
	if (ret != 0)
		return ret;

	ftw = ((1ULL << 48) / dev->sample_rate_khz * f_carrier_khz);
	for (i = 0; i < 6; i++) {
		ret = ad9144_spi_write(dev, REG_FTW0 + i,
				       (ftw >> (8 * i)) & 0xFF);
		if (ret != 0)
			return ret;
	}

	phase_offset = (phase/180) * (1 << 15);
	ret = ad9144_spi_write(dev, REG_NCO_PHASE_OFFSET0, phase_offset & 0xFF);
	if (ret != 0)
		return ret;
	ret = ad9144_spi_write(dev, REG_NCO_PHASE_OFFSET1, (phase_offset >> 8) &
			       0xFF);
	if (ret != 0)
		return ret;

	if (modulation_type  == MODULATION_TYPE(1)) {
		ret = ad9144_spi_write(dev, REG_NCO_FTW_UPDATE, FTW_UPDATE_REQ);
		if (ret != 0)
			return ret;
	}

	return ret;
}

static unsigned int ad9144_get_sample_rate(struct ad9144_dev *dev)
{
	unsigned int rate;

	if (dev->pll_enable)
		rate = dev->pll_dac_frequency_khz * 1000;
	else
		rate = dev->sample_rate_khz * 1000;

	return rate;
}

static int ad9144_jesd204_link_init(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason,
				    struct jesd204_link *lnk)
{
	struct ad9144_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9144_dev *dev = priv->dev;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	jesd204_copy_link_params(lnk, &dev->link_config);

	lnk->sample_rate = ad9144_get_sample_rate(dev);
	lnk->sample_rate_div = dev->interpolation;
	lnk->jesd_encoder = JESD204_ENCODER_8B10B;
	lnk->jesd_version = JESD204_VERSION_B;
	lnk->is_transmit = true;
	lnk->lane_ids = dev->link_config.lane_ids;

	return JESD204_STATE_CHANGE_DONE;
}

/*******************************************************************************
 * @brief ad9144_setup_link
********************************************************************************/
static int ad9144_setup_link(struct ad9144_dev *dev,
			     struct jesd204_link *config)
{
	unsigned int lane_mask;
	unsigned int M, L;
	unsigned int val;
	unsigned int i, j;

	/*
	 * Datasheet calls this mode 11, 12, 13. L and M need to be
	 * programmed to half their actual values.
	 */
	M = config->num_converters - 1;
	L = config->num_lanes - 1;
	lane_mask = (1 << config->num_lanes) - 1;

	for (i = 0; i < 4; i++) {
		j = 2 * i;

		val = dev->lane_mux[j];
		val |= dev->lane_mux[j + 1] << 3;
		ad9144_spi_write(dev, REG_XBAR(i), val);
	}

	val = 0;
	if (!config->subclass)
		val |= NO_OS_BIT(4);
	if (!config->sysref.capture_falling_edge)
		val |= NO_OS_BIT(2);
	ad9144_spi_write(dev, REG_SYSREF_ACTRL0, val);

	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_1, config->subclass);

	ad9144_spi_write(dev, REG_ILS_DID, config->device_id);
	ad9144_spi_write(dev, REG_ILS_BID, config->bank_id);

	val = L; /* L */
	if (config->scrambling)
		val |= NO_OS_BIT(7);
	ad9144_spi_write(dev, REG_ILS_SCR_L, val);

	ad9144_spi_write(dev, REG_ILS_F, config->octets_per_frame - 1); /* F */
	ad9144_spi_write(dev, REG_ILS_K, config->frames_per_multiframe - 1); /* K */
	ad9144_spi_write(dev, REG_ILS_M, M); /* M */
	ad9144_spi_write(dev, REG_ILS_CS_N, 15); /* N */

	val = 15; /* NP */
	val |= config->subclass << 5; /* SUBCLASSV */
	ad9144_spi_write(dev, REG_ILS_NP, val);

	val = config->samples_per_conv_frame - 1; /* S */
	val |= NO_OS_BIT(5); /* JESDVER */
	ad9144_spi_write(dev, REG_ILS_S, val);

	val = config->high_density ? NO_OS_BIT(7) : 0x0; /* HD */
	ad9144_spi_write(dev, REG_ILS_HD_CF, val);

	/* Static for now */
	ad9144_spi_write(dev, REG_KVAL, 0x01);

	ad9144_spi_write(dev, REG_LANEDESKEW, lane_mask);
	ad9144_spi_write(dev, REG_CTRLREG1, config->octets_per_frame);
	ad9144_spi_write(dev, REG_LANEENABLE, lane_mask);

	/*
	 * Length of the SYNC~ error pulse in PCLK cycles. According to the
	 * JESD204 standard the pulse length should be two frame clock cycles.
	 *
	 * 1 PCLK cycle = 4 octets
	 *   => SYNC~ pulse length = 2 * octets_per_frame / 4
	 */
	switch (config->octets_per_frame) {
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

	return 0;
}

static int ad9144_setup_pll(struct ad9144_dev *dev)
{
	unsigned int fref, fdac;
	unsigned int lo_div_mode;
	unsigned int ref_div_mode = 0;
	unsigned int vco_param[3];
	unsigned int bcount;
	unsigned int fvco;

	fref = dev->pll_ref_frequency_khz;
	fdac = dev->pll_dac_frequency_khz;

	if (fref > 1000000 || fref < 35000)
		return -EINVAL;

	if (fdac > 2800000 || fdac < 420000)
		return -EINVAL;

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
		vco_param[0] = 0x8;
		vco_param[1] = 0x3;
		vco_param[2] = 0x7;
	} else if (fvco < 7250000) {
		vco_param[0] = 0x9;
		vco_param[1] = 0x3;
		vco_param[2] = 0x6;
	} else {
		vco_param[0] = 0x9;
		vco_param[1] = 0x13;
		vco_param[2] = 0x6;
	}

	ad9144_spi_write_seq(dev, ad9144_pll_fixed_writes,
			     NO_OS_ARRAY_SIZE(ad9144_pll_fixed_writes));

	ad9144_spi_write(dev, REG_DACLOGENCNTRL, lo_div_mode);
	ad9144_spi_write(dev, REG_DACLDOCNTRL1, ref_div_mode);
	ad9144_spi_write(dev, REG_DACINTEGERWORD0, bcount);

	ad9144_spi_write(dev, REG_DACPLLT5, vco_param[0]);
	ad9144_spi_write(dev, REG_DACPLLTB, vco_param[1]);
	ad9144_spi_write(dev, REG_DACPLLT18, vco_param[2]);

	ad9144_spi_write(dev, REG_DACPLLCNTRL, 0x10);

	return 0;
}

static unsigned long ad9144_get_lane_rate(struct ad9144_dev *dev,
		unsigned int sample_rate)
{
	/*
	 * lanerate_khz = ((samplerate_hz / interpolation) * 20 * M / L) / 1000
	 *
	 * Slightly reordered here to avoid loss of precession or overflows.
	 */
	return NO_OS_DIV_ROUND_CLOSEST(sample_rate,
				       50 * dev->num_lanes * dev->interpolation / dev->num_converters);
}

static void ad9144_set_nco_freq(struct ad9144_dev *dev, uint32_t sample_rate,
				uint32_t nco_freq)
{
	unsigned int mod_type;
	unsigned int i;
	uint64_t ftw, temp;
	uint8_t val;

	if (nco_freq == 0 || nco_freq >= sample_rate) {
		mod_type = AD9144_MOD_TYPE_NONE;
	} else if (sample_rate == nco_freq * 4) {
		mod_type = AD9144_MOD_TYPE_COARSE4;
	} else if (sample_rate == nco_freq * 8) {
		mod_type = AD9144_MOD_TYPE_COARSE8;
	} else {
		mod_type = AD9144_MOD_TYPE_FINE;
		//ftw = mul_u64_u32_div(1ULL << 48, nco_freq, sample_rate);
		temp = no_os_mul_u64_u32_shr(1ULL << 48, nco_freq, 0);
		ftw = no_os_div_u64(temp, sample_rate);

		for (i = 0; i < 6; i++) {
			ad9144_spi_write(dev, REG_FTW0 + i, ftw & 0xff);
			ftw >>= 8;
		}
	}

	ad9144_spi_read(dev, REG_DATAPATH_CTRL, &val);
	val &= ~AD9144_MOD_TYPE_MASK;
	val |= mod_type;
	ad9144_spi_write(dev, REG_DATAPATH_CTRL, val);

	if (mod_type == AD9144_MOD_TYPE_FINE)
		ad9144_spi_write(dev, REG_NCO_FTW_UPDATE, 1);

	no_os_mdelay(1);
}

static void ad9144_setup_samplerate(struct ad9144_dev *dev)
{
	unsigned int sample_rate;
	unsigned int serdes_plldiv, serdes_cdr;
	uint8_t val;
	unsigned long lane_rate_kHz;

	sample_rate = ad9144_get_sample_rate(dev);
	lane_rate_kHz = ad9144_get_lane_rate(dev, sample_rate);

	ad9144_set_nco_freq(dev, sample_rate, dev->fcenter_shift);

	/*
	 * Based on table 4 of the AD9144 datasheet Rev. B.
	 */
	if (lane_rate_kHz < 2880000) {
		serdes_cdr = 0x0a;
		serdes_plldiv = 0x06;
	} else if (lane_rate_kHz < 5750000) {
		serdes_cdr = 0x08;
		serdes_plldiv = 0x05;
	} else {
		serdes_cdr = 0x28;
		serdes_plldiv = 0x04;
	}

	// physical layer
	ad9144_spi_write(dev, REG_SYNTH_ENABLE_CNTRL, 0x00);	// disable serdes pll

	ad9144_spi_write(dev, REG_TERM_BLK1_CTRLREG0,
			 0x01);	// input termination calibration
	ad9144_spi_write(dev, REG_TERM_BLK2_CTRLREG0,
			 0x01);	// input termination calibration

	ad9144_spi_write(dev, REG_CDR_OPERATING_MODE_REG_0, serdes_cdr);

	ad9144_spi_write(dev, REG_CDR_RESET, 0x00);	// cdr reset
	ad9144_spi_write(dev, REG_CDR_RESET, 0x01);	// cdr reset

	ad9144_spi_write(dev, REG_REF_CLK_DIVIDER_LDO, serdes_plldiv);

	ad9144_spi_write(dev, REG_SYNTH_ENABLE_CNTRL, 0x01);	// enable serdes pll
	no_os_mdelay(20);

	ad9144_spi_read(dev, 0x281, &val);
	if ((val & 0x01) == 0x00)
		pr_err("SERDES PLL not locked.\n");

	ad9144_dac_calibrate(dev);
}

/*******************************************************************************
 * @brief ad9144_setup
********************************************************************************/
static int ad9144_setup(struct ad9144_dev *dev,
			struct jesd204_link *link_config)
{
	unsigned int sync_mode;
	unsigned int phy_mask;
	unsigned int pd_dac;
	unsigned int pd_clk;
	unsigned int val;
	unsigned int i;

	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_0, 0x00);	// single link - link 0

	// power-up and dac initialization

	pd_clk = NO_OS_GENMASK(7 - NO_OS_DIV_ROUND_UP(dev->num_converters, 2), 6);
	pd_dac = NO_OS_GENMASK(6 - dev->num_converters, 3);

	ad9144_spi_write(dev, REG_PWRCNTRL0, pd_dac); /* Power-up DACs */
	ad9144_spi_write(dev, REG_CLKCFG0, pd_clk); /* Power-up clocks */
	ad9144_spi_write(dev, REG_SYSREF_ACTRL0,
			 0x00);	// sysref - power up/falling edge

	ad9144_spi_write(dev, REG_DEV_CONFIG_9, 0xb7);	// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_10, 0x87);	// jesd termination

	ad9144_spi_write(dev, REG_SERDES_SPI_REG, 0x01);	// pclk == qbd master clock

	ad9144_spi_write_seq(dev, ad9144_required_device_config,
			     NO_OS_ARRAY_SIZE(ad9144_required_device_config));

	/*
	 * SERDES optimization according to table 39 AD9144 Rev. B
	 * datasheet.
	 */
	ad9144_spi_write(dev, 0x296, 0x03);
	ad9144_spi_write(dev, 0x28a, 0x7b);

	ad9144_spi_write(dev, REG_DEV_CONFIG_11, 0xb7);	// jesd termination
	ad9144_spi_write(dev, REG_DEV_CONFIG_12, 0x87);	// jesd termination

	ad9144_spi_write_seq(dev, ad9144_optimal_serdes_settings,
			     NO_OS_ARRAY_SIZE(ad9144_optimal_serdes_settings));

	if (dev->pll_enable)
		ad9144_setup_pll(dev);

	// digital data path

	switch (dev->interpolation) {
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
	ad9144_spi_write(dev, REG_INTERP_MODE, val);	// interpolation
	ad9144_spi_write(dev, REG_DATA_FORMAT, 0x00);	// 2's complement

	// transport layer

	phy_mask = 0xff;
	for (i = 0; i < link_config->num_lanes; i++)
		phy_mask &= ~NO_OS_BIT(dev->lane_mux[i]);

	ad9144_spi_write(dev, REG_MASTER_PD, 0x00);
	ad9144_spi_write(dev, REG_PHY_PD, phy_mask);

	ad9144_setup_link(dev, link_config);

	ad9144_spi_write(dev, REG_EQ_BIAS_REG, 0x62);	// equalizer

	// data link layer

	/* LMFC settings for link 0 */
	ad9144_spi_write(dev, REG_LMFC_DELAY_0, 0x00);	// lmfc delay
	ad9144_spi_write(dev, REG_LMFC_VAR_0, 0x0a);	// receive buffer delay

	/* LMFC settings for link 1 */
	ad9144_spi_write(dev, REG_LMFC_DELAY_1, 0x00);	// lmfc delay
	ad9144_spi_write(dev, REG_LMFC_VAR_1, 0x0a);	// receive buffer delay

	if (link_config->sysref.mode == JESD204_SYSREF_ONESHOT)
		sync_mode = 0x1;
	else
		sync_mode = 0x2;

	ad9144_spi_write(dev, REG_SYNC_CTRL, sync_mode);
	ad9144_spi_write(dev, REG_SYNC_CTRL, sync_mode | SYNCENABLE);
	ad9144_spi_write(dev, REG_SYNC_CTRL, sync_mode | SYNCENABLE | SYNCARM);

	ad9144_setup_samplerate(dev);

	if (dev->jdev)
		return 0;

	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_0, 0x01);	// enable link

	return 0;
}

static int ad9144_jesd204_link_setup(struct jesd204_dev *jdev,
				     enum jesd204_state_op_reason reason,
				     struct jesd204_link *lnk)
{
	struct ad9144_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9144_dev *dev = priv->dev;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	/*Enable Link*/

	ret = ad9144_setup(dev, lnk);
	if (ret != 0) {
		pr_err("Failed to enable JESD204 link (%d)\n", ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}


static int ad9144_jesd204_link_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9144_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9144_dev *dev = priv->dev;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	/*Enable Link*/
	ret = ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_0,
			       reason == JESD204_STATE_OP_REASON_INIT);
	if (ret != 0) {
		pr_err("Failed to enabled JESD204 link (%d)\n", ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9144_link_status_get(struct ad9144_dev *dev)
{
	int ret, i;
	uint8_t regs[4];

	for (i = 0; i < NO_OS_ARRAY_SIZE(regs); i++) {
		ret = ad9144_spi_read(dev, REG_CODEGRPSYNCFLG + i, &regs[i]);
		if (ret != 0) {
			pr_err("Get Link0 status failed\n");
			return -EIO;
		}
	}

	pr_info("Link0 code grp sync: %x\n", regs[0]);
	pr_info("Link0 frame sync stat: %x\n", regs[1]);
	pr_info("Link0 good checksum stat: %x\n", regs[2]);
	pr_info("Link0 init lane_sync stat: %x\n", regs[3]);
	pr_info("Link0 %d lanes @ %lu kBps\n", dev->num_lanes,
		ad9144_get_lane_rate(dev, ad9144_get_sample_rate(dev)));

	if (no_os_hweight8(regs[0]) != dev->num_lanes ||
	    regs[0] != regs[1] || regs[0] != regs[3])
		ret = -EFAULT;

	return 0;
}

static int ad9144_jesd204_link_running(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason,
				       struct jesd204_link *lnk)
{
	struct ad9144_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9144_dev *dev = priv->dev;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	ret = ad9144_link_status_get(dev);
	if (ret) {
		pr_err("Failed JESD204 link status (%d)\n", ret);
		return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_ad9144_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9144_jesd204_link_init,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_link = ad9144_jesd204_link_setup,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9144_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9144_jesd204_link_running,
		},
	},

	.max_num_links = 1,
	.num_retries = 2,
	.sizeof_priv = sizeof(struct ad9144_jesd204_priv),
};

/*******************************************************************************
 * @brief ad9144_setup_legacy
********************************************************************************/
int32_t ad9144_setup_legacy(struct ad9144_dev **device,
			    const struct ad9144_init_param *init_param)
{
	uint32_t serdes_plldiv;
	uint32_t serdes_cdr;
	uint8_t chip_id;
	uint8_t scratchpad;
	uint32_t val;
	int32_t ret;
	struct ad9144_dev *dev;

	dev = (struct ad9144_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret == -1)
		printf("%s : Device descriptor failed!\n", __func__);

	// reset
	ad9144_spi_write(dev, REG_SPI_INTFCONFA, SOFTRESET_M | SOFTRESET);
	ad9144_spi_write(dev, REG_SPI_INTFCONFA, init_param->spi3wire ? 0x00 : 0x18);
	no_os_mdelay(1);

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
			     NO_OS_ARRAY_SIZE(ad9144_required_device_config));
	ad9144_spi_write_seq(dev, ad9144_optimal_serdes_settings,
			     NO_OS_ARRAY_SIZE(ad9144_optimal_serdes_settings));

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
	no_os_mdelay(20);

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
	ad9144_spi_write(dev, REG_XBAR(0),
			 SRC_LANE0(init_param->lane_mux[0]) |
			 SRC_LANE1(init_param->lane_mux[1]));
	ad9144_spi_write(dev, REG_XBAR(1),
			 SRC_LANE2(init_param->lane_mux[2]) |
			 SRC_LANE3(init_param->lane_mux[3]));
	ad9144_spi_write(dev, REG_XBAR(2),
			 SRC_LANE4(init_param->lane_mux[4]) |
			 SRC_LANE5(init_param->lane_mux[5]));
	ad9144_spi_write(dev, REG_XBAR(3),
			 SRC_LANE6(init_param->lane_mux[6]) |
			 SRC_LANE7(init_param->lane_mux[7]));
	ad9144_spi_write(dev, REG_GENERAL_JRX_CTRL_0, 0x01);	// enable link

	// dac calibration
	ad9144_dac_calibrate(dev);

	*device = dev;

	return ret;
}

/*******************************************************************************
 * @brief ad9144_setup_jesd_fsm
********************************************************************************/
int32_t ad9144_setup_jesd_fsm(struct ad9144_dev **device,
			      const struct ad9144_init_param *init_param)
{
	struct ad9144_jesd204_priv *priv;
	uint8_t chip_id;
	uint8_t scratchpad;
	int32_t ret;
	struct ad9144_dev *dev;
	unsigned char i;

	dev = (struct ad9144_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret == -1)
		printf("%s : Device descriptor failed!\n", __func__);

	// reset
	ad9144_spi_write(dev, REG_SPI_INTFCONFA, SOFTRESET_M | SOFTRESET);
	ad9144_spi_write(dev, REG_SPI_INTFCONFA, init_param->spi3wire ? 0x00 : 0x18);
	no_os_mdelay(1);

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

	dev->pll_ref_frequency_khz = init_param->pll_ref_frequency_khz;
	dev->pll_dac_frequency_khz = init_param->pll_dac_frequency_khz;
	dev->pll_enable = init_param->pll_enable;
	dev->interpolation = init_param->interpolation;
	for (i = 0; i < 8; i++) {
		dev->lane_mux[i] = init_param->lane_mux[i];
	}
	dev->fcenter_shift = init_param->fcenter_shift;
	dev->num_converters = init_param->num_converters;
	dev->num_lanes = init_param->num_lanes;

	dev->sample_rate_khz = init_param->lane_rate_kbps / 40 *
			       dev->num_lanes * 2 /
			       dev->num_converters;

	ret = jesd204_dev_register(&dev->jdev, &jesd204_ad9144_init);
	if (ret)
		return ret;
	priv = jesd204_dev_priv(dev->jdev);;
	priv->dev = dev;

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
	no_os_mdelay(10);

	for (i = 0; i < dev->num_converters; i++) {
		ad9144_spi_write(dev, REG_CAL_INDX, NO_OS_BIT(i));	// read dac-i

		ret = ad9144_spi_check_status(dev, REG_CAL_CTRL, 0xc0, 0x80);
		if (ret == -1)
			printf("%s: dac-%d calibration failed!\n", __func__, i);
	}

	ad9144_spi_write(dev, REG_CAL_CLKDIV, 0x30);	// turn off cal clock

	return 0;
}

/*******************************************************************************
 * @brief Free the resources allocated by ad9144_setup_ functions.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9144_remove(struct ad9144_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

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
				printf("%s : short-pattern-test mismatch (%#06lx, %#06lx, %#06lx)!.\n",
				       __func__, dac, sample,
				       init_param->stpl_samples[dac][sample]);
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
	no_os_mdelay(500);

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

