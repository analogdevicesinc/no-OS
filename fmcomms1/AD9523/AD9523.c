/***************************************************************************//**
 *   @file   AD9523.c
 *   @brief  Implementation of AD9523 Driver.
 *   @author ACozma(andrei.cozma@analog.com)
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
#include <math.h>
#include "spi_interface.h"
#include "AD9523.h"
#include "AD9523_cfg.h"

/******************************************************************************/
/************************ Local variables and types ***************************/
/******************************************************************************/
struct ad9523_state 
{
    struct ad9523_platform_data *pdata;
    uint32_t vcxo_freq;
    uint32_t vco_freq;
    uint32_t vco_out_freq[3];
    uint8_t vco_out_map[14];
}ad9523_st;

enum ad9523_raw_masks
{
    CHAN_INFO_RAW,
	CHAN_INFO_FREQUENCY,
    CHAN_INFO_PHASE
};

enum 
{
	AD9523_STAT_PLL1_LD,
	AD9523_STAT_PLL2_LD,
	AD9523_STAT_REFA,
	AD9523_STAT_REFB,
	AD9523_STAT_REF_TEST,
	AD9523_STAT_VCXO,
	AD9523_STAT_PLL2_FB_CLK,
	AD9523_STAT_PLL2_REF_CLK,
	AD9523_SYNC,
	AD9523_EEPROM,
};

enum 
{
	AD9523_VCO1,
	AD9523_VCO2,
	AD9523_VCXO,
	AD9523_NUM_CLK_SRC,
};

/* Platform dependent sleep function */
extern void delay_us(uint32_t us_count);

/* Helpers to avoid excess line breaks */
#define AD_IFE(_pde, _a, _b) ((pdata->_pde) ? _a : _b)
#define AD_IF(_pde, _a) AD_IFE(_pde, _a, 0)

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param registerAddress - The address of the register to read.
 *
 * @return registerValue - The register's value or negative error code.
*******************************************************************************/
int32_t ad9523_read(uint32_t registerAddress)
{
	uint16_t	regAddr = 0;
	uint32_t	registerValue = 0;
	uint32_t	registerValue1 = 0;
	int32_t ret;
	uint32_t i = 0;

	regAddr = 0x8000 + (registerAddress & 0x1FFF);
	for(i = 0; i < AD9523_TRANSF_LEN(registerAddress); i++)
	{
		ret = SPI_Read(SPI_SEL_AD9523, regAddr, &registerValue1);
		if(ret < 0)
			return ret;
		regAddr--;
		registerValue <<= 8;
		registerValue |= registerValue1;
	}
	return (int32_t)registerValue;
}

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param registerAddress - The address of the register to write to.
 * @param registerValue - The value to write to the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_write(uint32_t registerAddress,
				     uint32_t registerValue)
{
	uint32_t i = 0;
	int32_t ret = -1;
	uint16_t regAddr = 0;

	regAddr = (registerAddress & 0x1FFF);
	for(i = 0; i < AD9523_TRANSF_LEN(registerAddress); i++)
	{
		ret = SPI_Write(SPI_SEL_AD9523, regAddr, (registerValue >> ((AD9523_TRANSF_LEN(registerAddress) - i - 1) * 8)) & 0xFF);
		if(ret < 0)
			return ret;
		regAddr--;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Updates the AD9523 configuration
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_io_update()
{
    return ad9523_write(AD9523_IO_UPDATE, AD9523_IO_UPDATE_EN);
}

/***************************************************************************//**
 * @brief Sets the clock provider for selected channel.
 *
 * @param ch - Selected channel.
 * @param out - Selected clock provider.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_vco_out_map(uint32_t ch, uint32_t out)
{
	struct ad9523_state *st = &ad9523_st;
	int32_t ret;
	uint32_t mask;

	switch (ch) {
    case 0:
    case 1:
    case 2: 
    case 3:
		ret = ad9523_read(AD9523_PLL1_OUTPUT_CHANNEL_CTRL);
		if (ret < 0)
			break;
		mask = AD9523_PLL1_OUTP_CH_CTRL_VCXO_SRC_SEL_CH0 << ch;
		if (out) {
			ret |= mask;
			out = 2;
		} else {
			ret &= ~mask;
		}
		ret = ad9523_write(AD9523_PLL1_OUTPUT_CHANNEL_CTRL, ret);
		break;
    case 4:
    case 5:
    case 6:
		ret = ad9523_read(AD9523_PLL1_OUTPUT_CTRL);
		if (ret < 0)
			break;
		mask = AD9523_PLL1_OUTP_CTRL_VCO_DIV_SEL_CH4_M2 << (ch - 4);
		if (out)
			ret |= mask;
		else
			ret &= ~mask;
		ret = ad9523_write(AD9523_PLL1_OUTPUT_CTRL, ret);
		break;
    case 7:
    case 8:
    case 9:
		ret = ad9523_read(AD9523_PLL1_OUTPUT_CHANNEL_CTRL);
		if (ret < 0)
			break;
		mask = AD9523_PLL1_OUTP_CH_CTRL_VCO_DIV_SEL_CH7_M2 << (ch - 7);
		if (out)
			ret |= mask;
		else
			ret &= ~mask;
		ret = ad9523_write(AD9523_PLL1_OUTPUT_CHANNEL_CTRL, ret);
		break;
	default:
		return 0;
	}

	st->vco_out_map[ch] = out;

	return ret;
}

/***************************************************************************//**
 * @brief Sets the clock provider for selected channel.
 *
 * @param ch - Selected channel.
 * @param freq - The output frequency of the selected channel. This parameter is
 *				 required for selecting the VCXO or the VCO clock for channels
 *				 0 - 3, or the the best VCO Divider (M1 or M2) for channels
 *				 4 - 9.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_set_clock_provider(int32_t ch, uint32_t freq)
{
	struct ad9523_state *st = &ad9523_st;
	int32_t tmp1, tmp2;
	uint8_t use_alt_clk_src;

	switch (ch) {
    case 0:
    case 1:
    case 2:
    case 3:
		use_alt_clk_src = (freq == st->vco_out_freq[AD9523_VCXO]);
		break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
		tmp1 = st->vco_out_freq[AD9523_VCO1] / freq;
		tmp2 = st->vco_out_freq[AD9523_VCO2] / freq;
		tmp1 *= freq;
		tmp2 *= freq;
		use_alt_clk_src = ((freq - tmp1) > (freq - tmp2));
		break;
	default:
		/* Ch 10..14: No action required, return success */
		return 0;
	}

	return ad9523_vco_out_map(ch, use_alt_clk_src);
}

/***************************************************************************//**
 * @brief Reads the frequency, the phase or the power status of selected
 *		  channel.
 *
 * @param val - pointer to store the first 32 bits of data
 * @param val2 - pointer to store the last 32 bits of data
 * @param mask - The parameter selected to be read.
 *				 Example: CHAN_INFO_RAW
 *						  CHAN_INFO_FREQUENCY
 *						  CHAN_INFO_PHASE
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_read_raw(int32_t channel,
                         int32_t *val,
                         int32_t *val2,
                         int32_t mask)
{
	struct ad9523_state *st = &ad9523_st;
	uint32_t code;
	int32_t ret;

	ret = ad9523_read(AD9523_CHANNEL_CLOCK_DIST(channel));

	if (ret < 0)
		return ret;

	switch (mask) {
	case CHAN_INFO_RAW:
		*val = !(ret & AD9523_CLK_DIST_PWR_DOWN_EN);
		return 0;
	case CHAN_INFO_FREQUENCY:
		*val = st->vco_out_freq[st->vco_out_map[channel]] /
			AD9523_CLK_DIST_DIV_REV(ret);
		return 0;
	case CHAN_INFO_PHASE:
		code = (AD9523_CLK_DIST_DIV_PHASE_REV(ret) * 3141592) /
			AD9523_CLK_DIST_DIV_REV(ret);
		*val = code / 1000000;
		*val2 = (code % 1000000) * 10;
		return 0;
	default:
		return -1;
	}
};

/***************************************************************************//**
 * @brief Writes the frequency, the phase or the power status for selected
 *		  channel.
 *
 * @param val - the first 32 bits of data to write
 * @param val2 - the last 32 bits of data to write
 * @param mask - The parameter selected to be written.
 *				 Example: CHAN_INFO_RAW
 *						  CHAN_INFO_FREQUENCY
 *						  CHAN_INFO_PHASE
 *
 * @return Returns 0 in case of success or negative error code. 
 *         When setting the frequency the returned value is the set frequency 
 *         or negative error code.
*******************************************************************************/
int32_t ad9523_write_raw(int32_t channel,
                         int32_t val,
                         int32_t val2,
                         int32_t mask)
{
	struct ad9523_state *st = &ad9523_st;
	uint32_t reg;
	int32_t ret, tmp, code;
    int32_t ret_val = 0;

	ret = ad9523_read(AD9523_CHANNEL_CLOCK_DIST(channel));
	if (ret < 0)
		goto out;

	reg = ret;

	switch (mask) {
	case CHAN_INFO_RAW:
		if (val)
			reg &= ~AD9523_CLK_DIST_PWR_DOWN_EN;
		else
			reg |= AD9523_CLK_DIST_PWR_DOWN_EN;
		break;
	case CHAN_INFO_FREQUENCY:
		if (val <= 0) {
			ret = -1;
			goto out;
		}
		ret = ad9523_set_clock_provider(channel, val);
		if (ret < 0)
			goto out;
		tmp = st->vco_out_freq[st->vco_out_map[channel]] / val;
		if((st->vco_out_freq[st->vco_out_map[channel]] / tmp - val) >
		   (val - st->vco_out_freq[st->vco_out_map[channel]] / (tmp + 1)))
		   tmp++;
		tmp = tmp < 1 ? 1 : tmp > 1024 ? 1024 : tmp;
		ret_val = st->vco_out_freq[st->vco_out_map[channel]] / tmp;
        reg &= ~(0x3FF << 8);
		reg |= AD9523_CLK_DIST_DIV(tmp);
		break;
	case CHAN_INFO_PHASE:
		code = val * 1000000 + val2 % 1000000;
		tmp = (code * AD9523_CLK_DIST_DIV_REV(ret)) / 3141592;
		tmp = tmp < 0 ? 0 : tmp > 63 ? 63 : tmp;
		reg &= ~AD9523_CLK_DIST_DIV_PHASE(~0);
		reg |= AD9523_CLK_DIST_DIV_PHASE(tmp);
		break;
	default:
		ret = -1;
		goto out;
	}

	ret = ad9523_write(AD9523_CHANNEL_CLOCK_DIST(channel), reg);
	if (ret < 0)
		goto out;

	ad9523_io_update();
out:

	return ret < 0 ? ret : ret_val;
}

/***************************************************************************//**
 * @brief Stores data into the EEPROM
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_store_eeprom()
{
	int32_t ret, tmp;

	ret = ad9523_write(AD9523_EEPROM_CTRL1,
			   AD9523_EEPROM_CTRL1_EEPROM_WRITE_PROT_DIS);
	if (ret < 0)
		return ret;
	ret = ad9523_write(AD9523_EEPROM_CTRL2,
			   AD9523_EEPROM_CTRL2_REG2EEPROM);
	if (ret < 0)
		return ret;

	tmp = 4;
	do {
		delay_us(16000);
		ret = ad9523_read(AD9523_EEPROM_DATA_XFER_STATUS);
		if (ret < 0)
			return ret;
	} while ((ret & AD9523_EEPROM_DATA_XFER_IN_PROGRESS) && tmp--);

	ret = ad9523_write(AD9523_EEPROM_CTRL1, 0);
	if (ret < 0)
		return ret;

	ret = ad9523_read(AD9523_EEPROM_ERROR_READBACK);
	if (ret < 0)
		return ret;

	if (ret & AD9523_EEPROM_ERROR_READBACK_FAIL) {
		//Verify EEPROM failed
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Updates the AD9523 configuration.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_sync()
{
	int32_t ret, tmp;

	ret = ad9523_read(AD9523_STATUS_SIGNALS);
	if (ret < 0)
		return ret;

	tmp = ret;
	tmp |= AD9523_STATUS_SIGNALS_SYNC_MAN_CTRL;

	ret = ad9523_write(AD9523_STATUS_SIGNALS, tmp);
	if (ret < 0)
		return ret;

	ad9523_io_update();
	tmp &= ~AD9523_STATUS_SIGNALS_SYNC_MAN_CTRL;

	ret = ad9523_write(AD9523_STATUS_SIGNALS, tmp);
	if (ret < 0)
		return ret;

	return ad9523_io_update();
}

/***************************************************************************//**
 * @brief Sets the output frequency for selected channel.
 *
 * @param channel - Selected channel number.
 * @param Hz - The output frequency for the selected channel. If the value is 
 *             INT64_MAX then the function just returns the already set value.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_frequency(int32_t channel, int64_t Hz)
{
    int32_t ret;
    int32_t ret_freq;

    if(Hz != INT64_MAX)
    {
        ret = ad9523_write_raw(channel, (int32_t)Hz, 0, CHAN_INFO_FREQUENCY);
        if(ret < 0)
            return ret;
        
        ret_freq = (int32_t)ret;
    }
    else
    {
        ret = ad9523_read_raw(channel, &ret_freq, 0, CHAN_INFO_FREQUENCY);
        if(ret < 0)
            return ret;
    }

    return (int64_t)ret_freq;
}

/***************************************************************************//**
 * @brief Sets the phase for selected channel.
 *
 * @param channel - Selected channel number.
 * @param rad - The phase for the selected channel. If the value is INT64_MAX 
 *              then the function just returns the already set value.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_phase(int32_t channel, int64_t rad)
{
    int32_t ret;
    int32_t ret_phase;

    if(rad != INT64_MAX)
    {
        ret = ad9523_write_raw(channel, (int32_t)rad, 0, CHAN_INFO_PHASE);
        if(ret < 0)
            return ret;

        ret_phase = (int32_t)rad;
    }
    else
    {
        ret = ad9523_read_raw(channel, &ret_phase, 0, CHAN_INFO_PHASE);
        if(ret < 0)
            return ret;
    }

    return (int64_t)ret_phase;
}

/***************************************************************************//**
 * @brief Sets the phase for selected channel.
 *
 * @param channel - Selected channel number.
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel. If the value is INT64_MAX then the function just 
 *               returns the already set value.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_raw(int32_t channel, int64_t raw_data)
{
    int32_t ret;
    int32_t ret_raw;

    if(raw_data != INT64_MAX)
    {
        ret = ad9523_write_raw(channel, (int32_t)raw_data, 0, CHAN_INFO_RAW);
        if(ret < 0)
            return ret;

        ret_raw = (int32_t)raw_data;
    }
    else
    {
        ret = ad9523_read_raw(channel, &ret_raw, 0, CHAN_INFO_RAW);
        if(ret < 0)
            return ret;
    }

    return (int64_t)ret_raw;
}

/***************************************************************************//**
 * @brief Helper function to get the status of the AD9523
 *
 * @param status_bit - The status bit to check
 *
 * @return Returns the state of the checked status bit.
*******************************************************************************/
int32_t ad9523_status(int32_t status_bit)
{
    int32_t ret;

    ret = ad9523_read(AD9523_READBACK_0);
    if (ret < 0)
        return ret;

    return ((ret & status_bit) != 0);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 0.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t AD9523_out_altvoltage0_ZD_OUTPUT_frequency(int64_t Hz)
{
    return ad9523_out_frequency(0, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 0.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage0_ZD_OUTPUT_phase(int64_t rad)
{
	return ad9523_out_phase(0, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 0.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage0_ZD_OUTPUT_raw(int64_t raw_data)
{
    return ad9523_out_raw(0, raw_data);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 1.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_altvoltage1_DAC_CLK_frequency(int64_t Hz)
{
    return ad9523_out_frequency(1, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 1.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage1_DAC_CLK_phase(int64_t rad)
{
	return ad9523_out_phase(1, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 1.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage1_DAC_CLK_raw(int64_t raw_data)
{
    return ad9523_out_raw(1, raw_data);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 2.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_altvoltage2_ADC_CLK_frequency(int64_t Hz)
{
    return ad9523_out_frequency(2, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 2.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage2_ADC_CLK_phase(int64_t rad)
{
	return ad9523_out_phase(2, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 2.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage2_ADC_CLK_raw(int64_t raw_data)
{
    return ad9523_out_raw(2, raw_data);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 4.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_altvoltage4_DAC_REF_CLK_frequency(int64_t Hz)
{
    return ad9523_out_frequency(4, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 4.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage4_DAC_REF_CLK_phase(int64_t rad)
{
	return ad9523_out_phase(4, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 4.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage4_DAC_REF_CLK_raw(int64_t raw_data)
{
    return ad9523_out_raw(4, raw_data);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 5.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_altvoltage5_TX_LO_REF_CLK_frequency(int64_t Hz)
{
    return ad9523_out_frequency(5, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 5.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage5_TX_LO_REF_CLK_phase(int64_t rad)
{
	return ad9523_out_phase(5, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 5.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage5_TX_LO_REF_CLK_raw(int64_t raw_data)
{
    return ad9523_out_raw(5, raw_data);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 6.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_altvoltage6_DAC_DCO_CLK_frequency(int64_t Hz)
{
    return ad9523_out_frequency(6, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 6.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage6_DAC_DCO_CLK_phase(int64_t rad)
{
	return ad9523_out_phase(6, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 6.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage6_DAC_DCO_CLK_raw(int64_t raw_data)
{
    return ad9523_out_raw(6, raw_data);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 8.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_altvoltage8_ADC_SYNC_CLK_frequency(int64_t Hz)
{
    return ad9523_out_frequency(8, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 8.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage8_ADC_SYNC_CLK_phase(int64_t rad)
{
	return ad9523_out_phase(8, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 8.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage8_ADC_SYNC_CLK_raw(int64_t raw_data)
{
    return ad9523_out_raw(8, raw_data);
}

/***************************************************************************//**
 * @brief Sets the output frequency for channel 9.
 *
 * @param Hz - The output frequency for the selected channel.
 *
 * @return Returns negative error code, or the actual frequency in Hz.
*******************************************************************************/
int64_t ad9523_out_altvoltage9_RX_LO_REF_CLK_frequency(int64_t Hz)
{
    return ad9523_out_frequency(9, Hz);
}

/***************************************************************************//**
 * @brief Sets the phase for channel 9.
 *
 * @param rad - The phase for the selected channel.
 *
 * @return Returns negative error code, or the actual phase in rad. 
*******************************************************************************/
int64_t ad9523_out_altvoltage9_RX_LO_REF_CLK_phase(int64_t rad)
{
	return ad9523_out_phase(9, rad);
}

/***************************************************************************//**
 * @brief Powers down / enables channel 9.
 *
 * @param data - '0' powers down channel 0, while any value > 0 enables the
 *				 channel.
 *
 * @return Returns the channel power status.
*******************************************************************************/
int64_t ad9523_out_altvoltage9_RX_LO_REF_CLK_raw(int64_t raw_data)
{
    return ad9523_out_raw(9, raw_data);
}

/***************************************************************************//**
 * @brief Returns the state of the VCXO clock.
*******************************************************************************/
int32_t ad9523_vcxo_clk_present()
{
    return ad9523_status(AD9523_READBACK_0_STAT_VCXO);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9523_reset(void)
{
	int32_t ret;

	ret = ad9523_write(AD9523_SERIAL_PORT_CONFIG,
			           AD9523_SER_CONF_SOFT_RESET |
			           (AD9523_SPI_3_WIRE ? 0 :
			           AD9523_SER_CONF_SDO_ACTIVE));

    return ret;
}

/***************************************************************************//**
 * @brief Initializes the AD9523.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad9523_setup()
{
	struct ad9523_state *st = &ad9523_st;
    struct ad9523_platform_data *pdata = &ad9523_pdata_lpc;
	struct ad9523_channel_spec *chan;
	uint32_t active_mask = 0;
	int32_t ret, i;

	ret = ad9523_reset();

	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_SERIAL_PORT_CONFIG, 0x00);
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_READBACK_CTRL,
			           AD9523_READBACK_CTRL_READ_BUFFERED);
	if (ret < 0)
		return ret;

	ret = ad9523_io_update();
	if (ret < 0)
		return ret;

	/*
	 * PLL1 Setup
	 */
	ret = ad9523_write(AD9523_PLL1_REF_A_DIVIDER,
		               pdata->refa_r_div);
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL1_REF_B_DIVIDER,
		               pdata->refb_r_div);
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL1_FEEDBACK_DIVIDER,
		               pdata->pll1_feedback_div);
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL1_CHARGE_PUMP_CTRL,
		               AD9523_PLL1_CHARGE_PUMP_CURRENT_nA(pdata->pll1_charge_pump_current_nA) |
		               AD9523_PLL1_CHARGE_PUMP_MODE_NORMAL |
		               AD9523_PLL1_BACKLASH_PW_MIN);
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL1_INPUT_RECEIVERS_CTRL,
		               AD_IF(refa_diff_rcv_en, AD9523_PLL1_REFA_RCV_EN) |
		               AD_IF(refb_diff_rcv_en, AD9523_PLL1_REFB_RCV_EN) |
		               AD_IF(osc_in_diff_en, AD9523_PLL1_OSC_IN_DIFF_EN) |
		               AD_IF(osc_in_cmos_neg_inp_en,
		               AD9523_PLL1_OSC_IN_CMOS_NEG_INP_EN) |
		               AD_IF(refa_diff_rcv_en, AD9523_PLL1_REFA_DIFF_RCV_EN) |
		               AD_IF(refb_diff_rcv_en, AD9523_PLL1_REFB_DIFF_RCV_EN));
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL1_REF_CTRL,
		               AD_IF(zd_in_diff_en, AD9523_PLL1_ZD_IN_DIFF_EN) |
		               AD_IF(zd_in_cmos_neg_inp_en,
		               AD9523_PLL1_ZD_IN_CMOS_NEG_INP_EN) |
		               AD_IF(zero_delay_mode_internal_en,
		               AD9523_PLL1_ZERO_DELAY_MODE_INT) |
		               AD_IF(osc_in_feedback_en, AD9523_PLL1_OSC_IN_PLL_FEEDBACK_EN) |
		               AD_IF(refa_cmos_neg_inp_en, AD9523_PLL1_REFA_CMOS_NEG_INP_EN) |
		               AD_IF(refb_cmos_neg_inp_en, AD9523_PLL1_REFB_CMOS_NEG_INP_EN));
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL1_MISC_CTRL,
		               AD9523_PLL1_REFB_INDEP_DIV_CTRL_EN |
		               AD9523_PLL1_REF_MODE(pdata->ref_mode));
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL1_LOOP_FILTER_CTRL,
		               AD9523_PLL1_LOOP_FILTER_RZERO(pdata->pll1_loop_filter_rzero));
	if (ret < 0)
		return ret;
	/*
	 * PLL2 Setup
	 */

	ret = ad9523_write(AD9523_PLL2_CHARGE_PUMP,
		               AD9523_PLL2_CHARGE_PUMP_CURRENT_nA(pdata->
			           pll2_charge_pump_current_nA));

	ret = ad9523_read(AD9523_PLL2_CHARGE_PUMP);

	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL2_FEEDBACK_DIVIDER_AB,
    		           AD9523_PLL2_FB_NDIV_A_CNT(pdata->pll2_ndiv_a_cnt) |
	    	           AD9523_PLL2_FB_NDIV_B_CNT(pdata->pll2_ndiv_b_cnt));
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL2_CTRL,
		               AD9523_PLL2_CHARGE_PUMP_MODE_NORMAL |
		               AD9523_PLL2_BACKLASH_CTRL_EN |
		               AD_IF(pll2_freq_doubler_en, AD9523_PLL2_FREQ_DOUBLER_EN));
	if (ret < 0)
		return ret;

	st->vco_freq = (pdata->vcxo_freq * (pdata->pll2_freq_doubler_en ? 2 : 1)
			        / pdata->pll2_r2_div) * AD9523_PLL2_FB_NDIV(pdata->
			        pll2_ndiv_a_cnt, pdata->pll2_ndiv_b_cnt);

	ret = ad9523_write(AD9523_PLL2_VCO_DIVIDER,
		               AD9523_PLL2_VCO_DIV_M1(pdata->pll2_vco_diff_m1) |
		               AD9523_PLL2_VCO_DIV_M2(pdata->pll2_vco_diff_m2) |
		               AD_IFE(pll2_vco_diff_m1, 0,
		                      AD9523_PLL2_VCO_DIV_M1_PWR_DOWN_EN) |
		               AD_IFE(pll2_vco_diff_m2, 0,
		                      AD9523_PLL2_VCO_DIV_M2_PWR_DOWN_EN));
	if (ret < 0)
		return ret;

	if (pdata->pll2_vco_diff_m1)
		st->vco_out_freq[AD9523_VCO1] =
			st->vco_freq / pdata->pll2_vco_diff_m1;

	if (pdata->pll2_vco_diff_m2)
		st->vco_out_freq[AD9523_VCO2] =
			st->vco_freq / pdata->pll2_vco_diff_m2;

	st->vco_out_freq[AD9523_VCXO] = pdata->vcxo_freq;

	ret = ad9523_write(AD9523_PLL2_R2_DIVIDER,
		               AD9523_PLL2_R2_DIVIDER_VAL(pdata->pll2_r2_div));
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL2_LOOP_FILTER_CTRL,
		               AD9523_PLL2_LOOP_FILTER_CPOLE1(pdata->cpole1) |
		               AD9523_PLL2_LOOP_FILTER_RZERO(pdata->rzero) |
		               AD9523_PLL2_LOOP_FILTER_RPOLE2(pdata->rpole2) |
		               AD_IF(rzero_bypass_en,
		                     AD9523_PLL2_LOOP_FILTER_RZERO_BYPASS_EN));
	if (ret < 0)
		return ret;

	for (i = 0; i < pdata->num_channels; i++) {
		chan = &pdata->channels[i];
		if (chan->channel_num < AD9523_NUM_CHAN) {
			active_mask |= (1 << chan->channel_num);
			ret = ad9523_write(AD9523_CHANNEL_CLOCK_DIST(chan->channel_num),
				               AD9523_CLK_DIST_DRIVER_MODE(chan->driver_mode) |
				               AD9523_CLK_DIST_DIV(chan->channel_divider) |
				               AD9523_CLK_DIST_DIV_PHASE(chan->divider_phase) |
				               (chan->sync_ignore_en ?
					               AD9523_CLK_DIST_IGNORE_SYNC_EN : 0) |
				               (chan->divider_output_invert_en ?
					               AD9523_CLK_DIST_INV_DIV_OUTPUT_EN : 0) |
				               (chan->low_power_mode_en ?
					               AD9523_CLK_DIST_LOW_PWR_MODE_EN : 0) |
				               (chan->output_dis ?
					               AD9523_CLK_DIST_PWR_DOWN_EN : 0));
			if (ret < 0)
				return ret;

			ret = ad9523_vco_out_map(chan->channel_num,
					                 chan->use_alt_clock_src);
			if (ret < 0)
				return ret;
		}
	}

	for(i = 0; i < AD9523_NUM_CHAN; i++)
    {
		if(!(active_mask & (1 << i)))
        {
            ad9523_write(AD9523_CHANNEL_CLOCK_DIST(i),
			             AD9523_CLK_DIST_DRIVER_MODE(TRISTATE) |
			             AD9523_CLK_DIST_PWR_DOWN_EN);
        }
    }

	ret = ad9523_write(AD9523_POWER_DOWN_CTRL, 0);
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_STATUS_SIGNALS,
			           AD9523_STATUS_MONITOR_01_PLL12_LOCKED);
	if (ret < 0)
		return ret;

	ret = ad9523_io_update();
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL2_VCO_CTRL,
					   AD9523_PLL2_VCO_CALIBRATE);
	if (ret < 0)
		return ret;

	ret = ad9523_io_update();
	if (ret < 0)
		return ret;

	ret = ad9523_write(AD9523_PLL2_VCO_CTRL, 0x00);
	if (ret < 0)
		return ret;

	ret = ad9523_io_update();
	if (ret < 0)
		return ret;

	return 0;
}
