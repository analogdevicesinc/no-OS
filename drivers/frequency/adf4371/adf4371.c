/***************************************************************************//**
 *   @file   adf4371.c
 *   @brief  Implementation of ADF4371 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "adf4371.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADF4371_WRITE			(0 << 15)
#define ADF4371_READ			(1 << 15)
#define ADF4371_ADDR(x)			((x) & 0x7FFF)

/* Registers address macro */
#define ADF4371_REG(x)			(x)

/* ADF4371_REG0 */
#define ADF4371_ADDR_ASC_MSK		NO_OS_BIT(2)
#define ADF4371_ADDR_ASC(x)		no_os_field_prep(ADF4371_ADDR_ASC_MSK, x)
#define ADF4371_ADDR_ASC_R_MSK		NO_OS_BIT(5)
#define ADF4371_ADDR_ASC_R(x)		no_os_field_prep(ADF4371_ADDR_ASC_R_MSK, x)
#define ADF4371_SDO_ACT_MSK		NO_OS_BIT(3)
#define ADF4371_SDO_ACT(x)		no_os_field_prep(ADF4371_SDO_ACT_MSK, x)
#define ADF4371_SDO_ACT_R_MSK		NO_OS_BIT(4)
#define ADF4371_SDO_ACT_R(x)		no_os_field_prep(ADF4371_SDO_ACT_R_MSK, x)
#define ADF4371_RESET_CMD		0x81

/* ADF4371_REG17 */
#define ADF4371_FRAC2WORD_L_MSK		NO_OS_GENMASK(7, 1)
#define ADF4371_FRAC2WORD_L(x)		no_os_field_prep(ADF4371_FRAC2WORD_L_MSK, x)
#define ADF4371_FRAC1WORD_MSK		NO_OS_BIT(0)
#define ADF4371_FRAC1WORD(x)		no_os_field_prep(ADF4371_FRAC1WORD_MSK, x)

/* ADF4371_REG18 */
#define ADF4371_FRAC2WORD_H_MSK		NO_OS_GENMASK(6, 0)
#define ADF4371_FRAC2WORD_H(x)		no_os_field_prep(ADF4371_FRAC2WORD_H_MSK, x)

/* ADF4371_REG1A */
#define ADF4371_MOD2WORD_MSK		NO_OS_GENMASK(5, 0)
#define ADF4371_MOD2WORD(x)		no_os_field_prep(ADF4371_MOD2WORD_MSK, x)

/* ADF4371_REG1E */
#define ADF4371_CP_CURRENT_MSK		NO_OS_GENMASK(7, 4)
#define ADF4371_CP_CURRENT(x)		no_os_field_prep(ADF4371_CP_CURRENT_MSK, x)
#define ADF4371_PD_POL_MSK		NO_OS_BIT(3)
#define ADF4371_PD_POL(x)		no_os_field_prep(ADF4371_PD_POL_MSK, x)

/* ADF4371_REG20 */
#define ADF4371_MUXOUT_MSK		NO_OS_GENMASK(7, 4)
#define ADF4371_MUXOUT(x)		no_os_field_prep(ADF4371_MUXOUT_MSK, x)
#define ADF4371_MUXOUT_LVL_MSK		NO_OS_BIT(2)
#define ADF4371_MUXOUT_LVL(x)		no_os_field_prep(ADF4371_MUXOUT_LVL_MSK, x)
#define ADF4371_MUXOUT_EN_MSK		NO_OS_BIT(3)
#define ADF4371_MUXOUT_EN(x)		no_os_field_prep(ADF4371_MUXOUT_EN_MSK, x)

/* ADF4371_REG22 */
#define ADF4371_REFIN_MODE_MASK		NO_OS_BIT(6)
#define ADF4371_REFIN_MODE(x)		no_os_field_prep(ADF4371_REFIN_MODE_MASK, x)

/* ADF4371_REG24 */
#define ADF4371_RF_DIV_SEL_MSK		NO_OS_GENMASK(6, 4)
#define ADF4371_RF_DIV_SEL(x)		no_os_field_prep(ADF4371_RF_DIV_SEL_MSK, x)

/* ADF4371_REG25 */
#define ADF4371_MUTE_LD_MSK		NO_OS_BIT(7)
#define ADF4371_MUTE_LD(x)		no_os_field_prep(ADF4371_MUTE_LD_MSK, x)

/* ADF4371_REG32 */
#define ADF4371_TIMEOUT_MSK		NO_OS_GENMASK(1, 0)
#define ADF4371_TIMEOUT(x)		no_os_field_prep(ADF4371_TIMEOUT_MSK, x)

/* ADF4371_REG34 */
#define ADF4371_VCO_ALC_TOUT_MSK	NO_OS_GENMASK(4, 0)
#define ADF4371_VCO_ALC_TOUT(x)		no_os_field_prep(ADF4371_VCO_ALC_TOUT_MSK, x)

/* Specifications */
#define ADF4371_MIN_VCO_FREQ		4000000000ULL /* 4000 MHz */
#define ADF4371_MAX_VCO_FREQ		8000000000ULL /* 8000 MHz */
#define ADF4371_MAX_OUT_RF8_FREQ	ADF4371_MAX_VCO_FREQ /* Hz */
#define ADF4371_MIN_OUT_RF8_FREQ	(ADF4371_MIN_VCO_FREQ / 64) /* Hz */
#define ADF4371_MAX_OUT_RF16_FREQ	(ADF4371_MAX_VCO_FREQ * 2) /* Hz */
#define ADF4371_MIN_OUT_RF16_FREQ	(ADF4371_MIN_VCO_FREQ * 2) /* Hz */
#define ADF4371_MAX_OUT_RF32_FREQ	(ADF4371_MAX_VCO_FREQ * 4) /* Hz */
#define ADF4371_MIN_OUT_RF32_FREQ	(ADF4371_MIN_VCO_FREQ * 4) /* Hz */

#define ADF4371_MAX_FREQ_PFD		250000000UL /* Hz */
#define ADF4371_MAX_FREQ_REFIN		600000000UL /* Hz */

/* MOD1 is a 24-bit primary modulus with fixed value of 2^25 */
#define ADF4371_MODULUS1		33554432ULL
/* MOD2 is the programmable, 14-bit auxiliary fractional modulus */
#define ADF4371_MAX_MODULUS2		NO_OS_BIT(14)

#define ADF4371_CHECK_RANGE(freq, range) \
	((freq > ADF4371_MAX_ ## range) || (freq < ADF4371_MIN_ ## range))

enum {
	ADF4371_FREQ,
	ADF4371_POWER_DOWN,
	ADF4371_CHANNEL_NAME,
	ADF4371_MUXOUT_ENABLE
};

enum {
	ADF4371_CH_RF8,
	ADF4371_CH_RFAUX8,
	ADF4371_CH_RF16,
	ADF4371_CH_RF32
};

enum adf4371_variant {
	ADF4371,
	ADF4372
};

enum adf4371_muxout {
	ADF4371_TRISTATE = 0x00,
	ADF4371_DIG_LOCK = 0x01,
	ADF4371_CH_PUMP_UP = 0x02,
	ADF4371_CH_PUMP_DOWN = 0x03,
	ADF4371_RDIV2 = 0x04,
	ADF4371_N_DIV_OUT = 0x05,
	ADF4371_VCO_TEST = 0x06,
	ADF4371_HIGH = 0x08,
	ADF4371_VCO_CALIB_R_BAND = 0x09,
	ADF4371_VCO_CALIB_N_BAND = 0x0A
};

struct adf4371_pwrdown {
	uint32_t reg;
	uint32_t bit;
};

static const int32_t adf4371_cp_current_microamp[] = {
	350, 700, 1050, 1400, 1750, 2100, 2450, 2800,
	3150, 3500, 3850, 4200, 4550, 4900, 5250, 5600
};

static const struct adf4371_pwrdown adf4371_pwrdown_ch[4] = {
	[ADF4371_CH_RF8] = { ADF4371_REG(0x25), 2 },
	[ADF4371_CH_RFAUX8] = { ADF4371_REG(0x72), 3 },
	[ADF4371_CH_RF16] = { ADF4371_REG(0x25), 3 },
	[ADF4371_CH_RF32] = { ADF4371_REG(0x25), 4 },
};

struct reg_sequence {
	uint16_t reg;
	uint8_t val;
};

static const struct reg_sequence adf4371_reg_defaults[] = {
	{ ADF4371_REG(0x01), 0x00 },
	{ ADF4371_REG(0x12), 0x40 },
	{ ADF4371_REG(0x1E), 0x48 },
	{ ADF4371_REG(0x20), 0x14 },
	{ ADF4371_REG(0x22), 0x00 },
	{ ADF4371_REG(0x23), 0x00 },
	{ ADF4371_REG(0x24), 0x80 },
	{ ADF4371_REG(0x25), 0x07 },
	{ ADF4371_REG(0x27), 0xC5 },
	{ ADF4371_REG(0x28), 0x83 },
	{ ADF4371_REG(0x2C), 0x44 },
	{ ADF4371_REG(0x2D), 0x11 },
	{ ADF4371_REG(0x2E), 0x12 },
	{ ADF4371_REG(0x2F), 0x94 },
	{ ADF4371_REG(0x32), 0x04 },
	{ ADF4371_REG(0x35), 0xFA },
	{ ADF4371_REG(0x36), 0x30 },
	{ ADF4371_REG(0x39), 0x07 },
	{ ADF4371_REG(0x3A), 0x55 },
	{ ADF4371_REG(0x3E), 0x0C },
	{ ADF4371_REG(0x3F), 0x80 },
	{ ADF4371_REG(0x40), 0x50 },
	{ ADF4371_REG(0x41), 0x28 },
	{ ADF4371_REG(0x47), 0xC0 },
	{ ADF4371_REG(0x52), 0xF4 },
	{ ADF4371_REG(0x70), 0x03 },
	{ ADF4371_REG(0x71), 0x60 },
	{ ADF4371_REG(0x72), 0x32 },
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * SPI register write to device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_write(struct adf4371_dev *dev,
			     uint16_t reg,
			     uint8_t val)
{
	uint8_t buf[3];
	uint16_t cmd;

	cmd = ADF4371_WRITE | ADF4371_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	buf[2] = val;

	return no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
}

/**
 * SPI register write bulk to device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The buffer of data to be written.
 * @param size - The buffer size.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_write_bulk(struct adf4371_dev *dev,
				  uint16_t reg,
				  uint8_t *val,
				  uint8_t size)
{
	uint8_t buf[10];
	uint16_t cmd;
	uint8_t i;

	cmd = ADF4371_WRITE | ADF4371_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	for (i = 0; i < size; i++)
		buf[2 + i] = val[i];

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2 + size);
}

/**
 * SPI register read from device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_read(struct adf4371_dev *dev,
			    uint16_t reg,
			    uint8_t *val)
{
	uint8_t buf[3];
	uint16_t cmd;
	int ret;

	cmd = ADF4371_READ | ADF4371_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	buf[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	*val = buf[2];

	return 0;
}

/**
 * SPI register update.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param reg - The register address.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_update(struct adf4371_dev *dev,
			      uint16_t reg,
			      uint8_t mask,
			      uint8_t val)
{
	uint8_t read_val;
	int32_t ret = 0;

	ret = adf4371_read(dev, reg, &read_val);
	read_val &= ~mask;
	read_val |= val;
	ret |= adf4371_write(dev, reg, read_val);

	return ret;
}

/**
 * Get the output frequency of one channel.
 * @param dev - The device structure.
 * @param channel - The selected channel.
 * @return The ouput frequency.
 */
static uint64_t adf4371_pll_fract_n_get_rate(struct adf4371_dev *dev,
		uint32_t channel)
{
	uint64_t val, tmp;
	uint32_t ref_div_sel;

	if (dev->mod2 == 0)
		return 0;

	val = (((uint64_t)dev->integer * ADF4371_MODULUS1) + dev->fract1) * dev->fpfd;
	tmp = (uint64_t)dev->fract2 * dev->fpfd;
	no_os_do_div(&tmp, dev->mod2);
	val += tmp + ADF4371_MODULUS1 / 2;

	if (channel == ADF4371_CH_RF8 || channel == ADF4371_CH_RFAUX8)
		ref_div_sel = dev->rf_div_sel;
	else
		ref_div_sel = 0;

	no_os_do_div(&val, ADF4371_MODULUS1 * (1 << ref_div_sel));

	if (channel == ADF4371_CH_RF16)
		val <<= 1;
	else if (channel == ADF4371_CH_RF32)
		val <<= 2;

	return val;
}

/**
 * Compute PLL parameters.
 * @param vco - The VCO frequency.
 * @param pfd - The PFD frequency.
 * @param integer - The integer division factor.
 * @param fract1 - The fractionality.
 * @param fract2 - The auxiliary fractionality.
 * @param mod2 - The auxiliary modulus.
 * @return None.
 */
static void adf4371_pll_fract_n_compute(uint64_t vco,
					uint64_t pfd,
					uint32_t *integer,
					uint32_t *fract1,
					uint32_t *fract2,
					uint32_t *mod2)
{
	uint64_t tmp;
	uint32_t gcd_div;

	tmp = no_os_do_div(&vco, pfd);
	tmp = tmp * ADF4371_MODULUS1;
	*fract2 = no_os_do_div(&tmp, pfd);

	*integer = vco;
	*fract1 = tmp;

	*mod2 = pfd;

	while (*mod2 > ADF4371_MAX_MODULUS2) {
		*mod2 >>= 1;
		*fract2 >>= 1;
	}

	gcd_div = no_os_greatest_common_divisor(*fract2, *mod2);
	*mod2 /= gcd_div;
	*fract2 /= gcd_div;
}

/**
 * Set the output frequency for one channel.
 * @param dev - The device structure.
 * @param freq - The output frequency.
 * @param channel - The selected channel.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_set_freq(struct adf4371_dev *dev,
				uint64_t freq,
				uint32_t channel)
{
	uint32_t cp_bleed;
	uint8_t int_mode = 0;
	int32_t ret;

	switch (channel) {
	case ADF4371_CH_RF8:
	case ADF4371_CH_RFAUX8:
		if (ADF4371_CHECK_RANGE(freq, OUT_RF8_FREQ))
			return -1;

		dev->rf_div_sel = 0;

		while (freq < ADF4371_MIN_VCO_FREQ) {
			freq <<= 1;
			dev->rf_div_sel++;
		}
		break;
	case ADF4371_CH_RF16:
		/* ADF4371 RF16 8000...16000 MHz */
		if (ADF4371_CHECK_RANGE(freq, OUT_RF16_FREQ))
			return -1;

		freq >>= 1;
		break;
	case ADF4371_CH_RF32:
		/* ADF4371 RF32 16000...32000 MHz */
		if (ADF4371_CHECK_RANGE(freq, OUT_RF32_FREQ))
			return -1;

		freq >>= 2;
		break;
	default:
		return -1;
	}

	adf4371_pll_fract_n_compute(freq, dev->fpfd, &dev->integer, &dev->fract1,
				    &dev->fract2, &dev->mod2);

	dev->buf[0] = dev->integer >> 8;
	dev->buf[1] = 0x40; /* REG12 default */
	dev->buf[2] = 0x00;
	dev->buf[3] = dev->fract1 & 0xFF;
	dev->buf[4] = dev->fract1 >> 8;
	dev->buf[5] = dev->fract1 >> 16;
	dev->buf[6] = ADF4371_FRAC2WORD_L(dev->fract2 & 0x7F) |
		      ADF4371_FRAC1WORD(dev->fract1 >> 24);
	dev->buf[7] = ADF4371_FRAC2WORD_H(dev->fract2 >> 7);
	dev->buf[8] = dev->mod2 & 0xFF;
	dev->buf[9] = ADF4371_MOD2WORD(dev->mod2 >> 8);

	ret = adf4371_write_bulk(dev, ADF4371_REG(0x11), dev->buf, 10);
	if (ret < 0)
		return ret;
	/*
	 * The R counter allows the input reference frequency to be
	 * divided down to produce the reference clock to the PFD
	 */
	ret = adf4371_write(dev, ADF4371_REG(0x1F), dev->ref_div_factor);
	if (ret < 0)
		return ret;

	ret = adf4371_update(dev, ADF4371_REG(0x24),
			     ADF4371_RF_DIV_SEL_MSK,
			     ADF4371_RF_DIV_SEL(dev->rf_div_sel));
	if (ret < 0)
		return ret;

	/*
	 * The optimum bleed current is set by ((4/N) × ICP)/3.75,
	 * where ICP is the charge pump current in μA
	 */
	cp_bleed = NO_OS_DIV_ROUND_UP(400 * dev->cp_settings.icp, dev->integer * 375);
	cp_bleed = no_os_clamp(cp_bleed, 1U, 255U);
	ret = adf4371_write(dev, ADF4371_REG(0x26), cp_bleed);
	if (ret < 0)
		return ret;
	/*
	 * Set to 1 when in INT mode (when FRAC1 = FRAC2 = 0),
	 * and set to 0 when in FRAC mode.
	 */
	if (dev->fract1 == 0 && dev->fract2 == 0)
		int_mode = 0x01;

	ret = adf4371_write(dev, ADF4371_REG(0x2B), int_mode);
	if (ret < 0)
		return ret;

	return adf4371_write(dev, ADF4371_REG(0x10), dev->integer & 0xFF);
}

/**
 * Power down one channel.
 * @param dev - The device structure.
 * @param channel - The selected channel.
 * @param power_down - The power down state.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_channel_power_down(struct adf4371_dev *dev,
		uint32_t channel,
		bool power_down)
{
	uint32_t bit, reg;
	uint8_t readval;
	int32_t ret;

	reg = adf4371_pwrdown_ch[channel].reg;
	bit = adf4371_pwrdown_ch[channel].bit;
	ret = adf4371_read(dev, reg, &readval);
	if (ret < 0)
		return ret;

	readval &= ~NO_OS_BIT(bit);
	readval |= (!power_down << bit);

	return adf4371_write(dev, reg, readval);
}

/**
 * Configure the channels.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_channel_config(struct adf4371_dev *dev)
{
	uint64_t rate;
	int32_t i, ret;

	for (i = 0; i < dev->num_channels; i++) {
		if (dev->channel_cfg[i].freq == 0)
			continue;

		rate = adf4371_pll_fract_n_get_rate(dev, i);
		if (rate == 0) {
			ret = adf4371_set_freq(dev, dev->channel_cfg[i].freq, i);
			if (ret < 0)
				return ret;
		} else if (rate != dev->channel_cfg[i].freq) {
			printf("Clock rate for chanel %"PRId32" is not in sync\n", i);
			return -1;
		}
		/* Powerup channel */
		if (dev->channel_cfg[i].enable) {
			ret = adf4371_channel_power_down(dev, i, false);
			if (ret < 0)
				return ret;
		}
	}

	return 0;
}

/**
 * Setup the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4371_setup(struct adf4371_dev *dev)
{
	uint32_t vco_alc_timeout = 1;
	uint32_t synth_timeout = 2;
	uint32_t vco_band_div;
	uint32_t timeout = 1;
	uint32_t tmp;
	bool en = true;
	uint8_t mask;
	uint8_t val;
	int32_t ret;
	int32_t i;

	ret = adf4371_write(dev, ADF4371_REG(0x0), ADF4371_RESET_CMD);
	if (ret < 0)
		return ret;

	if (dev->spi_3wire_en)
		en = false;

	ret = adf4371_update(dev, ADF4371_REG(0x0),
			     ADF4371_SDO_ACT_MSK | ADF4371_SDO_ACT_R_MSK,
			     ADF4371_SDO_ACT(en) | ADF4371_SDO_ACT_R(en));
	if (ret < 0)
		return ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(adf4371_reg_defaults); i++) {
		ret = adf4371_write(dev,
				    adf4371_reg_defaults[i].reg,
				    adf4371_reg_defaults[i].val);
		if (ret < 0)
			return ret;
	}

	if (dev->differential_ref_clk) {
		ret = adf4371_update(dev, ADF4371_REG(0x22),
				     ADF4371_REFIN_MODE_MASK,
				     ADF4371_REFIN_MODE(1));
		if (ret < 0)
			return ret;
	}

	mask = ADF4371_PD_POL_MSK | ADF4371_CP_CURRENT_MSK;
	val = ADF4371_PD_POL(dev->pd_pol) |
	      ADF4371_CP_CURRENT(dev->cp_settings.regval);

	/* Set the phase detector polarity and the charge pump current */
	ret = adf4371_update(dev, ADF4371_REG(0x1E),  mask, val);
	if (ret < 0)
		return ret;

	/* Mute to Lock Detect */
	if (dev->mute_till_lock_en) {
		ret = adf4371_update(dev, ADF4371_REG(0x25),
				     ADF4371_MUTE_LD_MSK,
				     ADF4371_MUTE_LD(1));
		if (ret < 0)
			return ret;
	}

	ret = adf4371_update(dev, ADF4371_REG(0x20),
			     ADF4371_MUXOUT_MSK |
			     ADF4371_MUXOUT_EN_MSK |
			     ADF4371_MUXOUT_LVL_MSK,
			     ADF4371_MUXOUT(dev->muxout_default_mode) |
			     ADF4371_MUXOUT_EN(dev->muxout_en) |
			     ADF4371_MUXOUT_LVL(!dev->muxout_1v8_en));
	if (ret < 0)
		return ret;

	/* Set address in ascending order, so the write_bulk() will work */
	ret = adf4371_update(dev, ADF4371_REG(0x0),
			     ADF4371_ADDR_ASC_MSK | ADF4371_ADDR_ASC_R_MSK,
			     ADF4371_ADDR_ASC(1) | ADF4371_ADDR_ASC_R(1));
	if (ret < 0)
		return ret;

	/*
	 * Calculate and maximize PFD frequency
	 * fPFD = REFIN × ((1 + D)/(R × (1 + T)))
	 * Where D is the REFIN doubler bit, T is the reference divide by 2,
	 * R is the reference division factor
	 * TODO: it is assumed D and T equal 0.
	 */
	do {
		dev->ref_div_factor++;
		dev->fpfd = dev->clkin_freq / dev->ref_div_factor;
	} while (dev->fpfd > ADF4371_MAX_FREQ_PFD);

	/* Calculate Timeouts */
	vco_band_div = NO_OS_DIV_ROUND_UP(dev->fpfd, 2400000U);

	tmp = NO_OS_DIV_ROUND_CLOSEST(dev->fpfd, 1000000U);
	do {
		timeout++;
		if (timeout > 1023) {
			timeout = 2;
			synth_timeout++;
		}
	} while (synth_timeout * 1024 + timeout <= 20 * tmp);

	do {
		vco_alc_timeout++;
	} while (vco_alc_timeout * 1024 - timeout <= 50 * tmp);

	dev->buf[0] = vco_band_div;
	dev->buf[1] = timeout & 0xFF;
	dev->buf[2] = ADF4371_TIMEOUT(timeout >> 8) | 0x04;
	dev->buf[3] = synth_timeout;
	dev->buf[4] = ADF4371_VCO_ALC_TOUT(vco_alc_timeout);

	ret = adf4371_write_bulk(dev, ADF4371_REG(0x30), dev->buf, 5);
	if (ret < 0)
		return ret;

	return adf4371_channel_config(dev);
}

/**
 * Recalculate rate corresponding to a channel.
 * @param dev - The device structure.
 * @param chan - Channel number.
 * @param rate - Channel rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adf4371_clk_recalc_rate(struct adf4371_dev *dev, uint32_t chan,
				uint64_t *rate)
{
	if (chan > dev->num_channels)
		return -1;

	*rate = adf4371_pll_fract_n_get_rate(dev, chan);

	return 0;
}

/**
 * Calculate closest possible rate
 * @param dev - The device structure
 * @param rate - The desired rate.
 * @param rounded_rate - The closest possible rate of desired rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adf4371_clk_round_rate(struct adf4371_dev *dev, uint64_t rate,
			       uint64_t *rounded_rate)
{
	*rounded_rate = rate;

	return 0;
}

/**
 * Set channel rate.
 * @param dev - The device structure.
 * @param chan - Channel number.
 * @param rate - Channel rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adf4371_clk_set_rate(struct adf4371_dev *dev, uint32_t chan,
			     uint64_t rate)
{
	if (chan >= dev->num_channels)
		return -1;

	return adf4371_set_freq(dev, rate, chan);
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adf4371_init(struct adf4371_dev **device,
		     const struct adf4371_init_param *init_param)
{
	struct adf4371_dev *dev;
	int32_t ret;
	uint32_t i;

	dev = (struct adf4371_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		return ret;

	dev->spi_3wire_en = init_param->spi_3wire_enable;

	dev->clkin_freq = init_param->clkin_frequency;

	dev->differential_ref_clk = init_param->differential_ref_clock;

	if (init_param->loop_filter_inverting)
		dev->pd_pol = 0;
	else
		dev->pd_pol = 1;

	if (init_param->charge_pump_microamp == 0) {
		/* Default charge pump current is 1.75mA */
		dev->cp_settings.icp = 1750;
		dev->cp_settings.regval = 0x04;
	} else {
		i = no_os_find_closest(init_param->charge_pump_microamp,
				       adf4371_cp_current_microamp,
				       NO_OS_ARRAY_SIZE(adf4371_cp_current_microamp));
		dev->cp_settings.regval = i;
		dev->cp_settings.icp = adf4371_cp_current_microamp[i];
	}

	dev->mute_till_lock_en = init_param->mute_till_lock_enable;

	if (init_param->muxout_select > 10)
		dev->muxout_default_mode = 0; /* Tristate */
	else
		dev->muxout_default_mode = init_param->muxout_select;

	if (dev->muxout_default_mode > 0)
		dev->muxout_en = true;

	dev->muxout_1v8_en = init_param->muxout_level_1v8_enable;

	dev->num_channels = 4;

	for (i = 0; i < init_param->num_channels; i++) {
		dev->channel_cfg[init_param->channels[i].num].enable = true;
		dev->channel_cfg[init_param->channels[i].num].freq =
			init_param->channels[i].power_up_frequency;
	}

	ret = adf4371_setup(dev);
	if (ret < 0)
		goto error;

	*device = dev;

	return 0;

error:
	no_os_spi_remove(dev->spi_desc);
	no_os_free(dev);

	return ret;
}

/**
 * Remove the device - release resources.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adf4371_remove(struct adf4371_dev *device)
{
	int32_t ret = 0;

	if (device->spi_desc)
		ret = no_os_spi_remove(device->spi_desc);

	no_os_free(device);

	return ret;
}
