/***************************************************************************//**
 *   @file   AD9548.c
 *   @brief  Implementation of AD9548 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "spi_interface.h"
#include "AD9548.h"
#include "AD9548_cfg.h"

/******************************************************************************/
/************************ Local variables and types ***************************/
/******************************************************************************/
struct ad9548_state 
{
    struct ad9548_platform_data *pdata;
}ad9548_st;

enum ad9548_raw_masks
{
    CHAN_INFO_RAW,
	CHAN_INFO_FREQUENCY
};

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param registerAddress - The address of the register to write to.
 * @param registerValue - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9548_write(uint16_t registerAddress,
				     uint8_t registerValue)
{
	uint16_t regAddr = 0;

	regAddr = (registerAddress & 0x1FFF);
	return SPI_Write(SPI_SEL_AD9548, regAddr, registerValue);
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param registerAddress - The address of the register to read.
 *
 * @return registerValue - The register's value or negative error code.
*******************************************************************************/
int32_t ad9548_read(uint16_t registerAddress)
{
	uint16_t	regAddr = 0;
	uint32_t	registerValue = 0;
	int32_t ret;

	regAddr = 0x8000 + (registerAddress & 0x1FFF);
	ret = SPI_Read(SPI_SEL_AD9548, regAddr, &registerValue);

	return (ret < 0 ? ret : (int32_t)registerValue);
}

/***************************************************************************//**
 * @brief Updates the IO configuration
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
int32_t ad9548_update_io()
{
	int32_t ret;

	ret = ad9548_write(AD9548_REG_IO_UPDATE, 0x01);
	if(ret < 0)
		return ret;

	ret = ad9548_write(AD9548_REG_UPDATE_TW, 0x01);

	return ret;
}

/***************************************************************************//**
 * @brief Reads data from the AD9548
 *
 * @param channel - channel number (0 or 1)
 * @param val - pointer to store the first 32 bits of data
 * @param val2 - pointer to store the last 32 bits of data
 * @param m - the code of the data to read
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
int32_t ad9548_read_raw(int32_t channel,
                    int32_t *val,
                    int32_t *val2,
                    int32_t m)
{
	struct ad9548_state *st = &ad9548_st;
	int32_t ret, distr_settings, div = 0;

	distr_settings = ad9548_read(AD9548_REG_DISTRIBUTION_SETTINGS);
	if (distr_settings < 0)
		return distr_settings;

    div = ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_0 + channel*4) + 
          (ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_1 + channel*4) >> 8) +
          (ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_2 + channel*4) >> 16) +
          (ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_3 + channel*4) >> 24);
    if(div < 0)
        return -1;

	switch (m)
	{
	case CHAN_INFO_RAW:
		*val = distr_settings & (1 << channel);
		return 0;
	case CHAN_INFO_FREQUENCY:
		ret = ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_0 + channel*4);
		if (ret < 0)
			return -1;
		div +=ret;
		ret = ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_1 + channel*4);
		if (ret < 0)
			return -1;
		div += ret << 8;
		ret = ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_2 + channel*4);
		if (ret < 0)
			return -1;
		div += ret << 16;
		ret = ad9548_read(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_3 + channel*4);
		if (ret < 0)
			return -1;
		div += ret << 24;
		*val = st->pdata->dpll_out_freq * div;
		return 0;
	default:
		return -1;
	}
};

/***************************************************************************//**
 * @brief Writes data to the AD9548
 *
 * @param channel - channel number (0 or 1)
 * @param val - the first 32 bits of data to write
 * @param val2 - the last 32 bits of data to write
 * @param m - the code of the data to write
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
int32_t ad9548_write_raw(int32_t channel,
                     int32_t val,
                     int32_t val2,
                     int32_t mask)
{
	struct ad9548_state *st = &ad9548_st;
	uint32_t reg;
	int32_t ret, tmp;

	ret = ad9548_read(AD9548_REG_DISTRIBUTION_SETTINGS);
	if (ret < 0)
		goto out;

	reg = ret;

	switch (mask) {
	case CHAN_INFO_RAW:
		if (val)
			reg &= ~(1 << channel);
		else
			reg |= (1 << channel);
		ret = ad9548_write(AD9548_REG_DISTRIBUTION_SETTINGS, reg);
	    if (ret < 0)
		    goto out;
        break;
	case CHAN_INFO_FREQUENCY:
		if (val <= 0) {
			ret = -1;
			goto out;
		}
		tmp = st->pdata->dpll_out_freq / val;
		ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_0 + channel*4, tmp & 0xFF);
        if (ret < 0)
		    goto out;
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_1 + channel*4, (tmp >> 8) & 0xFF);
        if (ret < 0)
		    goto out;
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_2 + channel*4, (tmp >> 16) & 0xFF); 
        if (ret < 0)
		    goto out;
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_3 + channel*4, (tmp >> 24) & 0x3F);
        if (ret < 0)
		    goto out;
		break;
	default:
		ret = -1;
		goto out;
	}	

	ret = ad9548_update_io();
	if(ret < 0)
		return ret;
out:

	return ret;
}

/***************************************************************************//**
 * @brief Sets the output frequency for the specified channel
 *
 * @param channel - Channel number
 * @param Hz - Frequency in Hz
 *
 * @return Returns the actual set frequency or negative error code
*******************************************************************************/
int64_t ad9548_out_frequency(int32_t channel, int64_t Hz)
{
    int32_t ret;
    int32_t ret_freq;

    if(Hz != INT64_MAX)
    {
        ret = ad9548_write_raw(channel, (int32_t)Hz, 0, CHAN_INFO_FREQUENCY);
        if(ret < 0)
            return ret;

        ret_freq = (int32_t)Hz;
    }
    else
    {
        ret = ad9548_read_raw(channel, &ret_freq, 0, CHAN_INFO_FREQUENCY);
        if(ret < 0)
            return ret;
    }

    return (int64_t)ret_freq;
}

/***************************************************************************//**
 * @brief Sets the output frequency for output channel 1. Receives as parameter
 *
 * @param Hz - The output frequency in Hz
 *
 * @return Returns negative error code or the actual frequency.
*******************************************************************************/
int64_t ad9548_out_altvoltage0_frequency(int64_t Hz)
{
    return ad9548_out_frequency(0, Hz);
}

/***************************************************************************//**
 * @brief Sets the output frequency for output channel 2. Receives as parameter
 *
 * @param Hz - The output frequency in Hz
 *
 * @return Returns negative error code or the actual frequency.
*******************************************************************************/
int64_t ad9548_out_altvoltage1_frequency(int64_t Hz)
{
    return ad9548_out_frequency(1, Hz);
}

/***************************************************************************//**
 * @brief Sets the output frequency for output channel 2.
 *
 * @param Hz - The output frequency in Hz
 *
 * @return Returns negative error code or the actual frequency.
*******************************************************************************/
int64_t ad9548_out_altvoltage2_frequency(int64_t Hz)
{
    return ad9548_out_frequency(2, Hz);
}

/***************************************************************************//**
 * @brief Sets the output frequency for output channel 3.
 *
 * @param Hz - The output frequency in Hz
 *
 * @return Returns negative error code or the actual frequency.
*******************************************************************************/
int64_t ad9548_out_altvoltage3_frequency(int64_t Hz)
{
    return ad9548_out_frequency(3, Hz);
}

/***************************************************************************//**
 * @brief Triggers the clock distribution synchronization functionality.
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
int32_t ad9548_sync_dividers()
{
    int32_t ret;

    ret = ad9548_write(AD9548_REG_CAL_SYNC, 0x02);
    if(ret < 0)
        return ret;

    ret = ad9548_write(AD9548_REG_IO_UPDATE, 0x01);
    if(ret < 0)
    	return ret;

    ret = ad9548_write(AD9548_REG_CAL_SYNC, 0x00);
    if(ret < 0)
    	return ret;

    ret = ad9548_write(AD9548_REG_IO_UPDATE, 0x01);

    return ret;
}

/***************************************************************************//**
 * @brief Triggers system clock calibration.
 *
 * @return Returns 0 in case of success or negative error code
*******************************************************************************/
int32_t ad9548_calibrate_sys_clk()
{
    int32_t ret;

    ret = ad9548_write(AD9548_REG_CAL_SYNC, 0x01);
    if(ret < 0)
        return ret;

    ret = ad9548_write(AD9548_REG_IO_UPDATE, 0x01);
    if(ret < 0)
        return ret;

    while((ad9548_read(AD9548_REG_SYSTEM_CLOCK) & 0x01) == 0);

    ret = ad9548_write(AD9548_REG_CAL_SYNC, 0x00);
    if(ret < 0)
    	return ret;

    ret = ad9548_write(AD9548_REG_IO_UPDATE, 0x01);

    return ret;

}

/***************************************************************************//**
 * @brief Sets the power down mode for a specified component
 *
 * @param en - set to 0/1 for disable/enable
 * @param bit - component index
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t general_pwd(int32_t en, int32_t bit)
{
    int32_t ret;

    ret = ad9548_read(AD9548_REG_GENERAL_POWER_DOWN);
    if(ret < 0)
        return ret;

    if(en == 0 || en == 1)
    {
        ret &= ~(1 << bit);
        ret = ad9548_write(AD9548_REG_GENERAL_POWER_DOWN, ret | (en << bit));
        if(ret < 0)
            return ret;

        ret = ad9548_read(AD9548_REG_GENERAL_POWER_DOWN);
        if(ret < 0)
            return ret;
    }

    return ((ret & (1 << bit)) != 0);
}

/***************************************************************************//**
 * @brief Reset internal hardware but retain programmed register values
 *
 * @param en - set to 0/1 for disable/enable
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t ad9548_reset_sans_reg_map(int32_t en)
{
    return general_pwd(en, 7);
}

/***************************************************************************//**
 * @brief Sets the SYS CLK power mode
 *
 * @param en - set to 0/1 for disable/enable
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t ad9548_sys_clk_pwd(int32_t en)
{
    return general_pwd(en, 5);
}

/***************************************************************************//**
 * @brief Sets the reference power mode
 *
 * @param en - set to 0/1 for disable/enable
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t ad9548_reference_pwd(int32_t en)
{
    return general_pwd(en, 4);
}

/***************************************************************************//**
 * @brief Sets the TDC power mode
 *
 * @param en - set to 0/1 for disable/enable
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t ad9548_tdc_pwd(int32_t en)
{
    return general_pwd(en, 3);
}

/***************************************************************************//**
 * @brief Sets the DAC power mode
 *
 * @param en - set to 0/1 for disable/enable
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t ad9548_dac_pwd(int32_t en)
{
    return general_pwd(en, 2);
}

/***************************************************************************//**
 * @brief Sets the distribution power mode
 *
 * @param en - set to 0/1 for disable/enable
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t ad9548_dist_pwd(int32_t en)
{
    return general_pwd(en, 1);
}

/***************************************************************************//**
 * @brief Sets the full power down mode
 *
 * @param en - set to 0/1 for disable/enable
 *
 * @return Returns the enable state or negative error code
*******************************************************************************/
int32_t ad9548_full_pwd(int32_t en)
{
    return general_pwd(en, 0);
}

/***************************************************************************//**
 * @brief Returns the stability status of the SYS CLK
*******************************************************************************/
int32_t ad9548_sys_clk_stable()
{
    int32_t ret;

    ret = ad9548_read(AD9548_REG_SYSTEM_CLOCK);
    if(ret < 0)
        return ret;

    return ((ret & (1 << 4)) != 0);
}

/***************************************************************************//**
 * @brief Returns the PLL lock status
*******************************************************************************/
int32_t ad9548_sys_clk_pll_locked()
{
    int32_t ret;

    ret = ad9548_read(AD9548_REG_SYSTEM_CLOCK);
    if(ret < 0)
        return ret;

    return ((ret & (1 << 0)) != 0);
}

/***************************************************************************//**
 * @brief Returns the DPLL phase lock status
*******************************************************************************/
int32_t ad9548_dpll_phase_locked()
{
    int32_t ret;

    ret = ad9548_read(AD9548_REG_IRQ_MONITOR_2);
    if(ret < 0)
        return ret;

    return ((ret & (1 << 0)) != 0);
}

/***************************************************************************//**
 * @brief Returns the DPLL frequency lock status
*******************************************************************************/
int32_t ad9548_dpll_frequency_locked()
{
    int32_t ret;

    ret = ad9548_read(AD9548_REG_IRQ_MONITOR_2);
    if(ret < 0)
        return ret;

    return ((ret & (1 << 2)) != 0);
}

/***************************************************************************//**
 * @brief Returns the state of the specified REF input
 *
 * @param ref_no - reference index
 *
 * @return Returns the state of the specified REF input or negative error code
*******************************************************************************/
int32_t ad9548_ref_state(int32_t ref_no)
{
    int32_t ret;

    ret = ad9548_read(AD9548_REG_IRQ_MONITOR_4 + ref_no/2);
    if(ret < 0)
        return ret;

    ret = ref_no % 2 ? ((ret & 0xF0) >> 8) : (ret & 0x0F);

    return ret;
}

/***************************************************************************//**
 * @brief Returns the REFA state
*******************************************************************************/
int32_t ad9548_refa_state()
{
    return ad9548_ref_state(0);
}

/***************************************************************************//**
 * @brief Returns the REFAA state
*******************************************************************************/
int32_t ad9548_refaa_state()
{
    return ad9548_ref_state(1);
}

/***************************************************************************//**
 * @brief Returns the REFB state
*******************************************************************************/
int32_t ad9548_refb_state()
{
	return ad9548_ref_state(2);
}

/***************************************************************************//**
 * @brief Returns the REFBB state
*******************************************************************************/
int32_t ad9548_refbb_state()
{
	return ad9548_ref_state(3);
}

/***************************************************************************//**
 * @brief Returns the REFC state
*******************************************************************************/
int32_t ad9548_refc_state()
{
	return ad9548_ref_state(4);
}

/***************************************************************************//**
 * @brief Returns the REFCC state
*******************************************************************************/
int32_t ad9548_refcc_state()
{
	return ad9548_ref_state(5);
}

/***************************************************************************//**
 * @brief Returns the REFD state
*******************************************************************************/
int32_t ad9548_refd_state()
{
	return ad9548_ref_state(6);
}

/***************************************************************************//**
 * @brief Returns the REFDD state
*******************************************************************************/
int32_t ad9548_refdd_state()
{
	return ad9548_ref_state(7);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9548_reset(void)
{
	int32_t ret;

    ret = ad9548_write(AD9548_REG_SPI_CTRL, 0x30);

    return ret;
}

/***************************************************************************//**
 * @brief Initializes the AD9548.
 *
 * @return status - Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9548_setup(void)
{
    struct ad9548_state *st = &ad9548_st;
    struct ad9548_platform_data *pdata = &ad9548_pdata_lpc;
	int32_t ret, i, ref_pwd; 
    int32_t ref_logic0, ref_logic1;
    int32_t distr_settings, distr_en, distr_sync;
	
    st->pdata = &ad9548_pdata_lpc;

	/* Serial port control and part identification */
	ret = ad9548_reset();
	if(ret < 0)
        return ret;
    
    ret = ad9548_write(AD9548_REG_SPI_CTRL, 0x10);
    ret = ad9548_write(AD9548_REG_SPI_CTRL, 0x10);
    ret = ad9548_write(AD9548_REG_SPI_CTRL, 0x10);
    ret = ad9548_write(AD9548_REG_SPI_CTRL, 0x10);
	if(ret < 0)
        return ret;
	
    /* System clock */
    ret = ad9548_write(AD9548_REG_SYSCLK_0, 
                      (pdata->sys_clk_ext_loop_filter_en << 7) | 
                      (pdata->sys_clk_charge_pump_manual_mode_en << 6) |
                      (pdata->sys_clk_charge_pump_current << 3) |
                      (pdata->sys_clk_pll_lock_detect_timer_dis << 2) |
                      (pdata->sys_clk_pll_lock_detect_timer));
    if(ret < 0)
        return ret;

	ret = ad9548_write(AD9548_REG_SYSCLK_1, pdata->sys_clk_fedback_div);
    if(ret < 0)
        return ret;
    
	ret = ad9548_write(AD9548_REG_SYSCLK_2, 
                       (pdata->sys_clk_m_div ? 0 : 1 << 6) | 
                       (pdata->sys_clk_m_div << 4) |
                       (pdata->sys_clk_2x_mul_en << 3) |
                       (pdata->sys_clk_pll_en << 2) |
                       (pdata->sys_clk_source));
    if(ret < 0)
        return ret;

	ret = ad9548_write(AD9548_REG_NOM_SYSCLK_PERIOD_0, (pdata->sys_clk_period & 0xFF));
    if(ret < 0)
        return ret;	
    ret = ad9548_write(AD9548_REG_NOM_SYSCLK_PERIOD_1,  ((pdata->sys_clk_period >> 8) & 0xFF));
	if(ret < 0)
        return ret;    
    ret = ad9548_write(AD9548_REG_NOM_SYSCLK_PERIOD_2, ((pdata->sys_clk_period >> 16) & 0x1F));
    if(ret < 0)
        return ret;	
    
    ret = ad9548_write(AD9548_REG_SYSTEM_CLK_STABILITY_0, (pdata->sys_clk_stability & 0xFF));
	if(ret < 0)
        return ret;
    ret = ad9548_write(AD9548_REG_SYSTEM_CLK_STABILITY_1, ((pdata->sys_clk_stability >> 8) & 0xFF));
	if(ret < 0)
        return ret;
    ret = ad9548_write(AD9548_REG_SYSTEM_CLK_STABILITY_2, ((pdata->sys_clk_stability >> 16) & 0x0F));
    if(ret < 0)
        return ret;

    /* General configuration */
	ret = ad9548_write(AD9548_REG_WATCHDOG_TIMER_0, (pdata->watchdog_timer & 0xFF));
    if(ret < 0)
        return ret;
	ret = ad9548_write(AD9548_REG_WATCHDOG_TIMER_1, ((pdata->watchdog_timer >> 8) & 0xFF));
    if(ret < 0)
        return ret;
	
    ret = ad9548_write(AD9548_REG_DAC_CURRENT_0, (pdata->aux_dac_full_scale_current & 0xff));
	if(ret < 0)
        return ret;
    ret = ad9548_write(AD9548_REG_DAC_CURRENT_1, ((pdata->aux_dac_full_scale_current >> 8) & 0x03));
   	if(ret < 0)
        return ret;

    /* DPLL */
	ret = ad9548_write(AD9548_REG_TUNING_WORD_0, pdata->dpll_tunning_word0);
   	if(ret < 0)
        return ret;
	ret = ad9548_write(AD9548_REG_TUNING_WORD_1, pdata->dpll_tunning_word1);
   	if(ret < 0)
        return ret;
    ret = ad9548_write(AD9548_REG_TUNING_WORD_2, pdata->dpll_tunning_word2);
   	if(ret < 0)
        return ret;
    ret = ad9548_write(AD9548_REG_TUNING_WORD_3, pdata->dpll_tunning_word3);
   	if(ret < 0)
        return ret;
    ret = ad9548_write(AD9548_REG_TUNING_WORD_4, pdata->dpll_tunning_word4);
   	if(ret < 0)
        return ret;
    ret = ad9548_write(AD9548_REG_TUNING_WORD_5, pdata->dpll_tunning_word5);
   	if(ret < 0)
        return ret;	
    
    ret = ad9548_write(AD9548_REG_PULL_IN_RANGE_LIMITS_0, (pdata->dpll_pull_in_range_limit_low & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_PULL_IN_RANGE_LIMITS_1, ((pdata->dpll_pull_in_range_limit_low >> 8) & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_PULL_IN_RANGE_LIMITS_2, ((pdata->dpll_pull_in_range_limit_low >> 16) & 0xFF));
    if(ret < 0)
        return ret;	
    
    ret = ad9548_write(AD9548_REG_PULL_IN_RANGE_LIMITS_3, (pdata->dpll_pull_in_range_limit_high & 0xFF));
	if(ret < 0)
        return ret;	
    ret = ad9548_write(AD9548_REG_PULL_IN_RANGE_LIMITS_4, ((pdata->dpll_pull_in_range_limit_high >> 8) & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_PULL_IN_RANGE_LIMITS_5, ((pdata->dpll_pull_in_range_limit_high >> 16) & 0xFF));
    if(ret < 0)
        return ret;	
	    
    ret = ad9548_write(AD9548_REG_OPEN_LOOP_PHASE_OFFSET_0, (pdata->dpll_dds_phase_offset & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_OPEN_LOOP_PHASE_OFFSET_1, ((pdata->dpll_dds_phase_offset >> 8) & 0xFF));
    if(ret < 0)
        return ret;	

	ret = ad9548_write(AD9548_REG_CLOSED_LOOP_PHASE_LOCK_OFFSET_0, 
                      (pdata->dpll_closed_loop_phase_lock_offset_low & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_CLOSED_LOOP_PHASE_LOCK_OFFSET_1, 
                      ((pdata->dpll_closed_loop_phase_lock_offset_low >> 8) & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_CLOSED_LOOP_PHASE_LOCK_OFFSET_2, 
                      ((pdata->dpll_closed_loop_phase_lock_offset_low >> 16) & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_CLOSED_LOOP_PHASE_LOCK_OFFSET_3, 
                      ((pdata->dpll_closed_loop_phase_lock_offset_low >> 24) & 0xFF));
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_CLOSED_LOOP_PHASE_LOCK_OFFSET_4, 
                      (pdata->dpll_closed_loop_phase_lock_offset_high & 0xFF));
    if(ret < 0)
        return ret;	
	
    ret = ad9548_write(AD9548_REG_INCREMENTAL_PHASE_LOCK_OFFSET_1,
    				   pdata->dpll_incremental_phase_lock_offset & 0xFF);
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_INCREMENTAL_PHASE_LOCK_OFFSET_2,
					  (pdata->dpll_incremental_phase_lock_offset >> 8) & 0xFF);
    if(ret < 0)
        return ret;	
	
    ret = ad9548_write(AD9548_REG_PHASE_SLEW_LIMIT_0, pdata->dpll_phase_slew_limit & 0xFF);
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_PHASE_SLEW_LIMIT_1, (pdata->dpll_phase_slew_limit >> 8) & 0xFF);
    if(ret < 0)
        return ret;	
	
    ret = ad9548_write(AD9548_REG_HISTORY_ACCUMULATION_TIMER_0, pdata->dpll_history_acc_timer & 0xFF);
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_HISTORY_ACCUMULATION_TIMER_1, (pdata->dpll_history_acc_timer >> 8) & 0xFF);
    if(ret < 0)
        return ret;	
	ret = ad9548_write(AD9548_REG_HISTORY_ACCUMULATION_TIMER_2, (pdata->dpll_history_acc_timer >> 16) & 0xFF);
    if(ret < 0)
        return ret;	
	
    ret = ad9548_write(AD9548_REG_HISTORY_MODE, (uint8_t)pdata->dpll_history_mode);
    if(ret < 0)
        return ret;		
    
    /* Clock distribution output */
	distr_settings = (pdata->clock_distr_ext_resistor << 5) |
                     (pdata->clock_distr_high_freq_mode << 4) | 
                     0xFF;
    ret = ad9548_write(AD9548_REG_DISTRIBUTION_SETTINGS, distr_settings);
    if(ret < 0)
        return ret;

	ret = ad9548_write(AD9548_REG_DISTRIBUTION_SYNCRONIZATION, (uint8_t)pdata->clock_distr_sync_source);
    if(ret < 0)
        return ret;

	ret = ad9548_write(AD9548_REG_AUTOMATIC_SYNCRONIZATION,  (uint8_t)pdata->clock_distr_automatic_sync_mode);
    if(ret < 0)
        return ret;
	
    /* Reference inputs configuration */
    ref_pwd = 0xFF;
    ref_logic0 = 0x00;
    ref_logic1 = 0x00;
    for(i = 0; i < pdata->num_references; i++)
    {
        ref_pwd &= ~((!pdata->references[i].power_down_en) << pdata->references[i].ref_num); 
        ret = ad9548_write(AD9548_REG_REFERENCE_POWER_DOWN, ref_pwd);
        if(ret < 0)
            return ret;

        if(!(pdata->references[i].ref_num/4))
        {
            ref_logic0 |= (pdata->references[i].logic_family << ((pdata->references[i].ref_num % 4)*2));
            ret = ad9548_write(AD9548_REG_REFERENCE_LOGIC_FAMILY_0, ref_logic0); 
        }
        else
        {
            ref_logic1 |= (pdata->references[i].logic_family << ((pdata->references[i].ref_num % 4)*2));
            ret = ad9548_write(AD9548_REG_REFERENCE_LOGIC_FAMILY_1, ref_logic1); 
        }
        if(ret < 0)
            return ret;

        ret = ad9548_write(AD9548_REG_MANUAL_REFERENCE_PROFILE_SEL_0 + pdata->references[i].ref_num/2, 
                           (pdata->references[i].manual_profile_en << ((pdata->references[i].ref_num % 2)*4 + 3)) | 
                           (pdata->references[i].manual_profile << ((pdata->references[i].ref_num % 2)*4)));
        if(ret < 0)
            return ret;
    }

    /* Output channels configuration */
    distr_en = 0x00;
    distr_sync = 0x00;
    for(i = 0; i < pdata->num_channels; i++)
    {
        distr_settings &= ~((!pdata->channels[i].low_power_mode_en) << pdata->channels[i].channel_num);
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_SETTINGS, distr_settings);
        if(ret < 0)
            return ret;

        distr_en |= (!pdata->channels[i].output_dis) << pdata->channels[i].channel_num;
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_ENABLE, distr_en);
        if(ret < 0)
            return ret;
                
        distr_sync |= pdata->channels[i].low_power_sync_en << pdata->channels[i].channel_num;
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_SYNCRONIZATION, distr_sync);
        if(ret < 0)
            return ret;

        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_MODES_0 + pdata->channels[i].channel_num, 
                           (pdata->channels[i].driver_mode) |
                           (pdata->channels[i].drive_strength << 3) |
                           (pdata->channels[i].polarity_invert_en << 4) |
                           (pdata->channels[i].cmos_mode_phase_invert << 5));
        if(ret < 0)
            return ret;

        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_0 + pdata->channels[i].channel_num*4, 
                           (pdata->channels[i].channel_divider & 0xFF));
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_1 + pdata->channels[i].channel_num*4, 
                           ((pdata->channels[i].channel_divider >> 8) & 0xFF));
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_2 + pdata->channels[i].channel_num*4, 
                           ((pdata->channels[i].channel_divider >> 16) & 0xFF));
        ret = ad9548_write(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_3 + pdata->channels[i].channel_num*4, 
                           ((pdata->channels[i].channel_divider >> 24) & 0xFF));
        if(ret < 0)
            return ret;
    }        
	
    ret = ad9548_update_io();
    if(ret < 0)
    	return ret;

	ret = ad9548_calibrate_sys_clk();
	if(ret < 0)
		return ret;

	ret = ad9548_sync_dividers();
	if(ret < 0)
		return ret;
	
	return ret;
}
