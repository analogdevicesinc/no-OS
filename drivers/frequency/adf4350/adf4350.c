/***************************************************************************//**
 *   @file   adf4350.c
 *   @brief  Implementation of ADF4350 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012-2015(c) Analog Devices, Inc.
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
 *
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <malloc.h>
#include <stdio.h>
#include "adf4350.h"
#include "no_os_alloc.h"

/***************************************************************************//**
 * @brief Writes 4 bytes of data to ADF4350.
 *
 * @param dev - The device structure.
 * @param data - Data value to write.
 *
 * @return Returns 0 in case of success or negative error code..
*******************************************************************************/
int32_t adf4350_write(adf4350_dev *dev,
		      uint32_t data)
{
	uint8_t txData[4];

	txData[0] = (data & 0xFF000000) >> 24;
	txData[1] = (data & 0x00FF0000) >> 16;
	txData[2] = (data & 0x0000FF00) >> 8;
	txData[3] = (data & 0x000000FF) >> 0;

	return no_os_spi_write_and_read(dev->spi_desc, txData, 4);
}

/***************************************************************************//**
 * @brief Updates the registers values.
 *
 * @param dev - The device structure.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4350_sync_config(adf4350_dev *dev)
{
	int32_t ret, i, doublebuf = 0;

	for (i = ADF4350_REG5; i >= ADF4350_REG0; i--) {
		if ((dev->regs_hw[i] != dev->regs[i]) ||
		    ((i == ADF4350_REG0) && doublebuf)) {
			switch (i) {
			case ADF4350_REG1:
			case ADF4350_REG4:
				doublebuf = 1;
				break;
			}

			dev->val = (dev->regs[i] | i);
			ret = adf4350_write(dev, dev->val);
			if (ret < 0)
				return ret;
			dev->regs_hw[i] = dev->regs[i];
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4350_MAX_FREQ_PFD is
 *        greater than PFD frequency.
 *
 * @param dev - The device structure.
 * @param r_cnt - Initial r_cnt value.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4350_tune_r_cnt(adf4350_dev *dev,
			   uint16_t r_cnt)
{
	do {
		r_cnt++;
		dev->fpfd = (dev->clkin * (dev->pdata->ref_doubler_en ? 2 : 1)) /
			    (r_cnt * (dev->pdata->ref_div2_en ? 2 : 1));
	} while (dev->fpfd > ADF4350_MAX_FREQ_PFD);

	return r_cnt;
}

/***************************************************************************//**
 * @brief Computes the greatest common divider of two numbers
 *
 * @return Returns the gcd.
*******************************************************************************/
uint32_t gcd(uint32_t x, uint32_t y)
{
	uint32_t tmp;

	while (y != 0) {
		tmp = x % y;
		x = y;
		y = tmp;
	}

	return x;
}

/***************************************************************************//**
 * @brief Sets the ADF4350 frequency.
 *
 * @param dev - The device structure.
 * @param freq - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
int64_t adf4350_set_freq(adf4350_dev *dev,
			 uint64_t freq)
{
	uint64_t tmp;
	uint32_t div_gcd, prescaler, chspc;
	uint16_t mdiv, r_cnt = 0;
	uint8_t band_sel_div;
	int32_t ret;

	if ((freq > ADF4350_MAX_OUT_FREQ) || (freq < ADF4350_MIN_OUT_FREQ))
		return -1;

	if (freq > ADF4350_MAX_FREQ_45_PRESC) {
		prescaler = ADF4350_REG1_PRESCALER;
		mdiv = 75;
	} else {
		prescaler = 0;
		mdiv = 23;
	}

	dev->r4_rf_div_sel = 0;

	while (freq < ADF4350_MIN_VCO_FREQ) {
		freq <<= 1;
		dev->r4_rf_div_sel++;
	}

	/*
	 * Allow a predefined reference division factor
	 * if not set, compute our own
	 */
	if (dev->pdata->ref_div_factor)
		r_cnt = dev->pdata->ref_div_factor - 1;

	chspc = dev->chspc;

	do {
		do {
			do {
				r_cnt = adf4350_tune_r_cnt(dev, r_cnt);
				dev->r1_mod = dev->fpfd / chspc;
				if (r_cnt > ADF4350_MAX_R_CNT) {
					/* try higher spacing values */
					chspc++;
					r_cnt = 0;
				}
			} while ((dev->r1_mod > ADF4350_MAX_MODULUS) && r_cnt);
		} while (r_cnt == 0);


		tmp = freq * (uint64_t)dev->r1_mod + (dev->fpfd > 1);

		tmp = (tmp / dev->fpfd);	/* Div round closest (n + d/2)/d */

		dev->r0_fract = tmp % dev->r1_mod;
		tmp = tmp / dev->r1_mod;

		dev->r0_int = tmp;
	} while (mdiv > dev->r0_int);

	band_sel_div = dev->fpfd % ADF4350_MAX_BANDSEL_CLK >
		       ADF4350_MAX_BANDSEL_CLK / 2 ?
		       dev->fpfd / ADF4350_MAX_BANDSEL_CLK + 1 :
		       dev->fpfd / ADF4350_MAX_BANDSEL_CLK;

	if (dev->r0_fract && dev->r1_mod) {
		div_gcd = gcd(dev->r1_mod, dev->r0_fract);
		dev->r1_mod /= div_gcd;
		dev->r0_fract /= div_gcd;
	} else {
		dev->r0_fract = 0;
		dev->r1_mod = 1;
	}

	dev->regs[ADF4350_REG0] = ADF4350_REG0_INT(dev->r0_int) |
				  ADF4350_REG0_FRACT(dev->r0_fract);

	dev->regs[ADF4350_REG1] = ADF4350_REG1_PHASE(1) |
				  ADF4350_REG1_MOD(dev->r1_mod) |
				  prescaler;

	dev->regs[ADF4350_REG2] =
		ADF4350_REG2_10BIT_R_CNT(r_cnt) |
		ADF4350_REG2_DOUBLE_BUFF_EN |
		(dev->pdata->ref_doubler_en ? ADF4350_REG2_RMULT2_EN : 0) |
		(dev->pdata->ref_div2_en ? ADF4350_REG2_RDIV2_EN : 0) |
		(dev->pdata->r2_user_settings & (ADF4350_REG2_PD_POLARITY_POS |
				ADF4350_REG2_LDP_6ns | ADF4350_REG2_LDF_INT_N |
				ADF4350_REG2_CHARGE_PUMP_CURR_uA(5000) |
				ADF4350_REG2_MUXOUT(0x7) | ADF4350_REG2_NOISE_MODE(0x3)));

	dev->regs[ADF4350_REG3] = dev->pdata->r3_user_settings &
				  (ADF4350_REG3_12BIT_CLKDIV(0xFFF) |
				   ADF4350_REG3_12BIT_CLKDIV_MODE(0x3) |
				   ADF4350_REG3_12BIT_CSR_EN);

	dev->regs[ADF4350_REG4] =
		ADF4350_REG4_FEEDBACK_FUND |
		ADF4350_REG4_RF_DIV_SEL(dev->r4_rf_div_sel) |
		ADF4350_REG4_8BIT_BAND_SEL_CLKDIV(band_sel_div) |
		ADF4350_REG4_RF_OUT_EN |
		(dev->pdata->r4_user_settings &
		 (ADF4350_REG4_OUTPUT_PWR(0x3) |
		  ADF4350_REG4_AUX_OUTPUT_PWR(0x3) |
		  ADF4350_REG4_AUX_OUTPUT_EN |
		  ADF4350_REG4_AUX_OUTPUT_FUND |
		  ADF4350_REG4_MUTE_TILL_LOCK_EN));

	dev->regs[ADF4350_REG5] = ADF4350_REG5_LD_PIN_MODE_DIGITAL | 0x00180000;

	ret = adf4350_sync_config(dev);
	if(ret < 0) {
		return ret;
	}

	tmp = (uint64_t)((dev->r0_int * dev->r1_mod) +
			 dev->r0_fract) * (uint64_t)dev->fpfd;
	tmp = tmp / ((uint64_t)dev->r1_mod * ((uint64_t)1 << dev->r4_rf_div_sel));

	return tmp;
}

/***************************************************************************//**
 * @brief Initializes the ADF4350.
 *
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4350_setup(adf4350_dev **device,
		      adf4350_init_param init_param)
{
	adf4350_dev *dev;
	int32_t ret;

	dev = (adf4350_dev *)no_os_malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	dev->pdata = (struct adf4350_platform_data *)no_os_malloc(sizeof(*dev->pdata));
	if (!dev->pdata)
		return -1;

	dev->pdata->clkin = init_param.clkin;

	dev->pdata->channel_spacing = init_param.channel_spacing;

	dev->pdata->power_up_frequency = init_param.power_up_frequency;
	dev->pdata->ref_div_factor = init_param.reference_div_factor;
	dev->pdata->ref_doubler_en = init_param.reference_doubler_enable;
	dev->pdata->ref_div2_en = init_param.reference_div2_enable;

	/* r2_user_settings */

	dev->pdata->r2_user_settings =
		init_param.phase_detector_polarity_positive_enable ?
		ADF4350_REG2_PD_POLARITY_POS : 0;
	dev->pdata->r2_user_settings |=
		init_param.lock_detect_precision_6ns_enable ?
		ADF4350_REG2_LDP_6ns : 0;
	dev->pdata->r2_user_settings |=
		init_param.lock_detect_function_integer_n_enable ?
		ADF4350_REG2_LDF_INT_N : 0;
	dev->pdata->r2_user_settings |=
		ADF4350_REG2_CHARGE_PUMP_CURR_uA(init_param.charge_pump_current);
	dev->pdata->r2_user_settings |=
		ADF4350_REG2_MUXOUT(init_param.muxout_select);
	dev->pdata->r2_user_settings |=
		init_param.low_spur_mode_enable ? ADF4350_REG2_NOISE_MODE(0x3) : 0;

	/* r3_user_settings */

	dev->pdata->r3_user_settings = init_param.cycle_slip_reduction_enable ?
				       ADF4350_REG3_12BIT_CSR_EN : 0;
	dev->pdata->r3_user_settings |= init_param.charge_cancellation_enable ?
					ADF4351_REG3_CHARGE_CANCELLATION_EN : 0;
	dev->pdata->r3_user_settings |= init_param.anti_backlash_3ns_enable ?
					ADF4351_REG3_ANTI_BACKLASH_3ns_EN : 0;
	dev->pdata->r3_user_settings |=
		init_param.band_select_clock_mode_high_enable ?
		ADF4351_REG3_BAND_SEL_CLOCK_MODE_HIGH : 0;
	dev->pdata->r3_user_settings |=
		ADF4350_REG3_12BIT_CLKDIV(init_param.clk_divider_12bit);
	dev->pdata->r3_user_settings |=
		ADF4350_REG3_12BIT_CLKDIV_MODE(init_param.clk_divider_mode);

	/* r4_user_settings */

	dev->pdata->r4_user_settings = init_param.aux_output_enable ?
				       ADF4350_REG4_AUX_OUTPUT_EN : 0;
	dev->pdata->r4_user_settings |= init_param.aux_output_fundamental_enable ?
					ADF4350_REG4_AUX_OUTPUT_FUND : 0;
	dev->pdata->r4_user_settings |= init_param.mute_till_lock_enable ?
					ADF4350_REG4_MUTE_TILL_LOCK_EN : 0;
	dev->pdata->r4_user_settings |=
		ADF4350_REG4_OUTPUT_PWR(init_param.output_power);
	dev->pdata->r4_user_settings |=
		ADF4350_REG4_AUX_OUTPUT_PWR(init_param.aux_output_power);

	adf4350_out_altvoltage0_refin_frequency(dev, dev->pdata->clkin);
	adf4350_out_altvoltage0_frequency_resolution(dev,
			dev->pdata->channel_spacing);
	adf4350_out_altvoltage0_frequency(dev, dev->pdata->power_up_frequency);

	*device = dev;

	printf("ADF4350 successfully initialized.\n");

	return ret;
}

/***************************************************************************//**
 * @brief Stores PLL 0 frequency in Hz.
 *
 * @param dev - The device structure.
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int64_t adf4350_out_altvoltage0_frequency(adf4350_dev *dev,
		int64_t Hz)
{
	return adf4350_set_freq(dev, Hz);
}

/***************************************************************************//**
 * @brief Stores PLL 0 frequency resolution/channel spacing in Hz.
 *
 * @param dev - The device structure.
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int32_t adf4350_out_altvoltage0_frequency_resolution(adf4350_dev *dev,
		int32_t Hz)
{
	if(Hz != INT32_MAX) {
		dev->chspc = Hz;
	}

	return dev->chspc;
}

/***************************************************************************//**
 * @brief Sets PLL 0 REFin frequency in Hz.
 *
 * @param dev - The device structure.
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int64_t adf4350_out_altvoltage0_refin_frequency(adf4350_dev *dev,
		int64_t Hz)
{
	if(Hz != INT32_MAX) {
		dev->clkin = Hz;
	}

	return dev->clkin;
}

/***************************************************************************//**
 * @brief Powers down the PLL.
 *
 * @param dev - The device structure.
 * @param pwd - Power option.
 *				Example: 0 - Power up the PLL.
 *						 1 - Power down the PLL.
 *
 * @return Returns the PLL's power status.
*******************************************************************************/
int32_t adf4350_out_altvoltage0_powerdown(adf4350_dev *dev,
		int32_t pwd)
{
	if(pwd == 1) {
		dev->regs[ADF4350_REG2] |= ADF4350_REG2_POWER_DOWN_EN;
		adf4350_sync_config(dev);
	}
	if(pwd == 0) {
		dev->regs[ADF4350_REG2] &= ~ADF4350_REG2_POWER_DOWN_EN;
		adf4350_sync_config(dev);
	}


	return (dev->regs[ADF4350_REG2] & ADF4350_REG2_POWER_DOWN_EN);
}
