/***************************************************************************//**
 * @file ad9152.c
 * @brief Implementation of AD9152 Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2015-2023(c) Analog Devices, Inc.
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
#include "ad9152.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

struct ad9152_jesd204_priv {
	struct ad9152_dev *dev;
};

#define AD9152_MOD_TYPE_NONE            (0x0 << 2)
#define AD9152_MOD_TYPE_FINE            (0x1 << 2)
#define AD9152_MOD_TYPE_COARSE4         (0x2 << 2)
#define AD9152_MOD_TYPE_COARSE8         (0x3 << 2)
#define AD9152_MOD_TYPE_MASK            (0x3 << 2)

/***************************************************************************//**
 * @brief ad9152_spi_read
 *******************************************************************************/
int32_t ad9152_spi_read(struct ad9152_dev *dev,
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
 * @brief ad9152_spi_write
 *******************************************************************************/
int32_t ad9152_spi_write(struct ad9152_dev *dev,
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
 * @brief ad9152_spi_check_status
 *******************************************************************************/
int32_t ad9152_spi_check_status(struct ad9152_dev *dev,
				uint16_t reg_addr,
				uint8_t reg_mask,
				uint8_t exp_reg_data)
{
	uint16_t timeout = 0;
	uint8_t status = 0;

	do {
		ad9152_spi_read(dev,
				reg_addr,
				&status);
		if ((status & reg_mask) == exp_reg_data) {
			return 0;
		} else {
			timeout++;
			no_os_mdelay(1);
		}
	} while (timeout < 100);

	return -1;
}

struct ad9152_reg_seq {
	uint16_t reg;
	uint16_t val;
};

/***************************************************************************//**
 * @brief ad9152_spi_write_seq
 *******************************************************************************/
int32_t ad9152_spi_write_seq(struct ad9152_dev *dev,
			     const struct ad9152_reg_seq *seq, uint32_t num)
{
	int32_t ret = 0;

	while (num) {
		ret |= ad9152_spi_write(dev, seq->reg, seq->val);
		num--;
		seq++;
	}

	return 0;
}

/*
 * Required SERDES PLL Register Settings, as per table 16 of the AD9152 datasheet.
 */
static const struct ad9152_reg_seq ad9152_optimal_serdes_settings[] = {
	{ 0x284, 0x62 },
	{ 0x285, 0xc9 },
	{ 0x286, 0x0e },
	{ 0x287, 0x12 },
	{ 0x28b, 0x00 },
	{ 0x290, 0x89 },
	{ 0x294, 0x24 },
	{ 0x297, 0x0d },
	{ 0x299, 0x02 },
	{ 0x29a, 0x8e },
	{ 0x29c, 0x2a },
	{ 0x29f, 0x7e },
	{ 0x2a0, 0x06 },
};

/*
 * DAC PLL fixed register values according to table 17 and table 18
 * of the AD9152 datasheet Rev. B.
 */
static const struct ad9152_reg_seq ad9152_pll_fixed_writes[] = {
	{ 0x08d, 0x7b },
	{ 0x1b0, 0x00 },
	{ 0x1b9, 0x24 },
	{ 0x1bc, 0x0d },
	{ 0x1be, 0x02 },
	{ 0x1bf, 0x8e },
	{ 0x1c0, 0x2a },
	{ 0x1c4, 0x7e },
	{ 0x1c1, 0x2c },

	{ 0x1b4, 0x78 },
	{ 0x08a, 0x12 },
	{ 0x087, 0x62 },
	{ 0x088, 0xc9 },
	{ 0x089, 0x0e },
};

/***************************************************************************//**
 * @brief ad9152_pll_setup
 *******************************************************************************/
static int32_t ad9152_pll_setup(struct ad9152_dev *dev)
{
	uint32_t fref, fdac;
	uint32_t lo_div_mode;
	uint32_t ref_div_mode = 0;
	uint8_t vco_param[4];
	uint32_t bcount;
	uint32_t fvco;
	int32_t ret;

	fref = dev->pll_ref_frequency_khz;
	fdac = dev->pll_dac_frequency_khz;

	if (fref > 1000000 || fref < 80000)
		return -1;

	if (fdac > 2250000 || fdac < 420000)
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

	if (fvco < 6840000) {
		vco_param[0] = 0x49;
		vco_param[1] = 0x1a;
		vco_param[2] = 0xc7;
		vco_param[3] = 0x08;
	} else if (fvco < 8690000) {
		vco_param[0] = 0x49;
		vco_param[1] = 0x12;
		vco_param[2] = 0xc9;
		vco_param[3] = 0x06;
	} else {
		vco_param[0] = 0x4d;
		vco_param[1] = 0x04;
		vco_param[2] = 0xc9;
		vco_param[3] = 0x06;
	}

	/* DAC PLL Configuration Procedure*/
	ad9152_spi_write_seq(dev, ad9152_pll_fixed_writes,
			     NO_OS_ARRAY_SIZE(ad9152_pll_fixed_writes));

	ad9152_spi_write(dev, REG_DACLOGENCNTRL, lo_div_mode);
	ad9152_spi_write(dev, REG_DACLDOCNTRL1, ref_div_mode);
	ad9152_spi_write(dev, REG_DACINTEGERWORD0, bcount);

	ad9152_spi_write(dev, REG_DACPLLT6, vco_param[0]);
	ad9152_spi_write(dev, REG_DACPLLTB, vco_param[1]);
	ad9152_spi_write(dev, REG_DACPLLT5, vco_param[2]);
	ad9152_spi_write(dev, REG_DACPLLT18, vco_param[3]);

	/* Enable DAC PLL */
	ad9152_spi_write(dev, REG_DACPLLCNTRL, 0x10);

	/* Verify that the DAC PLL is locked */
	ret = ad9152_spi_check_status(dev, REG_DACPLLSTATUS, 0x22, 0x22);
	if (ret == -1)
		printf("%s : DAC PLL NOT locked!.\n", __func__);

	return ret;
}

/***************************************************************************//**
 * @brief ad9152_set_nco
 *******************************************************************************/
int32_t ad9152_set_nco(struct ad9152_dev *dev, int32_t f_carrier_khz,
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

	if ((uint32_t)f_carrier_khz >= dev->sample_rate_khz / 2) {
		/* No modulation */
		modulation_type = MODULATION_TYPE(0);
	} else if (dev->sample_rate_khz == (uint32_t)f_carrier_khz * 4) {
		/* Coarse − f DAC /4 */
		modulation_type = MODULATION_TYPE(2);
	} else if (dev->sample_rate_khz == (uint32_t)f_carrier_khz * 8) {
		/* Coarse − f DAC /8 */
		modulation_type = MODULATION_TYPE(3);
	} else {
		/* NCO Fine Modulation */
		modulation_type = MODULATION_TYPE(1);
	}
	ret = ad9152_spi_read(dev, REG_DATAPATH_CTRL, &reg);
	if (ret != 0)
		return ret;
	reg = (reg & ~MODULATION_TYPE_MASK) | modulation_type;
	if (sel_sideband)
		reg |= SEL_SIDEBAND;
	else
		reg &= ~SEL_SIDEBAND;

	ret = ad9152_spi_write(dev, REG_DATAPATH_CTRL, reg);
	if (ret != 0)
		return ret;

	ftw = ((1ULL << 48) / dev->sample_rate_khz * f_carrier_khz);
	for (i = 0; i < 6; i++) {
		ret = ad9152_spi_write(dev, REG_FTW0 + i,
				       (ftw >> (8 * i)) & 0xFF);
		if (ret != 0)
			return ret;
	}

	phase_offset = (phase / 180) * (1 << 15);
	ret = ad9152_spi_write(dev, REG_NCO_PHASE_OFFSET0, phase_offset & 0xFF);
	if (ret != 0)
		return ret;
	ret = ad9152_spi_write(dev, REG_NCO_PHASE_OFFSET1, (phase_offset >> 8) &
			       0xFF);
	if (ret != 0)
		return ret;

	if (modulation_type  == MODULATION_TYPE(1)) {
		ret = ad9152_spi_write(dev, REG_NCO_FTW_UPDATE, FTW_UPDATE_REQ);
		if (ret != 0)
			return ret;
	}

	return ret;
}

/***************************************************************************//**
 * @brief ad9152_get_sample_rate
 *******************************************************************************/
static unsigned int ad9152_get_sample_rate(struct ad9152_dev *dev)
{
	unsigned int rate;

	if (dev->pll_enable)
		rate = dev->pll_dac_frequency_khz * 1000;
	else
		rate = dev->sample_rate_khz * 1000;

	return rate;
}

/***************************************************************************//**
 * @brief ad9152_get_lane_rate
 *******************************************************************************/
static unsigned long ad9152_get_lane_rate(struct jesd204_link *lnk)
{
	/*
	 * lanerate_khz = ((samplerate_hz / interpolation) * 20 * M / L) / 1000
	 *
	 * Slightly reordered here to avoid loss of precession or overflows.
	 */
	return NO_OS_DIV_ROUND_CLOSEST(lnk->sample_rate,
				       50 * lnk->num_lanes * lnk->sample_rate_div / lnk->num_converters);
}

/***************************************************************************//**
 * @brief ad9152_set_nco_freq
 *******************************************************************************/
static void ad9152_set_nco_freq(struct ad9152_dev *dev, uint32_t sample_rate,
				uint32_t nco_freq)
{
	unsigned int mod_type;
	unsigned int i;
	uint64_t ftw, temp;
	uint8_t val;

	if (nco_freq == 0 || nco_freq >= sample_rate) {
		mod_type = AD9152_MOD_TYPE_NONE;
	} else if (sample_rate == nco_freq * 4) {
		mod_type = AD9152_MOD_TYPE_COARSE4;
	} else if (sample_rate == nco_freq * 8) {
		mod_type = AD9152_MOD_TYPE_COARSE8;
	} else {
		mod_type = AD9152_MOD_TYPE_FINE;
		//ftw = mul_u64_u32_div(1ULL << 48, nco_freq, sample_rate);
		temp = no_os_mul_u64_u32_shr(1ULL << 48, nco_freq, 0);
		ftw = no_os_div_u64(temp, sample_rate);

		for (i = 0; i < 6; i++) {
			ad9152_spi_write(dev, REG_FTW0 + i, ftw & 0xff);
			ftw >>= 8;
		}
	}

	ad9152_spi_read(dev, REG_DATAPATH_CTRL, &val);
	val &= ~AD9152_MOD_TYPE_MASK;
	val |= mod_type;
	ad9152_spi_write(dev, REG_DATAPATH_CTRL, val);

	if (mod_type == AD9152_MOD_TYPE_FINE)
		ad9152_spi_write(dev, REG_NCO_FTW_UPDATE, 1);

	no_os_mdelay(1);
}

/*******************************************************************************
 * @brief ad9152_setup
 ********************************************************************************/
static int ad9152_setup(struct ad9152_dev *dev,
			struct jesd204_link *link_config)
{
	unsigned int serdes_plldiv, serdes_cdr;
	unsigned long serdes_lane_rate_kHz;
	unsigned int lane_mask;
	unsigned int sync_mode;
	unsigned int phy_mask;
	uint8_t vco_param[3];
	unsigned int M, L;
	unsigned int i, j;
	uint8_t pd_dac;
	uint8_t pd_clk;
	uint8_t val;

	/* 1. START UP THE DAC */

	/* Power-up and DAC initialization Settings*/
	pd_clk = link_config->num_converters == 1 ? NO_OS_BIT(2) : 0;
	pd_dac = link_config->num_converters == 1 ? NO_OS_BIT(5) : 0;

	val = pd_dac | pd_clk;
	ad9152_spi_write(dev, REG_PWRCNTRL0, val); // Power-up DACs and clocks
	ad9152_spi_write(dev, REG_CLKCFG0, 0x00);              // DUTY_EN = 0
	ad9152_spi_write(dev, REG_SYSREF_ACTRL0, 0x00);        // sysref - power up/falling edge
	ad9152_spi_write(dev, 0x1cd, 0xd8);                    // Band gap configuration.

	/* Required SERDES PLL Register Settings */
	serdes_lane_rate_kHz = ad9152_get_lane_rate(link_config);

	/*
	 * Based on table 36 of the AD9152 datasheet Rev. B.
	 */
	if (serdes_lane_rate_kHz < 7150000) {
		vco_param[0] = 0x02;
		vco_param[1] = 0x49;
		vco_param[2] = 0x7b;
	} else {
		vco_param[0] = 0x03;
		vco_param[1] = 0x4c;
		vco_param[2] = 0x2b;
	}

	/*
	 * SERDES PLL Register Settings according to table 16 AD9152 Rev. B
	 * datasheet.
	 */
	ad9152_spi_write_seq(dev, ad9152_optimal_serdes_settings,
			     NO_OS_ARRAY_SIZE(ad9152_optimal_serdes_settings));
	/*
	 * SERDES PLL VCO Control settings according to table 36 AD9152 Rev. B
	 * datasheet.
	 */
	ad9152_spi_write(dev, 0x296, vco_param[0]);
	ad9152_spi_write(dev, 0x291, vco_param[1]);
	ad9152_spi_write(dev, 0x28a, vco_param[2]);

	if (dev->pll_enable)
		ad9152_pll_setup(dev);

	/* 2. DIGITAL DATAPATH */

	/*
	 * Based on table 26 of the AD9152 datasheet Rev. B.
	 */
	switch (dev->interpolation) {
	case 2:
		val = 0x01;
		break;
	case 4:
		val = 0x02;
		break;
	case 8:
		val = 0x03;
		break;
	default:
		val = 0x00;
		break;
	}
	ad9152_spi_write(dev, REG_INTERP_MODE, val);   // interpolation
	ad9152_spi_write(dev, REG_DATA_FORMAT, 0x00);  // 2's complement

	/* 3. TRANSPORT LAYER */

	phy_mask = 0xff;
	for (i = 0; i < link_config->num_lanes; i++)
		phy_mask &= ~NO_OS_BIT(dev->lane_mux[i]);

	ad9152_spi_write(dev, REG_MASTER_PD, 0x00);     // Power up the JESD interface
	ad9152_spi_write(dev, REG_PHY_PD, phy_mask);    // Set the Used/Unused Lanes

	M = link_config->num_converters - 1;
	L = link_config->num_lanes - 1;
	lane_mask = (1 << link_config->num_lanes) - 1;

	/*
	 * Based on table 32 of the AD9152 datasheet Rev. B.
	 */
	for (i = 0; i < 2; i++) {
		j = 2 * i;

		val = dev->lane_mux[j];
		val |= dev->lane_mux[j + 1] << 3;
		ad9152_spi_write(dev, REG_XBAR(i), val);
	}

	/* Setup the SYSREF receiver */
	val = 0;
	if (!link_config->subclass)
		val |= NO_OS_BIT(4);
	if (!link_config->sysref.capture_falling_edge)
		val |= NO_OS_BIT(2);
	ad9152_spi_write(dev, REG_SYSREF_ACTRL0, val);

	ad9152_spi_write(dev, REG_GENERAL_JRX_CTRL_1, link_config->subclass);

	ad9152_spi_write(dev, REG_ILS_DID, link_config->device_id);
	ad9152_spi_write(dev, REG_ILS_BID, link_config->bank_id);

	val = L;                                                                   // L
	if (link_config->scrambling)
		val |= NO_OS_BIT(7);
	ad9152_spi_write(dev, REG_ILS_SCR_L, val);

	ad9152_spi_write(dev, REG_ILS_F, link_config->octets_per_frame - 1);      // F
	ad9152_spi_write(dev, REG_ILS_K, link_config->frames_per_multiframe - 1); // K
	ad9152_spi_write(dev, REG_ILS_M, M);                                      // M
	ad9152_spi_write(dev, REG_ILS_CS_N, 15);                                  // N

	val = 15;                                                                 // NP
	val |= link_config->subclass << 5;                                        // SUBCLASSV
	ad9152_spi_write(dev, REG_ILS_NP, val);

	val = link_config->samples_per_conv_frame - 1;                            // S
	val |= NO_OS_BIT(5);                                                      // JESDVER
	ad9152_spi_write(dev, REG_ILS_S, val);

	val = link_config->high_density ? NO_OS_BIT(7) : 0x0;                     // HD
	ad9152_spi_write(dev, REG_ILS_HD_CF, val);

	/* Static for now */
	ad9152_spi_write(dev, REG_KVAL, 0x01);

	ad9152_spi_write(dev, REG_LANEDESKEW, lane_mask);
	ad9152_spi_write(dev, REG_CTRLREG1, link_config->octets_per_frame);
	ad9152_spi_write(dev, REG_LANEENABLE, lane_mask);

	/*
	 * Length of the SYNC~ error pulse in PCLK cycles. According to the
	 * JESD204 standard the pulse length should be two frame clock cycles.
	 *
	 * 1 PCLK cycle = 4 octets
	 *   => SYNC~ pulse length = 2 * octets_per_frame / 4
	 */
	switch (link_config->octets_per_frame) {
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
	ad9152_spi_write(dev, REG_SYNCB_GEN_1, val << 4);

	/* 4. PHYSICAL LAYER */

	ad9152_spi_write(dev, REG_SERDES_SPI_REG, 0x01); // pclk == qbd master clock

	ad9152_set_nco_freq(dev, ad9152_get_sample_rate(dev), dev->fcenter_shift);

	/*
	 * Based on table 4 of the AD9152 datasheet Rev. B.
	 */
	if (serdes_lane_rate_kHz < 3090000) {
		/* OVERSAMPLING 1.44 to 3.09 [Gbps] */
		serdes_cdr = 0x0b;
		serdes_plldiv = 0x06;
	} else if (serdes_lane_rate_kHz < 6190000) {
		/* FULL RATE    2.88 to 6.19 [Gbps] */
		serdes_cdr = 0x09;
		serdes_plldiv = 0x05;
	} else {
		/* HALF RATE    5.75 to 12.38 [Gbps] */
		serdes_cdr = 0x29;
		serdes_plldiv = 0x04;
	}

	ad9152_spi_write(dev, REG_SYNTH_ENABLE_CNTRL, 0x00); // disable serdes pll

	ad9152_spi_write(dev, REG_TERM_BLK1_CTRLREG0, 0x01); // input termination calibration

	ad9152_spi_write(dev, REG_CDR_OPERATING_MODE_REG_0, serdes_cdr);

	ad9152_spi_write(dev, REG_CDR_RESET, 0x00);          // cdr reset
	ad9152_spi_write(dev, REG_CDR_RESET, 0x01);	         // cdr reset

	ad9152_spi_write(dev, REG_REF_CLK_DIVIDER_LDO, serdes_plldiv);

	ad9152_spi_write(dev, REG_SYNTH_ENABLE_CNTRL, 0x01);	// enable serdes pll
	no_os_mdelay(100);

	ad9152_spi_read(dev, 0x281, &val);
	if ((val & 0x01) == 0x00) {
		pr_err("SERDES PLL not locked.\n");
	}

	ad9152_spi_write(dev, REG_EQ_BIAS_REG, 0x62);	// equalizer low power mode

	/* 5. DATA LINK LAYER */

	/* LMFC settings for link 0 */
	ad9152_spi_write(dev, REG_LMFC_DELAY_0, 0x00);	// lmfc delay
	ad9152_spi_write(dev, REG_LMFC_VAR_0, 0x0a);	// receive buffer delay

	if (link_config->sysref.mode == JESD204_SYSREF_ONESHOT)
		sync_mode = 0x1;
	else
		sync_mode = 0x2;

	ad9152_spi_write(dev, REG_SYNC_CTRL, sync_mode);
	ad9152_spi_write(dev, REG_SYNC_CTRL, sync_mode | SYNCENABLE);
	ad9152_spi_write(dev, REG_SYNC_CTRL, sync_mode | SYNCENABLE | SYNCARM);

	if (dev->jdev)
		return 0;

	ad9152_spi_write(dev, REG_GENERAL_JRX_CTRL_0, 0x01);	// enable link

	return 0;
}

/***************************************************************************//**
 * @brief ad9152_setup_legacy
 *******************************************************************************/
int32_t ad9152_setup_legacy(struct ad9152_dev **device,
			    const struct ad9152_init_param *init_param)
{
	uint8_t chip_id;
	uint8_t pll_stat;
	int32_t ret;
	struct ad9152_dev *dev;

	dev = (struct ad9152_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);

	ret = 0;

	ad9152_spi_read(dev, REG_SPI_PRODIDL, &chip_id);
	if(chip_id != AD9152_CHIP_ID) {
		printf("AD9152: Invalid CHIP ID (0x%x)!\n", chip_id);
		return -1;
	}

	dev->pll_ref_frequency_khz = init_param->pll_ref_frequency_khz;
	dev->pll_dac_frequency_khz = init_param->pll_dac_frequency_khz;
	dev->pll_enable = init_param->pll_enable;
	dev->interpolation = init_param->interpolation;

	dev->fcenter_shift = init_param->fcenter_shift;
	dev->num_converters = init_param->num_converters;
	dev->num_lanes = init_param->num_lanes;


	// power-up and dac initialization
	no_os_mdelay(5);

	ad9152_spi_write(dev, REG_SPI_INTFCONFA, SOFTRESET_M | SOFTRESET);	// reset
	ad9152_spi_write(dev, REG_SPI_INTFCONFA, 0x00);	// reset

	no_os_mdelay(4);

	ad9152_spi_write(dev, 0x011, 0x00);	// dacs - power up everything
	ad9152_spi_write(dev, 0x080, 0x00);	// clocks - power up everything
	ad9152_spi_write(dev, 0x081, 0x00);	// sysref - power up/falling edge

	// digital data path

	ad9152_spi_write(dev, 0x112, 0x00);	// interpolation (bypass)
	ad9152_spi_write(dev, 0x110, 0x00);	// 2's complement

	// transport layer

	ad9152_spi_write(dev, 0x200, 0x00);	// phy - power up
	ad9152_spi_write(dev, 0x201, 0x00);	// phy - power up
	ad9152_spi_write(dev, 0x230, 0x28);	// half-rate CDR
	ad9152_spi_write(dev, 0x312, 0x20);	// half-rate CDR
	ad9152_spi_write(dev, 0x300, 0x00);	// single link - link 0
	ad9152_spi_write(dev, 0x450, 0x00);	// device id (0x400)
	ad9152_spi_write(dev, 0x451, 0x00);	// bank id (0x401)
	ad9152_spi_write(dev, 0x452, 0x04);	// lane-id (0x402)
	ad9152_spi_write(dev, 0x453, 0x83);	// descrambling, 4 lanes
	ad9152_spi_write(dev, 0x454, 0x00);	// octects per frame per lane (1)
	ad9152_spi_write(dev, 0x455, 0x1f);	// mult-frame - framecount (32)
	ad9152_spi_write(dev, 0x456, 0x01);	// no-of-converters (2)
	ad9152_spi_write(dev, 0x457, 0x0f);	// no CS bits, 16bit dac
	ad9152_spi_write(dev, 0x458, 0x2f);	// subclass 1, 16bits per sample
	ad9152_spi_write(dev, 0x459,
			 0x20);	// jesd204b, 1 samples per converter per device
	ad9152_spi_write(dev, 0x45a, 0x80);	// HD mode, no CS bits
	ad9152_spi_write(dev, 0x45d, 0x49);	// check-sum of 0x450 to 0x45c
	ad9152_spi_write(dev, 0x478, 0x01);	// ilas mf count
	ad9152_spi_write(dev, 0x46c, 0x0f);	// enable deskew for all lanes
	ad9152_spi_write(dev, 0x476, 0x01);	// frame - bytecount (1)
	ad9152_spi_write(dev, 0x47d, 0x0f);	// enable all lanes

	// physical layer
	ad9152_spi_write(dev, 0x2a6, 0x08);
	ad9152_spi_write(dev, 0x248, 0xaa);
	ad9152_spi_write(dev, 0x2aa, 0xb7);	// jesd termination
	ad9152_spi_write(dev, 0x2ab, 0x87);	// jesd termination
	ad9152_spi_write(dev, 0x2a7, 0x01);	// input termination calibration
	ad9152_spi_write(dev, 0x314, 0x01);	// pclk == qbd master clock
	ad9152_spi_write(dev, 0x230, 0x28);	// cdr mode - halfrate, no division
	ad9152_spi_write(dev, 0x206, 0x00);	// cdr reset
	ad9152_spi_write(dev, 0x206, 0x01);	// cdr reset
	ad9152_spi_write(dev, 0x289, 0x04);	// data-rate == 10Gbps
	ad9152_spi_write(dev, 0x280, 0x01);	// enable serdes pll
	ad9152_spi_write(dev, 0x280, 0x05);	// enable serdes calibration
	no_os_mdelay(20);

	ad9152_spi_read(dev, 0x281, &pll_stat);
	if (pll_stat == 0) {
		printf("AD9152: PLL is NOT locked!.\n");
		ret = -1;
	}

	ad9152_spi_write(dev, 0x268, 0x62);	// equalizer

	// data link layer

	ad9152_spi_write(dev, 0x301, 0x01);	// subclass-1
	ad9152_spi_write(dev, 0x304, 0x00);	// lmfc delay
	ad9152_spi_write(dev, 0x306, 0x0a);	// receive buffer delay
	ad9152_spi_write(dev, 0x03a, 0x01);	// sync-oneshot mode
	ad9152_spi_write(dev, 0x03a, 0x81);	// sync-enable
	ad9152_spi_write(dev, 0x03a, 0xc1);	// sysref-armed
	ad9152_spi_write(dev, 0x300, 0x01);	// enable link

	ad9152_spi_write(dev, 0x0e7, 0x30);	// turn off cal clock

	*device = dev;

	return ret;
}

/***************************************************************************//**
 * @brief ad9152_jesd204_link_init
 *******************************************************************************/
static int ad9152_jesd204_link_init(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason,
				    struct jesd204_link *lnk)
{
	struct ad9152_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9152_dev *dev = priv->dev;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	jesd204_copy_link_params(lnk, &dev->link_config);

	lnk->sample_rate = ad9152_get_sample_rate(dev);
	lnk->sample_rate_div = dev->interpolation;
	lnk->jesd_encoder = JESD204_ENCODER_8B10B;
	lnk->jesd_version = JESD204_VERSION_B;
	lnk->is_transmit = true;
	lnk->lane_ids = dev->link_config.lane_ids;

	return JESD204_STATE_CHANGE_DONE;
}

/***************************************************************************//**
 * @brief ad9152_jesd204_link_setup
 *******************************************************************************/
static int ad9152_jesd204_link_setup(struct jesd204_dev *jdev,
				     enum jesd204_state_op_reason reason,
				     struct jesd204_link *lnk)
{
	struct ad9152_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9152_dev *dev = priv->dev;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	/*Enable Link*/

	ret = ad9152_setup(dev, lnk);
	if (ret != 0) {
		pr_err("Failed to setup JESD204 link (%d)\n", ret);
		return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

/***************************************************************************//**
 * @brief ad9152_jesd204_link_enable
 *******************************************************************************/
static int ad9152_jesd204_link_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9152_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9152_dev *dev = priv->dev;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	/*Enable Link*/
	ret = ad9152_spi_write(dev, REG_GENERAL_JRX_CTRL_0,
			       reason == JESD204_STATE_OP_REASON_INIT);
	if (ret != 0) {
		pr_err("Failed to enable JESD204 link (%d)\n", ret);
		return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

/***************************************************************************//**
 * @brief ad9152_jesd204_link_running
 *******************************************************************************/
static int ad9152_jesd204_link_running(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason,
				       struct jesd204_link *lnk)
{
	struct ad9152_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9152_dev *dev = priv->dev;
	uint8_t lane_mask;
	uint8_t regs[4];
	int i;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	for (i = 0; i < NO_OS_ARRAY_SIZE(regs); i++)
		ad9152_spi_read(dev, REG_CODEGRPSYNCFLG + i, &regs[i]);

	lane_mask = (1 << lnk->num_lanes) - 1;
	pr_info("Link0 %d lanes @ %lu kBps\n", lnk->num_lanes,
		ad9152_get_lane_rate(lnk));
	pr_info("Link0 code grp sync %s, value: %x. \n",
		regs[0] == lane_mask ? "is achieved" : "is lost", regs[0]);
	pr_info("Link0 frame sync %s, value: %x. \n",
		regs[1] == lane_mask ? "is achieved" : "is lost", regs[1]);
	pr_info("Link0 good checksum %s, value: %x. \n",
		regs[2] == lane_mask ? "is correct" : "is not correct", regs[2]);
	pr_info("Link0 init lane sync %s, value: %x. \n",
		regs[3] == lane_mask ? "is achieved" : "is lost", regs[3]);

	if (no_os_hweight8(regs[0]) != lnk->num_lanes ||
		regs[0] != regs[1] || regs[0] != regs[3]) {
			pr_err("Failed JESD204 link running\n");
			return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_ad9152_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9152_jesd204_link_init,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_link = ad9152_jesd204_link_setup,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9152_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9152_jesd204_link_running,
		},
	},

	.max_num_links = 1,
	.num_retries = 2,
	.sizeof_priv = sizeof(struct ad9152_jesd204_priv),
};

/*******************************************************************************
 * @brief ad9152_setup_jesd_fsm
 ********************************************************************************/
int32_t ad9152_setup_jesd_fsm(struct ad9152_dev **device,
			      const struct ad9152_init_param *init_param)
{
	struct ad9152_jesd204_priv *priv;
	uint8_t chip_id;
	int32_t ret;
	struct ad9152_dev *dev;
	unsigned char i;

	dev = (struct ad9152_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret == -1)
		printf("%s : Device descriptor failed!\n", __func__);

	// Soft reset
	ad9152_spi_write(dev, REG_SPI_INTFCONFA, SOFTRESET_M | SOFTRESET);
	// Deassert reset, set 3-wire or 4-wire SPI
	ad9152_spi_write(dev, REG_SPI_INTFCONFA, init_param->spi3wire ? 0x00 : 0x18);
	no_os_mdelay(1);

	ad9152_spi_read(dev, REG_SPI_PRODIDL, &chip_id);
	if (chip_id != AD9152_CHIP_ID) {
		printf("%s : Invalid CHIP ID (0x%x).\n", __func__, chip_id);
		return -1;
	}

	dev->pll_ref_frequency_khz = init_param->pll_ref_frequency_khz;
	dev->pll_dac_frequency_khz = init_param->pll_dac_frequency_khz;
	dev->pll_enable = init_param->pll_enable;
	dev->interpolation = init_param->interpolation;

	dev->fcenter_shift = init_param->fcenter_shift;
	dev->num_converters = init_param->num_converters;
	dev->num_lanes = init_param->num_lanes;

	for (i = 0; i < dev->num_lanes; i++)
		dev->lane_mux[i] = init_param->lane_mux[i];

	dev->sample_rate_khz = init_param->sampling_frequency_khz;

	ret = jesd204_dev_register(&dev->jdev, &jesd204_ad9152_init);
	if (ret)
		return ret;
	priv = jesd204_dev_priv(dev->jdev);
	priv->dev = dev;

	*device = dev;

	return ret;
}

/*******************************************************************************
 * @brief Free the resources allocated by ad9152_setup_ functions.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
 *******************************************************************************/
int32_t ad9152_remove(struct ad9152_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief ad9152_status - return the status of the JESD interface
 *******************************************************************************/
int32_t ad9152_status(struct ad9152_dev *dev)
{
	uint8_t status = 0;
	int32_t ret = 0;
	uint32_t lane_mask;

	lane_mask = (1 << dev->num_lanes) - 1;

	// check for jesd status on all lanes
	// failures on top are 100% guaranteed to make subsequent status checks fail

	ad9152_spi_read(dev, REG_CODEGRPSYNCFLG, &status);
	if (status != lane_mask) {
		printf("%s : CGS NOT received (%x)!.\n", __func__, status);
		ret = -1;
	}
	ad9152_spi_read(dev, REG_INITLANESYNCFLG, &status);
	if (status != lane_mask) {
		printf("%s : ILAS NOT received (%x)!.\n", __func__, status);
		ret = -1;
	}
	ad9152_spi_read(dev, REG_FRAMESYNCFLG, &status);
	if (status != lane_mask) {
		printf("%s : framer OUT OF SYNC (%x)!.\n", __func__, status);
		ret = -1;
	}
	ad9152_spi_read(dev, REG_GOODCHKSUMFLG, &status);
	if (status != lane_mask) {
		printf("%s : check-sum MISMATCH (%x)!.\n", __func__, status);
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
 * @brief ad9152_short_pattern_test
 *******************************************************************************/
int32_t ad9152_short_pattern_test(struct ad9152_dev *dev,
				  const struct ad9152_init_param *init_param)
{
	uint32_t dac = 0;
	uint32_t sample = 0;
	int32_t ret = 0;

	for (dac = 0; dac < dev->num_converters; dac++) {
		for (sample = 0; sample < 4; sample++) {
			ad9152_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x00));
			ad9152_spi_write(dev, REG_SHORT_TPL_TEST_2,
					 (init_param->stpl_samples[dac][sample] >> 8));
			ad9152_spi_write(dev, REG_SHORT_TPL_TEST_1,
					 (init_param->stpl_samples[dac][sample] >> 0));
			ad9152_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x01));
			ad9152_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x03));
			ad9152_spi_write(dev, REG_SHORT_TPL_TEST_0,
					 ((sample << 4) | (dac << 2) | 0x01));

			ret = ad9152_spi_check_status(dev,
						      REG_SHORT_TPL_TEST_3,
						      0x01, 0x00);
			if (ret == -1)
				printf("%s : short-pattern-test mismatch (%#06x, %#06x, %#06x)!.\n",
				       __func__, dac, sample,
				       init_param->stpl_samples[dac][sample]);
		}
	}
	return 0;
}

/***************************************************************************//**
 * @brief ad9152_datapath_prbs_test
 *******************************************************************************/
int32_t ad9152_datapath_prbs_test(struct ad9152_dev *dev,
				  const struct ad9152_init_param *init_param)
{
	uint8_t status = 0;
	int32_t ret = 0;

	if (init_param->interpolation == 1)
		return(ret);

	ad9152_spi_write(dev, REG_PRBS, ((init_param->prbs_type << 2) | 0x03));
	ad9152_spi_write(dev, REG_PRBS, ((init_param->prbs_type << 2) | 0x01));
	no_os_mdelay(100);

	ad9152_spi_read(dev, REG_PRBS, &status);
	if ((status & 0xc0) != 0xc0) {
		printf("AD9152: PRBS OUT OF SYNC (%x)!.\n", status);
		ret = -1;
	}
	ad9152_spi_read(dev, REG_PRBS_ERROR_I, &status);
	if (status != 0x00) {
		printf("AD9152: PRBS I channel ERRORS (%x)!.\n", status);
		ret = -1;
	}
	ad9152_spi_read(dev, REG_PRBS_ERROR_Q, &status);
	if (status != 0x00) {
		printf("AD9152: PRBS Q channel ERRORS (%x)!.\n", status);
		ret = -1;
	}

	return ret;
}

