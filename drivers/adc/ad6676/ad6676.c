/***************************************************************************//**
* @file ad6676.c
* @brief Implementation of AD6676 Driver.
* @authors Dragos Bogdan (dragos.bogdan@analog.com)
* @authors Andrei Grozav (andrei.grozav@analog.com)
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
#include <stdlib.h>
#include <stdio.h>
#include "ad6676.h"
#include "no_os_alloc.h"

/***************************************************************************//**
 * @brief SPI read from device.
 *
 * @param dev      - The device structure.
 * @param reg_addr - Adress of register to be read.
 * @param reg_data - Register data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_spi_read(struct ad6676_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
 * @brief SPI write to device.
 *
 * @param dev      - The device structure.
 * @param reg_addr - Adress of register to be written.
 * @param reg_data - Register data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_spi_write(struct ad6676_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

	return ret;
}

/***************************************************************************//**
 * @brief SPI write a 16 bit register as two consecutive registers, LSB first.
 *
 * @param dev - The device structure.
 * @param reg - Adress of register to be written.
 * @param val - Register data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_set_splitreg(struct ad6676_dev *dev,
				   uint32_t reg,
				   uint32_t val)
{
	int32_t ret;

	ret = ad6676_spi_write(dev, reg, val & 0xFF);
	ret |= ad6676_spi_write(dev, reg + 1, val >> 8);

	return ret;
}

/***************************************************************************//**
 * @brief SPI read from a 16 bit register as two consecutive registers,
 *        LSB first.
 *
 * @param dev - The device structure.
 * @param reg - Adress of register to be written.
 * @param val - Register data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static inline int32_t ad6676_get_splitreg(struct ad6676_dev *dev,
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
 * @brief Set ADC clock frequency.
 *
 * @param dev - The device structure.
 * @param val - New frequency.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_set_fadc(struct ad6676_dev *dev,
			       uint32_t val)
{
	return ad6676_set_splitreg(dev, AD6676_FADC_0,
				   no_os_clamp_t(uint32_t, val, MIN_FADC, MAX_FADC) / MHz);
}

/***************************************************************************//**
 * @brief Get the ADC clock frequency.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static inline uint32_t ad6676_get_fadc(struct ad6676_dev *dev)
{
	uint32_t val;
	int32_t ret = ad6676_get_splitreg(dev, AD6676_FADC_0, &val);
	if (ret < 0)
		return 0;

	return val * MHz;
}

/***************************************************************************//**
 * @brief Set the target IF frequency.
 *
 * @param dev        - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_set_fif(struct ad6676_dev *dev,
		       struct ad6676_init_param *init_param)
{
	return ad6676_set_splitreg(dev, AD6676_FIF_0,
				   no_os_clamp_t(uint32_t, init_param->f_if_hz, MIN_FIF, MAX_FIF) / MHz);
}

/***************************************************************************//**
 * @brief Get the target IF frequency.
 *
 * @param dev        - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
uint64_t ad6676_get_fif(struct ad6676_dev *dev,
			struct ad6676_init_param *init_param)
{
	uint64_t mix1 = 0, mix2 = 0;

	ad6676_spi_read(dev, AD6676_MIX1_TUNING, (uint8_t *)&mix1);
	ad6676_spi_read(dev, AD6676_MIX2_TUNING, (uint8_t *)&mix2);

	mix1 = mix1 * init_param->f_adc_hz;
	mix2 = mix2 * init_param->f_adc_hz;

	no_os_do_div(&mix1, 64);
	no_os_do_div(&mix2, init_param->m);

	return mix1 + mix2;
}

/***************************************************************************//**
 * @brief Set the target BW frequency.
 *
 * @param dev - The device structure.
 * @param val - New bandwidth frequency.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_set_bw(struct ad6676_dev *dev,
			     uint32_t val)
{
	return ad6676_set_splitreg(dev, AD6676_BW_0,
				   no_os_clamp_t(uint32_t, val, MIN_BW, MAX_BW) / MHz);
}

/***************************************************************************//**
 * @brief Get the target BW frequency.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static inline uint32_t ad6676_get_bw(struct ad6676_dev *dev)
{
	uint32_t val;
	int32_t ret = ad6676_get_splitreg(dev, AD6676_BW_0, &val);
	if (ret < 0)
		return 0;

	return val * MHz;
}

/***************************************************************************//**
 * @brief Set decimation factor in the decimation mode register.
 *
 * @param dev        - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_set_decimation(struct ad6676_dev *dev,
				     struct ad6676_init_param *init_param)
{
	switch (init_param->decimation) {
	case 32:
		init_param->decimation = DEC_32;
		init_param->m = 4096;
		break;
	case 24:
		init_param->decimation = DEC_24;
		init_param->m = 3072;
		break;
	case 16:
		init_param->decimation = DEC_16;
		init_param->m = 4096;
		break;
	case 12:
		init_param->decimation = DEC_12;
		init_param->m = 3072;
		break;
	default:
		return -1;
	}

	return ad6676_spi_write(dev, AD6676_DEC_MODE, init_param->decimation);
}

/***************************************************************************//**
 * @brief Set the clock synthesizer to generate a specific frequency using a
 *        given refrence clock and do VCO and CP calibration.
 *
 * @param dev      - The device structure.
 * @param refin_Hz - Reference clock frequency.
 * @param freq     - Synthesizer output frequency.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_set_clk_synth(struct ad6676_dev *dev,
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

	freq = no_os_clamp_t(uint32_t, freq, MIN_FADC_INT_SYNTH, MAX_FADC);

	reg_val = freq / (f_pfd / 2);
	/* 2A0 */
	ret = ad6676_set_splitreg(dev, AD6676_CLKSYN_INT_N_LSB, reg_val);
	if (ret < 0)
		return ret;

	ret = ad6676_spi_write(dev, AD6676_CLKSYN_LOGEN, RESET_CAL);
	if (ret < 0)
		return ret;

	/* Compute I_CP val */
	if(freq == MAX_FADC) {
		reg_val = 0x12;
	} else {
		reg_val = (f_pfd / MHz) * (freq / MHz) * (freq / MHz);
		val64 = 13300000000ULL + reg_val / 2;
		no_os_do_div(&val64, reg_val);
		reg_val = no_os_min_t(uint64_t, 64U, val64 - 1);
	}
	/* I_CP 2AC */
	ret = ad6676_spi_write(dev, AD6676_CLKSYN_I_CP, reg_val);
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

	if(freq == MAX_FADC) {
		ret = ad6676_spi_write(dev, AD6676_CLKSYN_KVCO_VCO, 0x2A); /* 2A9 */
		if (ret < 0) {
			return ret;
		}
	}

	/* Reference Div 2BB */
	ret = ad6676_spi_write(dev, AD6676_CLKSYN_R_DIV,
			       div_val | CLKSYN_R_DIV_RESERVED | CLKSYN_R_DIV_SYSREF_CTRL |
			       CLKSYN_R_DIV_CLKIN_IMPED);
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
		no_os_mdelay(1);
		ad6676_spi_read(dev, AD6676_CLKSYN_STATUS, (uint8_t *)&reg_val);
	} while (--tout && (reg_val & SYN_STAT_VCO_CAL_BUSY));

	if (!tout)
		printf("VCO CAL failed (0x%X)\n", reg_val);


	/* Start CP calibration */
	ret = ad6676_spi_write(dev, AD6676_CLKSYN_CP_CAL, CP_CAL_EN); /* 2AD */
	if (ret < 0)
		return ret;

	tout = 4;
	do {
		no_os_mdelay(1);
		ad6676_spi_read(dev, AD6676_CLKSYN_STATUS, (uint8_t *)&reg_val);
		reg_val &= SYN_STAT_PLL_LCK | SYN_STAT_VCO_CAL_BUSY |
			   SYN_STAT_CP_CAL_DONE;
	} while (--tout && (reg_val != (SYN_STAT_PLL_LCK |
					SYN_STAT_CP_CAL_DONE)));

	if (!tout) {
		printf("AD6676 Synthesizer PLL unlocked (0x%X)\n", reg_val);
		return -1;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Enable external clock for the ADC.
 *
 * @param dev  - The device structure.
 * @param freq - External clock frequency.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_set_extclk_cntl(struct ad6676_dev *dev,
				      uint32_t freq)
{
	int ret;

	printf("%s: frequency %u\n", __func__, freq);

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
 * @brief Setup JESD204 link parameters.
 *
 * @param dev        - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_jesd_setup(struct ad6676_dev *dev,
				 struct ad6676_init_param *init_param)
{
	int32_t ret;

	// sysref power down
	ret = ad6676_spi_write(dev, AD6676_SYNCB_CTRL,
			       (init_param->sysref_pd ? PD_SYSREF_RX : 0) |
			       (init_param->lvds_syncb ? LVDS_SYNCB : 0)); // lvds sync_n
	ret |= ad6676_spi_write(dev, AD6676_DID, 0x01); // device id
	ret |= ad6676_spi_write(dev, AD6676_BID, 0x05); // bank id
	ret |= ad6676_spi_write(dev, AD6676_L, (init_param->n_lanes - 1) |
				(init_param->scrambling_en ? SCR : 0)); // scrambling, 2 lanes
	if (init_param->n_lanes == 2)
		ret |= ad6676_spi_write(dev, AD6676_F, 0x01); // 2 bytes/frame
	else
		ret |= ad6676_spi_write(dev, AD6676_F, 0x03); // 4 bytes/frame
	ret |= ad6676_spi_write(dev, AD6676_K, init_param->
				frames_per_multiframe - 1);
	ret |= ad6676_spi_write(dev, AD6676_M, 0x01); // 2 converters
	ret |= ad6676_spi_write(dev, AD6676_S, 0x00); // 1 samples per frame
	ret |= ad6676_spi_write(dev, AD6676_SER2, 0xBD);

	return ret;

}

/***************************************************************************//**
 * @brief Setup shuffling rate and threshold for the adaptive shuffler.
 *
 * @param dev        - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_shuffle_setup(struct ad6676_dev *dev,
			     struct ad6676_init_param *init_param)
{
	uint32_t reg_val = 0;
	uint32_t val, thresh;
	int32_t i;

	thresh = no_os_clamp_t(uint8_t, init_param->shuffle_thresh, 0, 8U);

	for (i = 0; i < 4; i++) {
		if ((i + 1) == init_param->shuffle_ctrl)
			val = thresh;
		else
			val = 0xF;

		reg_val |= (val << (i * 4));
	}

	return ad6676_set_splitreg(dev, AD6676_SHUFFLE_THREG0, reg_val);
}

/***************************************************************************//**
 * @brief Do internal calibration of JESD, ADC or flash.
 *
 * @param dev - The device structure.
 * @param cal - Calibration options.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_calibrate(struct ad6676_dev *dev,
				uint32_t cal)
{
	int32_t tout_i, tout_o = 2;
	uint32_t done;

	do {
		ad6676_spi_write(dev, AD6676_CAL_CMD, cal);
		tout_i = 2;

		do {
			no_os_mdelay(250);
			ad6676_spi_read(dev, AD6676_CAL_DONE, (uint8_t *)&done);
			done &= CAL_DONE;
		} while (tout_i-- && !done);

		if (!done) {
			printf("AD6676 CAL timeout (0x%X)\n", cal);
			ad6676_spi_write(dev,
					 AD6676_FORCE_END_CAL,
					 FORCE_END_CAL);
			ad6676_spi_write(dev, AD6676_FORCE_END_CAL, 0);
		} else {
			return 0;
		}

	} while (tout_o--);

	printf("AD6676 CAL failed (0x%X)\n", cal);

	return -1;
}

/***************************************************************************//**
 * @brief Software reset all SPI registers to default value.
 *
 * @param dev      - The device structure.
 * @param spi3wire - True for 3 wire SPI, false for 4 wire SPI.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_reset(struct ad6676_dev *dev,
			    uint8_t spi3wire)
{
	int32_t ret;

	ret = ad6676_spi_write(dev, AD6676_SPI_CONFIG,
			       SPI_CONF_SW_RESET |
			       (spi3wire ? 0 : SPI_CONF_SDIO_DIR));
	no_os_mdelay(2);

	return ret;
}

/***************************************************************************//**
 * @brief Set output mode as twos complement or straight binary.
 *
 * @param dev  - The device structure.
 * @param mode - 0 for twos complement; 1 for straight binary.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad6676_outputmode_set(struct ad6676_dev *dev,
				     uint32_t mode)
{
	int32_t ret;

	ret = ad6676_spi_write(dev, AD6676_DP_CTRL, mode);
	if (ret < 0)
		return ret;

	return ad6676_spi_write(dev, AD6676_TEST_GEN, TESTGENMODE_OFF);
}

/***************************************************************************//**
 * @brief Set attenuation in decibels or disable attenuator.
 *
 * @param dev        - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_set_attenuation(struct ad6676_dev *dev,
			       struct ad6676_init_param *init_param)
{
	init_param->attenuation = no_os_clamp(init_param->attenuation, 0, 27);
	ad6676_spi_write(dev, AD6676_ATTEN_VALUE_PIN0,
			 init_param->attenuation);
	ad6676_spi_write(dev, AD6676_ATTEN_VALUE_PIN1,
			 init_param->attenuation);

	return 0;
}

/***************************************************************************//**
 * @brief Initialize the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_setup(struct ad6676_dev **device,
		     struct ad6676_init_param init_param)
{
	int32_t ret;
	uint32_t reg_val;
	uint8_t reg_id;
	uint8_t status;
	uint8_t scale;
	struct ad6676_dev *dev;

	dev = (struct ad6676_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret < 0)
		return ret;

	ad6676_spi_read(dev, AD6676_CHIP_ID0, &reg_id);
	if (reg_id != CHIP_ID0_AD6676) {
		printf("Unrecognized CHIP_ID 0x%X\n", reg_id);
		return -1;
	}

	ret = ad6676_reset(dev, init_param.spi3wire);
	if (ret < 0)
		return ret;

	if (!init_param.use_extclk)
		ad6676_set_clk_synth(dev,
				     init_param.ref_clk,
				     init_param.f_adc_hz);
	else
		ad6676_set_extclk_cntl(dev, init_param.f_adc_hz);

	scale = no_os_clamp(init_param.scale, 0, 64);

	ret |= ad6676_jesd_setup(dev, &init_param);

	ret |= ad6676_set_fadc(dev, init_param.f_adc_hz);
	ret |= ad6676_set_fif(dev, &init_param);
	ret |= ad6676_set_bw(dev, init_param.bw_hz);

	ret |= ad6676_spi_write(dev,
				AD6676_LEXT,
				init_param.ext_l);
	ret |= ad6676_spi_write(dev,
				AD6676_MRGN_L,
				init_param.bw_margin_low_mhz);
	ret |= ad6676_spi_write(dev,
				AD6676_MRGN_U,
				init_param.bw_margin_high_mhz);
	ret |= ad6676_spi_write(dev,
				AD6676_MRGN_IF,
				init_param.bw_margin_if_mhz);
	ret |= ad6676_spi_write(dev,
				AD6676_XSCALE_1,
				scale);

	ret |= ad6676_calibrate(dev, RESON1_CAL | INIT_ADC);

	ret |= ad6676_set_decimation(dev, &init_param);

	ret |= ad6676_calibrate(dev, XCMD0 | XCMD1 | INIT_ADC | TUNE_ADC |
				FLASH_CAL);

	ret |= ad6676_spi_read(dev, AD6676_JESDSYN_STATUS, (uint8_t *)&reg_val);
	reg_val &= SYN_STAT_PLL_LCK;

	if (reg_val != SYN_STAT_PLL_LCK) {
		printf("AD6676 JESD PLL unlocked (0x%X)\n", reg_val);
		return -1;
	}

	ret |= ad6676_outputmode_set(dev, DP_CTRL_TWOS_COMPLEMENT);

	ad6676_spi_read(dev, AD6676_CLKSYN_STATUS, &status);
	if ((status & 0xb) != (SYN_STAT_PLL_LCK | SYN_STAT_CP_CAL_DONE)) {
		printf("AD6676 PLL not locked!!\n\r");
	}

	ad6676_spi_read(dev, AD6676_JESDSYN_STATUS, &status);
	if ((status & 0xb) != (SYN_STAT_PLL_LCK | SYN_STAT_CP_CAL_DONE)) {
		printf("AD6676 JESD PLL not locked!!\n\r");
	}

	*device = dev;

	return ret;
}

/***************************************************************************//**
 * @brief Reconfigure device for other target frequency and bandwidth and
 *        recalibrate.
 *
 * @param dev        - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_update(struct ad6676_dev *dev,
		      struct ad6676_init_param *init_param)
{
	uint8_t scale;
	int32_t ret = 0;

	scale = no_os_clamp(init_param->scale, 0, 64);

	init_param->bw_hz = no_os_clamp_t(uint32_t, init_param->bw_hz,
					  init_param->f_adc_hz / 200,
					  init_param->f_adc_hz / 20);

	init_param->f_if_hz = no_os_clamp_t(uint32_t,
					    init_param->f_if_hz,
					    MIN_FIF,
					    MAX_FIF);

	ret = ad6676_set_fif(dev, init_param);
	ret |= ad6676_set_bw(dev, init_param->bw_hz);

	ret |= ad6676_spi_write(dev,
				AD6676_MRGN_L,
				init_param->bw_margin_low_mhz);
	ret |= ad6676_spi_write(dev,
				AD6676_MRGN_U,
				init_param->bw_margin_high_mhz);
	ret |= ad6676_spi_write(dev,
				AD6676_MRGN_IF,
				init_param->bw_margin_if_mhz);
	ret |= ad6676_spi_write(dev,
				AD6676_XSCALE_1,
				scale);

	ret = ad6676_calibrate(dev, RESON1_CAL | INIT_ADC);
	ret = ad6676_calibrate(dev, XCMD0 | XCMD1 | INIT_ADC | TUNE_ADC |
			       FLASH_CAL);

	return ret;
}

/***************************************************************************//**
 * @brief Perform an interface test.
 *
 * @param dev       - The device structure.
 * @param test_mode - Test mode to perform.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad6676_test(struct ad6676_dev *dev,
		    uint32_t test_mode)
{
	ad6676_spi_write(dev, AD6676_TEST_GEN, test_mode);

	return 0;
}
