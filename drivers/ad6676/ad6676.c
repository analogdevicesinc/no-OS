/***************************************************************************//**
* @file ad6676.c
* @brief Implementation of AD6676 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
#include <xil_printf.h>
#include "platform_drivers.h"
#include "ad6676.h"

/***************************************************************************//**
* @brief ad6676_spi_read
*******************************************************************************/
int32_t ad6676_spi_read(ad6676_dev *dev,
						uint16_t reg_addr,
						uint8_t *reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = spi_write_and_read(&dev->spi_dev, buf, 3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
* @brief ad6676_spi_write
*******************************************************************************/
int32_t ad6676_spi_write(ad6676_dev *dev,
						 uint16_t reg_addr,
						 uint8_t reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = spi_write_and_read(&dev->spi_dev, buf, 3);

	return ret;
}

/***************************************************************************//**
* @brief ad6676_set_splitreg
*******************************************************************************/
static int32_t ad6676_set_splitreg(ad6676_dev *dev,
								   uint32_t reg,
								   uint32_t val)
{
	int32_t ret;

	ret = ad6676_spi_write(dev, reg, val & 0xFF);
	ret |= ad6676_spi_write(dev, reg + 1, val >> 8);

	return ret;
}

/***************************************************************************//**
* @brief ad6676_get_splitreg
*******************************************************************************/
static inline int32_t ad6676_get_splitreg(ad6676_dev *dev,
										  uint32_t reg,
										  uint32_t *val)
{
	int32_t ret;
	uint8_t reg_data;

	ret = ad6676_spi_read(dev, reg, &reg_data);
	if (ret < 0)
		return ret;

	*val = reg_data;

	ret = ad6676_spi_read(dev, reg + 1, &reg_data);
	if (ret < 0)
		return ret;

	*val |= ((uint32_t)reg_data << 8);

	return ret;
}

/***************************************************************************//**
* @brief ad6676_set_fadc
*******************************************************************************/
static int32_t ad6676_set_fadc(ad6676_dev *dev,
							   uint32_t val)
{
	return ad6676_set_splitreg(dev, AD6676_FADC_0,
			clamp_t(uint32_t, val, MIN_FADC, MAX_FADC) / MHz);
}

/***************************************************************************//**
* @brief ad6676_get_fadc
*******************************************************************************/
static inline uint32_t ad6676_get_fadc(ad6676_dev *dev)
{
	uint32_t val;
	int32_t ret = ad6676_get_splitreg(dev, AD6676_FADC_0, &val);
	if (ret < 0)
		return 0;

	return val * MHz;
}

/***************************************************************************//**
* @brief ad6676_set_fif
*******************************************************************************/
static int32_t ad6676_set_fif(ad6676_dev *dev,
							  uint32_t val)
{
	struct ad6676_platform_data *pdata = dev->phy->pdata;

	return ad6676_set_splitreg(dev, AD6676_FIF_0,
		clamp_t(uint32_t, val, pdata->base.f_if_min_hz,
			pdata->base.f_if_max_hz) / MHz);
}

/***************************************************************************//**
* @brief ad6676_get_fif
*******************************************************************************/
static uint32_t ad6676_get_fif(ad6676_dev *dev)
{
	struct ad6676_platform_data *pdata = dev->phy->pdata;
	uint64_t mix1 = 0, mix2 = 0;

	ad6676_spi_read(dev, AD6676_MIX1_TUNING, (uint8_t *)&mix1);
	ad6676_spi_read(dev, AD6676_MIX2_TUNING, (uint8_t *)&mix2);

	mix1 = mix1 * pdata->base.f_adc_hz;
	mix2 = mix2 * pdata->base.f_adc_hz;

	do_div(&mix1, 64);
	do_div(&mix2, dev->phy->m);

	return mix1 + mix2;
}

/***************************************************************************//**
* @brief ad6676_set_bw
*******************************************************************************/
static int32_t ad6676_set_bw(ad6676_dev *dev,
							 uint32_t val)
{
	return ad6676_set_splitreg(dev, AD6676_BW_0,
		clamp_t(uint32_t, val, MIN_BW, MAX_BW) / MHz);
}

/***************************************************************************//**
* @brief ad6676_get_bw
*******************************************************************************/
static inline uint32_t ad6676_get_bw(ad6676_dev *dev)
{
	uint32_t val;
	int32_t ret = ad6676_get_splitreg(dev, AD6676_BW_0, &val);
	if (ret < 0)
		return 0;

	return val * MHz;
}

/***************************************************************************//**
* @brief ad6676_set_decimation
*******************************************************************************/
static int32_t ad6676_set_decimation(ad6676_dev *dev,
									 uint32_t val)
{
	switch (val) {
	case 32:
		val = DEC_32;
		dev->phy->m = 4096;
		break;
	case 24:
		val = DEC_24;
		dev->phy->m = 3072;
		break;
	case 16:
		val = DEC_16;
		dev->phy->m = 4096;
		break;
	case 12:
		val = DEC_12;
		dev->phy->m = 3072;
		break;
	default:
		return -1;
	}

	return ad6676_spi_write(dev, AD6676_DEC_MODE, val);
}

/***************************************************************************//**
* @brief ad6676_set_clk_synth
*******************************************************************************/
static int32_t ad6676_set_clk_synth(ad6676_dev *dev,
									uint32_t refin_Hz,
									uint32_t freq)
{
	uint32_t f_pfd, reg_val, tout, div_val;
	uint64_t val64;
	int32_t ret;

	if (refin_Hz < 40000000UL) {
		f_pfd = 2 * refin_Hz;
		div_val = R_DIV(3);
	} else if (refin_Hz < 80000000UL) {
		f_pfd = refin_Hz;
		div_val = R_DIV(0);
	} else if (refin_Hz < 160000000UL) {
		f_pfd = refin_Hz / 2;
		div_val = R_DIV(1);
	} else if (refin_Hz < 320000000UL) {
		f_pfd = refin_Hz / 4;
		div_val = R_DIV(2);
	} else
		return -1;

	/* Compute N val */

	freq = clamp_t(uint32_t, freq, MIN_FADC_INT_SYNTH, MAX_FADC);

	reg_val = freq / (f_pfd / 2);
	ret = ad6676_set_splitreg(dev, AD6676_CLKSYN_INT_N_LSB, reg_val); /* 2A0 */
	if (ret < 0)
		return ret;

	ret = ad6676_spi_write(dev, AD6676_CLKSYN_LOGEN, RESET_CAL);
	if (ret < 0)
		return ret;

	/* Compute I_CP val */

	reg_val = (f_pfd / MHz) * (freq / MHz) * (freq / MHz);
	val64 = 13300000000ULL + reg_val / 2;
	do_div(&val64, reg_val);
	reg_val = min_t(uint64_t, 64U, val64 - 1);

	ret = ad6676_spi_write(dev, AD6676_CLKSYN_I_CP, reg_val); /* I_CP 2AC */
	if (ret < 0)
		return ret;

	/* VCO Configuration Settings */
	if (freq <= 2950000000UL)
		reg_val = 0xF0;
	else if (freq < 3100000000UL)
		reg_val = 0xE0;
	else
		reg_val = 0xD0;

	ret = ad6676_spi_write(dev, AD6676_CLKSYN_VCO_BIAS, 0x37); /* 2AA */
	if (ret < 0)
		return ret;

	ret = ad6676_spi_write(dev, AD6676_CLKSYN_VCO_VAR, reg_val); /* 2B7 */
	if (ret < 0)
		return ret;

	ret = ad6676_spi_write(dev, AD6676_CLKSYN_R_DIV,
			div_val | CLKSYN_R_DIV_RESERVED); /* Reference Div 2BB */
	if (ret < 0)
		return ret;


	/* Enable CLKSYN and ADC clock */
	ret = ad6676_spi_write(dev, AD6676_CLKSYN_ENABLE, /* 2A0 */
		EN_OVER_IDE | EN_VCO | EN_VCO_ALC |
		EN_VCO_PTAT | EN_ADC_CK | EN_SYNTH);
	if (ret < 0)
		return ret;

	/* Start VCO calibration */
	ret = ad6676_spi_write(dev, AD6676_CLKSYN_VCO_CAL, /* 2AB */
		INIT_ALC_VALUE(0xC) | 0x5);
	if (ret < 0)
		return ret;

	tout = 4;
	do {
		mdelay(1);
		ad6676_spi_read(dev, AD6676_CLKSYN_STATUS, (uint8_t *)&reg_val);
	} while (--tout && (reg_val & SYN_STAT_VCO_CAL_BUSY));

	if (!tout)
		xil_printf("VCO CAL failed (0x%X)\n", reg_val);


	/* Start CP calibration */
	ret = ad6676_spi_write(dev, AD6676_CLKSYN_CP_CAL, CP_CAL_EN); /* 2AD */
	if (ret < 0)
		return ret;

	tout = 4;
	do {
		mdelay(1);
		ad6676_spi_read(dev, AD6676_CLKSYN_STATUS, (uint8_t *)&reg_val);
		reg_val &= SYN_STAT_PLL_LCK | SYN_STAT_VCO_CAL_BUSY | SYN_STAT_CP_CAL_DONE;
	} while (--tout && (reg_val != (SYN_STAT_PLL_LCK | SYN_STAT_CP_CAL_DONE)));

	if (!tout) {
		xil_printf("AD6676 Synthesizer PLL unlocked (0x%X)\n", reg_val);
		return -1;
	}

	return 0;
}

/***************************************************************************//**
* @brief ad6676_jesd_setup
*******************************************************************************/
static int32_t ad6676_set_extclk_cntl(ad6676_dev *dev,
									  uint32_t freq)
{
	int ret;

	xil_printf("%s: frequency %u\n", __func__, freq);

	ret = ad6676_spi_write(dev, AD6676_CLKSYN_LOGEN, 0x5);
	if (ret < 0)
		return ret;

	/* Enable EXT CLK and ADC clock */
	ret = ad6676_spi_write(dev, AD6676_CLKSYN_ENABLE, /* 2A0 */
		EN_EXT_CK | EN_ADC_CK);
	if (ret < 0)
		return ret;

	return 0;
}

/***************************************************************************//**
* @brief ad6676_jesd_setup
*******************************************************************************/
static int32_t ad6676_jesd_setup(ad6676_dev *dev,
								 struct ad6676_jesd_conf *conf)
{
	int32_t ret;

	ret = ad6676_spi_write(dev, AD6676_SYNCB_CTRL,
		(conf->sysref_pd ? PD_SYSREF_RX : 0) |
		(conf->lvds_syncb ? LVDS_SYNCB : 0)); // lvds sync_n
	ret |= ad6676_spi_write(dev, AD6676_DID, 0x01); // device id
	ret |= ad6676_spi_write(dev, AD6676_BID, 0x05); // bank id
	ret |= ad6676_spi_write(dev, AD6676_L, (conf->l - 1) |
		(conf->scrambling_en ? SCR : 0)); // scrambling, 2 lanes
	ret |= ad6676_spi_write(dev, AD6676_F, 0x01); // 2 bytes/frame
	ret |= ad6676_spi_write(dev, AD6676_K, conf->f - 1); // 16 frames/multi-frame
	ret |= ad6676_spi_write(dev, AD6676_M, 0x01); // 2 converters
	ret |= ad6676_spi_write(dev, AD6676_S, 0x00); // 1 samples per frame
	ret |= ad6676_spi_write(dev, AD6676_SER2, 0xBD);

	return ret;

}

/***************************************************************************//**
* @brief ad6676_shuffle_setup
*******************************************************************************/
int32_t ad6676_shuffle_setup(ad6676_dev *dev,
							 struct ad6676_shuffler_conf *conf)
{
	uint32_t reg_val, val, thresh;
	int32_t i;

	thresh = clamp_t(uint8_t, conf->shuffle_thresh, 0, 8U);

	for (i = 0; i < 4; i++) {
		if ((i + 1) == conf->shuffle_ctrl)
			val = thresh;
		else
			val = 0xF;

		reg_val |= (val << (i * 4));
	}

	return ad6676_set_splitreg(dev, AD6676_SHUFFLE_THREG0, reg_val);
}

/***************************************************************************//**
* @brief ad6676_calibrate
*******************************************************************************/
static int32_t ad6676_calibrate(ad6676_dev *dev,
								uint32_t cal)
{
	int32_t tout_i, tout_o = 2;
	uint32_t done;

	do {
		ad6676_spi_write(dev, AD6676_CAL_CMD, cal);
		tout_i = 2;

		do {
			mdelay(250);
			ad6676_spi_read(dev, AD6676_CAL_DONE, (uint8_t *)&done);
			done &= CAL_DONE;
		} while (tout_i-- && !done);

		if (!done) {
			xil_printf("AD6676 CAL timeout (0x%X)\n", cal);
			ad6676_spi_write(dev, AD6676_FORCE_END_CAL, FORCE_END_CAL);
			ad6676_spi_write(dev, AD6676_FORCE_END_CAL, 0);
		} else {
			return 0;
		}

	} while (tout_o--);

	xil_printf("AD6676 CAL failed (0x%X)\n", cal);

	return -1;
}

/***************************************************************************//**
* @brief ad6676_reset
*******************************************************************************/
static int32_t ad6676_reset(ad6676_dev *dev,
							uint8_t spi3wire)
{
	int32_t ret;

	ret = ad6676_spi_write(dev, AD6676_SPI_CONFIG,
				   SPI_CONF_SW_RESET |
				   (spi3wire ? 0 : SPI_CONF_SDIO_DIR));
	mdelay(2);

	return ret;
}

/***************************************************************************//**
* @brief ad6676_init
*******************************************************************************/
static int32_t ad6676_init(ad6676_dev *dev)
{
	int32_t ret;
	uint32_t reg_val;

	ret = ad6676_reset(dev, dev->phy->pdata->spi3wire);
	if (ret < 0)
		return ret;

	if (!dev->phy->pdata->base.use_extclk)
		ad6676_set_clk_synth(dev, dev->phy->ref_clk, dev->phy->pdata->base.f_adc_hz);
	else
		ad6676_set_extclk_cntl(dev, dev->phy->pdata->base.f_adc_hz);

	ad6676_jesd_setup(dev, &dev->phy->pdata->jesd);

	ad6676_set_fadc(dev, dev->phy->pdata->base.f_adc_hz);
	ad6676_set_fif(dev, dev->phy->pdata->base.f_if_hz);
	ad6676_set_bw(dev, dev->phy->pdata->base.bw_hz);

	ret |= ad6676_spi_write(dev, AD6676_LEXT, dev->phy->pdata->base.ext_l);
	ret |= ad6676_spi_write(dev, AD6676_MRGN_L, dev->phy->pdata->base.bw_margin_low_mhz);
	ret |= ad6676_spi_write(dev, AD6676_MRGN_U, dev->phy->pdata->base.bw_margin_high_mhz);
	ret |= ad6676_spi_write(dev, AD6676_MRGN_IF, dev->phy->pdata->base.bw_margin_if_mhz);
	ret |= ad6676_spi_write(dev, AD6676_XSCALE_1, dev->phy->pdata->base.scale);

	ad6676_set_decimation(dev, 32);

	ret = ad6676_calibrate(dev, RESON1_CAL | INIT_ADC);

	ad6676_set_decimation(dev, dev->phy->pdata->base.decimation);

	ret = ad6676_calibrate(dev, XCMD0 | XCMD1 | INIT_ADC | TUNE_ADC | FLASH_CAL);

	ad6676_spi_read(dev, AD6676_JESDSYN_STATUS, (uint8_t *)&reg_val);
	reg_val &= SYN_STAT_PLL_LCK;

	if (reg_val != SYN_STAT_PLL_LCK) {
		xil_printf("AD6676 JESD PLL unlocked (0x%X)\n", reg_val);
		return -1;
	}

	return 0;
}

/***************************************************************************//**
* @brief ad6676_update
*******************************************************************************/
int32_t ad6676_update(ad6676_dev *dev,
					  struct ad6676_platform_data *pdata)
{
	int32_t ret;

	pdata->base.bw_hz = clamp_t(uint32_t, pdata->base.bw_hz,
				    pdata->base.f_adc_hz / 200,
			     pdata->base.f_adc_hz / 20);

	pdata->base.f_if_hz = clamp_t(uint32_t, pdata->base.f_if_hz,
				pdata->base.f_if_min_hz,
				pdata->base.f_if_max_hz);

	ret = ad6676_set_fif(dev, pdata->base.f_if_hz);
	ret |= ad6676_set_bw(dev, pdata->base.bw_hz);

	ret |= ad6676_spi_write(dev, AD6676_MRGN_L, pdata->base.bw_margin_low_mhz);
	ret |= ad6676_spi_write(dev, AD6676_MRGN_U, pdata->base.bw_margin_high_mhz);
	ret |= ad6676_spi_write(dev, AD6676_MRGN_IF, pdata->base.bw_margin_if_mhz);
	ret |= ad6676_spi_write(dev, AD6676_XSCALE_1, pdata->base.scale);

	ret = ad6676_calibrate(dev, RESON1_CAL | INIT_ADC);
	ret = ad6676_calibrate(dev, XCMD0 | XCMD1 | INIT_ADC | TUNE_ADC | FLASH_CAL);

	pdata->base.f_if_hz = ad6676_get_fif(dev);

	return ret;
}

/***************************************************************************//**
* @brief ad6676_outputmode_set
*******************************************************************************/
static int32_t ad6676_outputmode_set(ad6676_dev *dev,
									 uint32_t mode)
{
	int32_t ret;

	ret = ad6676_spi_write(dev, AD6676_DP_CTRL, mode);
	if (ret < 0)
		return ret;

	return ad6676_spi_write(dev, AD6676_TEST_GEN, TESTGENMODE_OFF);
}

/***************************************************************************//**
* @brief ad6676_set_if_frequency
*******************************************************************************/
int32_t ad6676_set_if_frequency(ad6676_dev *dev,
								uint32_t if_freq_hz)
{
	dev->phy->pdata->base.f_if_hz = if_freq_hz;
	ad6676_update(dev, dev->phy->pdata);

	return 0;
}

/***************************************************************************//**
* @brief ad6676_get_if_freq
*******************************************************************************/
int32_t ad6676_get_if_frequency(ad6676_dev *dev,
								uint32_t *if_freq_hz)
{
	*if_freq_hz = dev->phy->pdata->base.f_if_hz;

	return 0;
}

/***************************************************************************//**
* @brief ad6676_set_if_bandwidth
*******************************************************************************/
int32_t ad6676_set_if_bandwidth(ad6676_dev *dev,
								uint32_t if_bw_hz)
{
	dev->phy->pdata->base.bw_hz = if_bw_hz;
	ad6676_update(dev, dev->phy->pdata);

	return 0;
}

/***************************************************************************//**
* @brief ad6676_get_if_bandwidth
*******************************************************************************/
int32_t ad6676_get_if_bandwidth(ad6676_dev *dev,
								uint32_t *if_bw_hz)
{
	*if_bw_hz = dev->phy->pdata->base.bw_hz;

	return 0;
}

/***************************************************************************//**
* @brief ad6676_set_scale
*******************************************************************************/
int32_t ad6676_set_scale(ad6676_dev *dev,
						 uint8_t scale)
{
	scale = clamp(scale, 0, 64);
	dev->phy->pdata->base.scale = scale;
	ad6676_update(dev, dev->phy->pdata);

	return 0;
}

/***************************************************************************//**
* @brief ad6676_get_scale
*******************************************************************************/
int32_t ad6676_get_scale(ad6676_dev *dev,
						 uint8_t *scale)
{
	*scale = dev->phy->pdata->base.scale;

	return 0;
}

/***************************************************************************//**
* @brief ad6676_set_attenuation
*******************************************************************************/
int32_t ad6676_set_attenuation(ad6676_dev *dev,
							   uint8_t attenuation)
{
	dev->phy->pdata->base.attenuation = clamp(attenuation, 0, 27);
	ad6676_spi_write(dev, AD6676_ATTEN_VALUE_PIN0,
			dev->phy->pdata->base.attenuation);
	ad6676_spi_write(dev, AD6676_ATTEN_VALUE_PIN1,
			dev->phy->pdata->base.attenuation);

	return 0;
}

/***************************************************************************//**
* @brief ad6676_get_attenuation
*******************************************************************************/
int32_t ad6676_get_attenuation(ad6676_dev *dev,
							   uint8_t *attenuation)
{
	*attenuation = dev->phy->pdata->base.attenuation;

	return 0;
}

/***************************************************************************//**
* @brief ad6676_gpio_config
*******************************************************************************/
int32_t ad6676_gpio_config(ad6676_dev *dev)
{
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_oen, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_sela, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_selb, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s0, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s1, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_resetb, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_agc1, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_agc2, GPIO_OUT);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_agc3, GPIO_IN);
	gpio_set_direction(&dev->gpio_dev, dev->phy->pdata->gpio.adc_agc4, GPIO_IN);

	gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_oen, GPIO_LOW);

	switch (dev->phy->pdata->base.decimation) {
	case 12:
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s0, GPIO_HIGH);
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s1, GPIO_HIGH);
		break;
	case 16:
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s0, GPIO_LOW);
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s1, GPIO_HIGH);
		break;
	case 24:
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s0, GPIO_LOW);
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s1, GPIO_HIGH);
		break;
	case 32:
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s0, GPIO_LOW);
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_s1, GPIO_LOW);
		break;
	default:
		return -1;
	}

	if (dev->phy->pdata->base.use_extclk) {
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_sela, GPIO_HIGH);
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_selb, GPIO_LOW);
	} else {
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_sela, GPIO_LOW);
		gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_selb, GPIO_HIGH);
	}

	gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_resetb, GPIO_HIGH);
	gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_agc1, GPIO_LOW);
	gpio_set_value(&dev->gpio_dev, dev->phy->pdata->gpio.adc_agc2, GPIO_LOW);

	return 0;
}

/***************************************************************************//**
* @brief ad6676_setup
*******************************************************************************/
int32_t ad6676_setup(ad6676_dev **device,
					 ad6676_init_param init_param)
{
	ad6676_dev *dev;
	uint8_t reg_id;
	uint8_t status;
	int32_t ret;

	dev = (ad6676_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	dev->gpio_dev.device_id = init_param.gpio_device_id;
	dev->gpio_dev.type = init_param.gpio_type;
	ret = gpio_init(&dev->gpio_dev);

	dev->spi_dev.chip_select = init_param.spi_chip_select;
	dev->spi_dev.mode = init_param.spi_mode;
	dev->spi_dev.device_id = init_param.spi_device_id;
	dev->spi_dev.type = init_param.spi_type;
	ret |= spi_init(&dev->spi_dev);

	dev->phy = (struct ad6676_phy *)malloc(sizeof(*dev->phy));
	if (!dev->phy) {
		return -1;
	}

	dev->phy->pdata = (struct ad6676_platform_data *)malloc(sizeof(*dev->phy->pdata));
	if (!dev->phy->pdata) {
		return -1;
	}

	dev->phy->ref_clk = init_param.reference_clk_rate;

	dev->phy->pdata->spi3wire = init_param.spi_3wire_enable;

	/* Base Configuration */
	dev->phy->pdata->base.f_adc_hz = init_param.adc_frequency_hz;
	dev->phy->pdata->base.f_if_hz = init_param.intermediate_frequency_hz;
	dev->phy->pdata->base.f_if_min_hz = init_param.intermediate_frequency_min_hz;
	dev->phy->pdata->base.f_if_max_hz = init_param.intermediate_frequency_max_hz;
	dev->phy->pdata->base.bw_hz = init_param.bandwidth_hz;
	dev->phy->pdata->base.bw_margin_low_mhz = init_param.bandwidth_margin_low_mhz;
	dev->phy->pdata->base.bw_margin_high_mhz = init_param.bandwidth_margin_high_mhz;
	dev->phy->pdata->base.bw_margin_if_mhz = init_param.bandwidth_margin_if_mhz;
	dev->phy->pdata->base.decimation = init_param.decimation;
	dev->phy->pdata->base.ext_l = init_param.external_inductance_l_nh;
	dev->phy->pdata->base.scale = init_param.idac1_fullscale_adjust;
	dev->phy->pdata->base.use_extclk = init_param.use_external_clk_enable;
	dev->phy->pdata->base.fadc_fixed = init_param.adc_frequency_fixed_enable;

	/* JESD Configuration */
	dev->phy->pdata->jesd.scrambling_en = init_param.jesd_scrambling_enable;
	dev->phy->pdata->jesd.lvds_syncb = init_param.jesd_use_lvds_syncb_enable;
	dev->phy->pdata->jesd.sysref_pd = init_param.jesd_powerdown_sysref_enable;
	dev->phy->pdata->jesd.l = init_param.jesd_l_lanes;
	dev->phy->pdata->jesd.f = init_param.jesd_f_frames_per_multiframe;

	/* Shuffler Configuration */
	dev->phy->pdata->shuffler.shuffle_ctrl = init_param.shuffler_control;
	dev->phy->pdata->shuffler.shuffle_thresh = init_param.shuffler_thresh;

	/* GPIO */
	dev->phy->pdata->gpio.adc_oen = init_param.gpio_adc_oen;
	dev->phy->pdata->gpio.adc_sela = init_param.gpio_adc_sela;
	dev->phy->pdata->gpio.adc_selb = init_param.gpio_adc_selb;
	dev->phy->pdata->gpio.adc_s0 = init_param.gpio_adc_s0;
	dev->phy->pdata->gpio.adc_s1 = init_param.gpio_adc_s1;
	dev->phy->pdata->gpio.adc_resetb = init_param.gpio_adc_resetb;
	dev->phy->pdata->gpio.adc_agc1 = init_param.gpio_adc_agc1;
	dev->phy->pdata->gpio.adc_agc2 = init_param.gpio_adc_agc2;
	dev->phy->pdata->gpio.adc_agc3 = init_param.gpio_adc_agc3;
	dev->phy->pdata->gpio.adc_agc4 = init_param.gpio_adc_agc4;

	dev->phy->pdata->base.attenuation = 12;

	ad6676_gpio_config(dev);

	ad6676_spi_read(dev, AD6676_CHIP_ID0, &reg_id);
	if (reg_id != CHIPID0_AD6676) {
		xil_printf("Unrecognized CHIP_ID 0x%X\n", reg_id);
		goto error;
	}

	ad6676_init(dev);

	ad6676_outputmode_set(dev, DP_CTRL_TWOS_COMPLEMENT);

	ad6676_spi_read(dev, AD6676_CLKSYN_STATUS, &status);
	if ((status & 0xb) != (SYN_STAT_PLL_LCK | SYN_STAT_CP_CAL_DONE)) {
		xil_printf("AD6676 PLL not locked!!\n\r");
	}

	ad6676_spi_read(dev, AD6676_JESDSYN_STATUS, &status);
	if ((status & 0xb) != (SYN_STAT_PLL_LCK | SYN_STAT_CP_CAL_DONE)) {
		xil_printf("AD6676 JESD PLL not locked!!\n\r");
	}

	*device = dev;

	xil_printf("AD6676 successfully initialized.\n");

	return ret;

error:
	free(dev->phy->pdata);
	free(dev->phy);

	return -1;
}
