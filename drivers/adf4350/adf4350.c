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
#include "platform_drivers.h"
#include "adf4350.h"

/******************************************************************************/
/************************ Local variables and types ***************************/
/******************************************************************************/
static struct adf4350_state
{
	struct adf4350_platform_data	*pdata;
	uint32_t	clkin;
	uint32_t	chspc;	/* Channel Spacing */
	uint32_t	fpfd;	/* Phase Frequency Detector */
	uint32_t	min_out_freq;
	uint32_t	r0_fract;
	uint32_t	r0_int;
	uint32_t	r1_mod;
	uint32_t	r4_rf_div_sel;
	uint32_t	regs[6];
	uint32_t	regs_hw[6];
	uint32_t 	val;
}adf4350_st;

uint8_t adf4350_slave_select;

/***************************************************************************//**
 * @brief Writes 4 bytes of data to ADF4350.
 *
 * @param data - Data value to write.
 *
 * @return Returns 0 in case of success or negative error code..
*******************************************************************************/
int32_t adf4350_write(uint32_t data)
{
	uint8_t txData[4];

	txData[0] = (data & 0xFF000000) >> 24;
	txData[1] = (data & 0x00FF0000) >> 16;
	txData[2] = (data & 0x0000FF00) >> 8;
	txData[3] = (data & 0x000000FF) >> 0;

	return spi_write_and_read(adf4350_slave_select, txData, 4);
}

/***************************************************************************//**
 * @brief Updates the registers values.
 *
 * @param st - The selected structure.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4350_sync_config(struct adf4350_state *st)
{
	int32_t ret, i, doublebuf = 0;

	for (i = ADF4350_REG5; i >= ADF4350_REG0; i--)
	{
		if ((st->regs_hw[i] != st->regs[i]) ||
			((i == ADF4350_REG0) && doublebuf))
		{
			switch (i) 
			{
				case ADF4350_REG1:
				case ADF4350_REG4:
					doublebuf = 1;
					break;
			}

			st->val = (st->regs[i] | i);
			ret = adf4350_write(st->val);
			if (ret < 0)
				return ret;
			st->regs_hw[i] = st->regs[i];
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4350_MAX_FREQ_PFD is
 *        greater than PFD frequency.
 *
 * @param st    - The selected structure.
 * @param r_cnt - Initial r_cnt value.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4350_tune_r_cnt(struct adf4350_state *st, uint16_t r_cnt)
{
	struct adf4350_platform_data *pdata = st->pdata;

	do 
	{
		r_cnt++;
		st->fpfd = (st->clkin * (pdata->ref_doubler_en ? 2 : 1)) /
			   (r_cnt * (pdata->ref_div2_en ? 2 : 1));
	} while (st->fpfd > ADF4350_MAX_FREQ_PFD);

	return r_cnt;
}

/***************************************************************************//**
 * @brief Computes the greatest common divider of two numbers
 *
 * @return Returns the gcd.
*******************************************************************************/
uint32_t gcd(uint32_t x, uint32_t y)
{
	int32_t tmp;

	tmp = y > x ? x : y;


	while((x % tmp) || (y % tmp))
	{
		tmp--;
	}

	return tmp;
}

/***************************************************************************//**
 * @brief Sets the ADF4350 frequency.
 *
 * @param st   - The selected structure.
 * @param freq - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
int64_t adf4350_set_freq(struct adf4350_state *st, uint64_t freq)
{
	struct adf4350_platform_data *pdata = st->pdata;
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
	} 
	else
	{
		prescaler = 0;
		mdiv = 23;
	}

	st->r4_rf_div_sel = 0;

	while (freq < ADF4350_MIN_VCO_FREQ)
	{
		freq <<= 1;
		st->r4_rf_div_sel++;
	}

	/*
	 * Allow a predefined reference division factor
	 * if not set, compute our own
	 */
	if (pdata->ref_div_factor)
		r_cnt = pdata->ref_div_factor - 1;

	chspc = st->chspc;

	do  
	{
		do 
		{
			do  
			{
				r_cnt = adf4350_tune_r_cnt(st, r_cnt);
						st->r1_mod = st->fpfd / chspc;
						if (r_cnt > ADF4350_MAX_R_CNT)
						{
							/* try higher spacing values */
							chspc++;
							r_cnt = 0;
						}
			} while ((st->r1_mod > ADF4350_MAX_MODULUS) && r_cnt);
		} while (r_cnt == 0);


		tmp = freq * (uint64_t)st->r1_mod + (st->fpfd > 1);
		
		tmp = (tmp / st->fpfd);	/* Div round closest (n + d/2)/d */
		
		st->r0_fract = tmp % st->r1_mod;
		tmp = tmp / st->r1_mod;
		
		st->r0_int = tmp;
	} while (mdiv > st->r0_int);

	band_sel_div = st->fpfd % ADF4350_MAX_BANDSEL_CLK > ADF4350_MAX_BANDSEL_CLK / 2 ?
					st->fpfd / ADF4350_MAX_BANDSEL_CLK + 1 :
					st->fpfd / ADF4350_MAX_BANDSEL_CLK;

	if (st->r0_fract && st->r1_mod) {
		div_gcd = gcd(st->r1_mod, st->r0_fract);
		st->r1_mod /= div_gcd;
		st->r0_fract /= div_gcd;
	}
	else
	{
		st->r0_fract = 0;
		st->r1_mod = 1;
	}

	st->regs[ADF4350_REG0] = ADF4350_REG0_INT(st->r0_int) |
				 ADF4350_REG0_FRACT(st->r0_fract);

	st->regs[ADF4350_REG1] = ADF4350_REG1_PHASE(1) |
				 ADF4350_REG1_MOD(st->r1_mod) |
				 prescaler;

	st->regs[ADF4350_REG2] =
		ADF4350_REG2_10BIT_R_CNT(r_cnt) |
		ADF4350_REG2_DOUBLE_BUFF_EN |
		(pdata->ref_doubler_en ? ADF4350_REG2_RMULT2_EN : 0) |
		(pdata->ref_div2_en ? ADF4350_REG2_RDIV2_EN : 0) |
		(pdata->r2_user_settings & (ADF4350_REG2_PD_POLARITY_POS |
		ADF4350_REG2_LDP_6ns | ADF4350_REG2_LDF_INT_N |
		ADF4350_REG2_CHARGE_PUMP_CURR_uA(5000) |
		ADF4350_REG2_MUXOUT(0x7) | ADF4350_REG2_NOISE_MODE(0x9)));

	st->regs[ADF4350_REG3] = pdata->r3_user_settings &
				 (ADF4350_REG3_12BIT_CLKDIV(0xFFF) |
				 ADF4350_REG3_12BIT_CLKDIV_MODE(0x3) |
				 ADF4350_REG3_12BIT_CSR_EN);

	st->regs[ADF4350_REG4] =
		ADF4350_REG4_FEEDBACK_FUND |
		ADF4350_REG4_RF_DIV_SEL(st->r4_rf_div_sel) |
		ADF4350_REG4_8BIT_BAND_SEL_CLKDIV(band_sel_div) |
		ADF4350_REG4_RF_OUT_EN |
		(pdata->r4_user_settings &
		(ADF4350_REG4_OUTPUT_PWR(0x3) |
		ADF4350_REG4_AUX_OUTPUT_PWR(0x3) |
		ADF4350_REG4_AUX_OUTPUT_EN |
		ADF4350_REG4_AUX_OUTPUT_FUND |
		ADF4350_REG4_MUTE_TILL_LOCK_EN));

	st->regs[ADF4350_REG5] = ADF4350_REG5_LD_PIN_MODE_DIGITAL | 0x00180000;

	ret = adf4350_sync_config(st);
	if(ret < 0)
	{
		return ret;
	}

    tmp = (uint64_t)((st->r0_int * st->r1_mod) + st->r0_fract) * (uint64_t)st->fpfd;
    tmp = tmp / ((uint64_t)st->r1_mod * ((uint64_t)1 << st->r4_rf_div_sel));

	return tmp;
}

/***************************************************************************//**
 * @brief Initializes the ADF4350.
 *
 * @param spiBaseAddr - SPI peripheral AXI base address.
 * @param ssNo        - Slave select line on which the slave is connected.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4350_setup(uint32_t spi_device_id, uint8_t slave_select,
		adf4350_init_param init_param)
{
	struct adf4350_state *st = &adf4350_st;

	adf4350_slave_select = slave_select;
	spi_init(spi_device_id, 0, 0);

	st->pdata = (struct adf4350_platform_data *)malloc(sizeof(*st->pdata));
	if (!st->pdata)
		return -1;

	st->pdata->clkin = init_param.clkin;

	st->pdata->channel_spacing = init_param.channel_spacing;

	st->pdata->power_up_frequency = init_param.power_up_frequency;
	st->pdata->ref_div_factor = init_param.reference_div_factor;
	st->pdata->ref_doubler_en = init_param.reference_doubler_enable;
	st->pdata->ref_div2_en = init_param.reference_div2_enable;

	/* r2_user_settings */

	st->pdata->r2_user_settings = init_param.phase_detector_polarity_positive_enable ?
			ADF4350_REG2_PD_POLARITY_POS : 0;
	st->pdata->r2_user_settings |= init_param.lock_detect_precision_6ns_enable ?
			ADF4350_REG2_LDP_6ns : 0;
	st->pdata->r2_user_settings |= init_param.lock_detect_function_integer_n_enable ?
			ADF4350_REG2_LDF_INT_N : 0;
	st->pdata->r2_user_settings |= ADF4350_REG2_CHARGE_PUMP_CURR_uA(init_param.charge_pump_current);
	st->pdata->r2_user_settings |= ADF4350_REG2_MUXOUT(init_param.muxout_select);
	st->pdata->r2_user_settings |= init_param.low_spur_mode_enable ? ADF4350_REG2_NOISE_MODE(0x3) : 0;

	/* r3_user_settings */

	st->pdata->r3_user_settings = init_param.cycle_slip_reduction_enable ?
			ADF4350_REG3_12BIT_CSR_EN : 0;
	st->pdata->r3_user_settings |= init_param.charge_cancellation_enable ?
			ADF4351_REG3_CHARGE_CANCELLATION_EN : 0;
	st->pdata->r3_user_settings |= init_param.anti_backlash_3ns_enable ?
			ADF4351_REG3_ANTI_BACKLASH_3ns_EN : 0;
	st->pdata->r3_user_settings |= init_param.band_select_clock_mode_high_enable ?
			ADF4351_REG3_BAND_SEL_CLOCK_MODE_HIGH : 0;
	st->pdata->r3_user_settings |= ADF4350_REG3_12BIT_CLKDIV(init_param.clk_divider_12bit);
	st->pdata->r3_user_settings |= ADF4350_REG3_12BIT_CLKDIV_MODE(init_param.clk_divider_mode);

	/* r4_user_settings */

	st->pdata->r4_user_settings = init_param.aux_output_enable ?
			ADF4350_REG4_AUX_OUTPUT_EN : 0;
	st->pdata->r4_user_settings |= init_param.aux_output_fundamental_enable ?
			ADF4350_REG4_AUX_OUTPUT_FUND : 0;
	st->pdata->r4_user_settings |= init_param.mute_till_lock_enable ?
			ADF4350_REG4_MUTE_TILL_LOCK_EN : 0;
	st->pdata->r4_user_settings |= ADF4350_REG4_OUTPUT_PWR(init_param.output_power);
	st->pdata->r4_user_settings |= ADF4350_REG4_AUX_OUTPUT_PWR(init_param.aux_output_power);

	adf4350_out_altvoltage0_refin_frequency(st->pdata->clkin);
	adf4350_out_altvoltage0_frequency_resolution(st->pdata->channel_spacing);
	adf4350_out_altvoltage0_frequency(st->pdata->power_up_frequency);

	printf("ADF4350 successfully initialized.\n");

    return 0;
}

/***************************************************************************//**
 * @brief Stores PLL 0 frequency in Hz.
 *
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int64_t adf4350_out_altvoltage0_frequency(int64_t Hz)
{
	return adf4350_set_freq(&adf4350_st, Hz);
}

/***************************************************************************//**
 * @brief Stores PLL 0 frequency resolution/channel spacing in Hz.
 *
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int32_t adf4350_out_altvoltage0_frequency_resolution(int32_t Hz)
{
	if(Hz != INT32_MAX)
	{
		adf4350_st.chspc = Hz;
	}
	
	return adf4350_st.chspc;
}

/***************************************************************************//**
 * @brief Sets PLL 0 REFin frequency in Hz.
 *
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int64_t adf4350_out_altvoltage0_refin_frequency(int64_t Hz)
{
	if(Hz != INT32_MAX)
	{
		adf4350_st.clkin = Hz;
	}
	
	return adf4350_st.clkin;
}

/***************************************************************************//**
 * @brief Powers down the PLL.
 *
 * @param pwd - Power option.
 *				Example: 0 - Power up the PLL.
 *						 1 - Power down the PLL.
 *
 * @return Returns the PLL's power status.
*******************************************************************************/
int32_t adf4350_out_altvoltage0_powerdown(int32_t pwd)
{
	struct adf4350_state *st = &adf4350_st;
	
	if(pwd == 1)
	{
		st->regs[ADF4350_REG2] |= ADF4350_REG2_POWER_DOWN_EN;
		adf4350_sync_config(st);
	}
	if(pwd == 0)
	{
		st->regs[ADF4350_REG2] &= ~ADF4350_REG2_POWER_DOWN_EN;
		adf4350_sync_config(st);
	}
	

	return (st->regs[ADF4350_REG2] & ADF4350_REG2_POWER_DOWN_EN);
}
