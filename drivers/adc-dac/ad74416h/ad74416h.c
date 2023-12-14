/***************************************************************************//**
 *   @file   ad74416h.c
 *   @brief  Source file of AD74416H Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "ad74416h.h"
#include "no_os_crc8.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD74416H_CRC_POLYNOMIAL 	0x7
#define AD74416H_DIN_DEBOUNCE_LEN 	NO_OS_BIT(5)
#define AD77416H_DEV_ADDRESS_MSK	NO_OS_GENMASK(5, 4)

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
NO_OS_DECLARE_CRC8_TABLE(_crc_table);

static const unsigned int ad74416h_debounce_map[AD74416H_DIN_DEBOUNCE_LEN] = {
	0,     13,    18,    24,    32,    42,    56,    75,
	100,   130,   180,   240,   320,   420,   560,   750,
	1000,  1300,  1800,  2400,  3200,  4200,  5600,  7500,
	10000, 13000, 18000, 24000, 32000, 42000, 56000, 75000,
};

/** The ADC conversion rates */
static const uint32_t conv_rate_ad74416h[] = {
	10, 20, 1200, 4800, 9600, 19200, 200,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Converts a millivolt value in the corresponding DAC 16 bit code.
 * @param desc - The device structure.
 * @param mvolts - The millivolts value.
 * @param code - The resulting DAC code.
 * @param ch - The channel index.
 * @return 0 in case of success, -EINVAL otherwise
 */
int ad74416h_dac_voltage_to_code(struct ad74416h_desc *desc, int32_t mvolts,
				 uint16_t *code, uint32_t ch)
{
	uint32_t range, offset, res;

	switch(desc->id) {
	case ID_AD74414H:
		res = AD74414H_DAC_RESOLUTION;
		break;
	case ID_AD74416H:
		res = AD74416H_DAC_RESOLUTION;
		break;
	default:
		return -EINVAL;
	}

	switch (desc->channel_configs[ch].vout_range) {
	case AD74416H_VOUT_RANGE_0_12V:
		if (mvolts > AD74416H_DAC_RANGE || mvolts < 0)
			return -EINVAL;
		range = AD74416H_DAC_RANGE;
		offset = 0;
		break;
	case AD74416H_VOUT_RANGE_NEG12_12V:
		if (mvolts > AD74416H_DAC_RANGE || mvolts < -AD74416H_DAC_RANGE)
			return -EINVAL;
		range = AD74416H_DAC_RANGE * 2;
		offset = AD74416H_DAC_RANGE;
		break;
	default:
		return -EINVAL;
	}

	*code = (mvolts + offset) * NO_OS_BIT(res) / range;

	return 0;
}

/**
 * @brief Convers a microamp value in the corresponding DAC 16 bit code
 * @param desc - The device structure.
 * @param uamps - The microamps value
 * @param code - The resulting DAC code
 * @return 0 in case of success, -EINVAL otherwise
 */
int ad74416h_dac_current_to_code(struct ad74416h_desc *desc, uint32_t uamps,
				 uint16_t *code)
{
	uint32_t res;

	switch(desc->id) {
	case ID_AD74414H:
		res = AD74414H_DAC_RESOLUTION;
		break;
	case ID_AD74416H:
		res = AD74416H_DAC_RESOLUTION;
		break;
	default:
		return -EINVAL;
	}

	if (uamps > AD74416H_DAC_CURRENT_RANGE)
		return -EINVAL;
	*code = uamps * NO_OS_BIT(res) / AD74416H_DAC_CURRENT_RANGE;
	return 0;
}

/**
 * @brief Load the address and value in a communication buffer using
 * the format that the device expects.
 * @param reg - Register address.
 * @param val - Register value.
 * @param buff - The communication buffer.
 */
static void ad74416h_format_reg_write(uint8_t addr, uint8_t reg, uint16_t val,
				      uint8_t *buff)
{
	buff[0] = no_os_field_prep(AD77416H_DEV_ADDRESS_MSK, addr);
	buff[1] = reg;
	no_os_put_unaligned_be16(val, &buff[2]);
	buff[4] = no_os_crc8(_crc_table, buff, 4, 0);
}

/**
 * @brief Read a raw communication frame
 * @param desc - The device structure.
 * @param addr - The register's address.
 * @param val - A raw comm frame.
 * @return 0 in case of success, negative error otherwise.
 */
int ad74416h_reg_read_raw(struct ad74416h_desc *desc, uint32_t addr,
			  uint8_t *val)
{
	int ret;
	/**
	 * Reading a register on AD74416H requires writing the address to the READ_SELECT
	 * register first and then doing another spi read, which will contain the requested
	 * register value.
	 */
	ad74416h_format_reg_write(desc->dev_addr, AD74416H_READ_SELECT, addr,
				  desc->comm_buff);

	ret = no_os_spi_write_and_read(desc->spi_desc, desc->comm_buff,
				       AD74416H_FRAME_SIZE);
	if (ret)
		return ret;

	/* Make sure that NOP sequence is written for the second frame */
	ad74416h_format_reg_write(desc->dev_addr, AD74416H_NOP, AD74416H_NOP,
				  val);

	return no_os_spi_write_and_read(desc->spi_desc, val, AD74416H_FRAME_SIZE);
}

/**
 * @brief Write a register's value
 * @param desc  - The device structure.
 * @param addr - The register's address.
 * @param val - The register's value.
 * @return 0 in case of success, negative error otherwise
 */
int ad74416h_reg_write(struct ad74416h_desc *desc, uint32_t addr, uint16_t val)
{
	ad74416h_format_reg_write(desc->dev_addr, addr, val, desc->comm_buff);

	return no_os_spi_write_and_read(desc->spi_desc, desc->comm_buff,
					AD74416H_FRAME_SIZE);
}

/**
 * @brief Read a register's value
 * @param desc  - The device structure.
 * @param addr - The register's address.
 * @param val - The register's read value.
 * @return 0 in case of success, negative error otherwise
 */
int ad74416h_reg_read(struct ad74416h_desc *desc, uint32_t addr, uint16_t *val)
{
	int ret;
	uint8_t expected_crc;

	ret = ad74416h_reg_read_raw(desc, addr, desc->comm_buff);
	if (ret)
		return ret;

	expected_crc = no_os_crc8(_crc_table, desc->comm_buff, 4, 0);
	if (expected_crc != desc->comm_buff[4])
		return -EINVAL;

	*val = no_os_get_unaligned_be16(&desc->comm_buff[2]);

	return 0;
}

/**
 * @brief Update a register's field.
 * @param desc  - The device structure.
 * @param addr - The register's address.
 * @param val - The register's value.
 * @param mask - The mask for a specific register field.
 * @return 0 in case of success, negative error otherwise.
 */
int ad74416h_reg_update(struct ad74416h_desc *desc, uint32_t addr,
			uint16_t mask,
			uint16_t val)
{
	int ret;
	uint16_t data;

	ret = ad74416h_reg_read(desc, addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= no_os_field_prep(mask, val);

	return ad74416h_reg_write(desc, addr, data);
}

/**
 * @brief Get the number of active channels.
 * @param desc - The device structure.
 * @param nb_channels - The number of active channels
 * @return 0 in case of success, negative error otherwise.
 */
int ad74416h_nb_active_channels(struct ad74416h_desc *desc,
				uint8_t *nb_channels)
{
	int ret;
	uint16_t reg_val;

	ret = ad74416h_reg_read(desc, AD74416H_ADC_CONV_CTRL, &reg_val);
	if (ret)
		return ret;

	reg_val = no_os_field_get(NO_OS_GENMASK(3, 0), reg_val);
	*nb_channels = no_os_hweight8((uint8_t)reg_val);

	return 0;
}

/**
 * @brief Read the raw ADC raw conversion value.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC raw conversion value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_get_raw_adc_result(struct ad74416h_desc *desc, uint32_t ch,
				uint32_t *val)
{
	uint32_t val_msb, val_lsb;
	int ret;

	if (desc->id == ID_AD74416H) {
		ret = ad74416h_reg_read(desc, AD74416H_ADC_RESULT_UPR(ch),
					&val_msb);
		if (ret)
			return ret;
	}

	ret = ad74416h_reg_read(desc, AD74416H_ADC_RESULT(ch), &val_lsb);
	if (ret)
		return ret;

	if (desc->id == ID_AD74416H)
		*val = (no_os_field_get(AD74416H_CONV_RES_UPR_MSK, val_msb) << 16) |
		       no_os_field_get(AD74416H_CONV_RESULT_MSK, val_lsb);
	else
		*val = no_os_field_get(AD74416H_CONV_RESULT_MSK, val_lsb);

	return 0;
}

/**
 * @brief Enable/disable a specific ADC channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param status - Enabled or disabled status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_adc_channel_enable(struct ad74416h_desc *desc, uint32_t ch,
				    bool status)
{
	int ret;

	ret = ad74416h_reg_update(desc, AD74416H_ADC_CONV_CTRL,
				  AD74416H_CH_EN_MSK(ch), status);
	if (ret)
		return ret;

	desc->channel_configs[ch].enabled = status;

	return 0;
}

/**
 * @brief Enable conversions on a diagnostic register
 * @param desc - The device structure.
 * @param ch - Diagnostic channel index.
 * @param status - Enabled or disabled status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_diag_channel_enable(struct ad74416h_desc *desc, uint32_t ch,
				     bool status)
{
	return ad74416h_reg_update(desc, AD74416H_ADC_CONV_CTRL,
				   AD74416H_DIAG_EN_MSK(ch), status);
}

/**
 * @brief Get the ADC measurement range for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC range value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_get_adc_range(struct ad74416h_desc *desc, uint32_t ch,
			   uint16_t *val)
{
	int ret;

	ret = ad74416h_reg_read(desc, AD74416H_ADC_CONFIG(ch), val);
	if (ret)
		return ret;

	*val = no_os_field_get(AD74416H_ADC_CONV_RANGE_MSK, *val);

	return 0;
}

/**
 * @brief Set the ADC measurement range for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC range value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_adc_range(struct ad74416h_desc *desc, uint32_t ch,
			   enum ad74416h_adc_range val)
{
	if (desc->id == ID_AD74414H) {
		switch(val) {
		case AD74416H_RNG_NEG12_12_V:
		case AD74416H_RNG_0_0P625V:
		case AD74416H_RNG_NEG104_104MV:
		case AD74416H_RNG_NEG2P5_2P5V:
			return -EINVAL;
		default:
			break;
		}
	}

	return ad74416h_reg_update(desc, AD74416H_ADC_CONFIG(ch),
				   AD74416H_ADC_CONV_RANGE_MSK, val);
}

/**
 * @brief Get the ADC Conversion Rate for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC rejection setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_get_adc_rate(struct ad74416h_desc *desc, uint32_t ch,
			  enum ad74416h_adc_rate *val)
{
	int ret;
	uint16_t rate_val;

	ret = ad74416h_reg_read(desc, AD74416H_ADC_CONFIG(ch), &rate_val);
	if (ret)
		return ret;

	*val = no_os_field_get(AD74416H_ADC_CONV_RATE_MSK, rate_val);

	return 0;
}

/**
 * @brief Set the ADC Conversion Rate for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC rejection setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_adc_rate(struct ad74416h_desc *desc, uint32_t ch,
			  enum ad74416h_adc_rate val)
{
	return ad74416h_reg_update(desc, AD74416H_ADC_CONFIG(ch),
				   AD74416H_ADC_CONV_RATE_MSK, val);
}

/**
 * @brief Get the ADC Input Node for conversion.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC input node setting.
 */
int ad74416h_get_adc_conv_mux(struct ad74416h_desc *desc, uint32_t ch,
			      enum ad74416h_adc_conv_mux *val)
{
	int ret;
	uint16_t rate_val;

	ret = ad74416h_reg_read(desc, AD74416H_ADC_CONFIG(ch), &rate_val);
	if (ret)
		return ret;

	*val = no_os_field_get(AD74416H_CONV_MUX_MSK, rate_val);

	return 0;
}

/**
 * @brief Set the ADC Input Node for conversion.
 * @param desc - The device structure
 * @param ch - The channel index.
 * @param val - The ADC input node setting.
 */
int ad74416h_set_adc_conv_mux(struct ad74416h_desc *desc, uint32_t ch,
			      enum ad74416h_adc_conv_mux val)
{
	if (desc->id == ID_AD74414H) {
		switch(val) {
		case AD74416H_MUX_VSENSEN_TO_AGND:
		case AD74416H_MUX_LF_TO_VSENSEN:
			return -EINVAL;
		default:
			break;
		}
	}

	return ad74416h_reg_update(desc, AD74416H_ADC_CONFIG(ch),
				   AD74416H_CONV_MUX_MSK, val);
}

/**
 * @brief Start or stop ADC conversions.
 * @param desc - The device structure.
 * @param status - The ADC conversion sequence.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_adc_conv_seq(struct ad74416h_desc *desc,
			      enum ad74416h_conv_seq status)
{
	int ret;

	ret = ad74416h_reg_update(desc, AD74416H_ADC_CONV_CTRL,
				  AD74416H_CONV_SEQ_MSK, status);
	if (ret)
		return ret;
	/**
	 * If the ADC was powered down, wait for 100us before the ADC starts
	 * doing conversions.
	 */
	no_os_udelay(100);

	return 0;
}

/**
 * @brief Get a single ADC raw value for a specific channel, then power down the ADC.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC raw result.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_get_adc_single(struct ad74416h_desc *desc, uint32_t ch,
			    uint16_t *val)
{
	int ret;
	uint32_t delay;
	uint8_t nb_active_channels;
	enum ad74416h_adc_rate rate;

	ret = ad74416h_set_adc_channel_enable(desc, ch, true);
	if (ret)
		return ret;

	ret = ad74416h_nb_active_channels(desc, &nb_active_channels);
	if (ret)
		return ret;

	ret = ad74416h_set_adc_conv_seq(desc, AD74416H_START_SINGLE);
	if (ret)
		return ret;

	ret = ad74416h_get_adc_rate(desc, ch, &rate);
	if (ret)
		return ret;

	delay = AD74116H_CONV_TIME_US / conv_rate_ad74416h[rate];

	no_os_udelay(delay * nb_active_channels);

	ret = ad74416h_get_raw_adc_result(desc, ch, val);
	if (ret)
		return ret;

	ret = ad74416h_set_adc_conv_seq(desc, AD74416H_STOP_PWR_DOWN);
	if (ret)
		return ret;

	return ad74416h_set_adc_channel_enable(desc, ch, false);
}

/**
 * @brief Read the die's temperature from the diagnostic register.
 * @param desc - The device structure.
 * @param ch - The diagnostic channel on which the temperature reading
 * is assigned and enabled.
 * @param temp - The measured temperature (in degrees Celsius).
 * @return 0 in case of success, -EINVAL otherwise.
 */
int ad74416h_get_temp(struct ad74416h_desc *desc, uint32_t ch, uint16_t *temp)
{
	int ret;

	ret = ad74416h_get_diag(desc, ch, temp);
	if (ret)
		return ret;

	*temp = (*temp + AD74416H_TEMP_OFFSET) * AD74416H_TEMP_SCALE_DIV /
		AD74416H_TEMP_SCALE;

	return 0;
}

/**
 * @brief Set the operation mode for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param ch_func - The operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_channel_function(struct ad74416h_desc *desc,
				  uint32_t ch, enum ad74416h_op_mode ch_func)
{
	int ret;
	uint16_t dac_code;

	if (desc->id == ID_AD74414H) {
		switch(ch_func) {
		case AD74416H_VOLTAGE_OUT:
		case AD74416H_VOLTAGE_IN:
		case AD74416H_RESISTANCE:
			return -EINVAL;
		default:
			break;
		}
	}

	ret = ad74416h_reg_update(desc, AD74416H_CH_FUNC_SETUP(ch),
				  AD74416H_CH_FUNC_SETUP_MSK, AD74416H_HIGH_Z);
	if (ret)
		return ret;

	/* Get the DAC code corresponing to 0mV */
	ret = ad74416h_dac_voltage_to_code(desc, 0, &dac_code, ch);
	if(ret)
		return ret;

	ret = ad74416h_reg_update(desc, AD74416H_DAC_CODE(ch),
				  AD74416H_DAC_CODE_MSK, dac_code);
	if (ret)
		return ret;

	/* Datasheet delay required before transition to new desired mode */
	no_os_udelay(200);

	ret = ad74416h_reg_update(desc, AD74416H_CH_FUNC_SETUP(ch),
				  AD74416H_CH_FUNC_SETUP_MSK, ch_func);
	if (ret)
		return ret;

	desc->channel_configs[ch].function = ch_func;

	/* Datasheet delay required before updating the new DAC code */
	no_os_udelay(200);

	return 0;
}

/**
 * @brief Set the voltage range for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param vout_range - The voltage range.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_channel_vout_range(struct ad74416h_desc *desc, uint32_t ch,
				    enum ad74416h_vout_range vout_range)
{
	int ret;

	ret = ad74416h_reg_update(desc, AD74416H_OUTPUT_CONFIG(ch),
				  AD74416H_VOUT_RANGE_MSK, vout_range);
	if (ret)
		return ret;

	desc->channel_configs[ch].vout_range = vout_range;

	return 0;
}

/**
 * @brief Set the current limit for a specific DAC channel in vout mode
 * @param desc - The devices structure
 * @param ch - The channel index
 * @param i_limit - The current limit
 * @return 0 in case of success, negative error otherwise
 */
int ad74416h_set_channel_i_limit(struct ad74416h_desc *desc,
				 uint32_t ch, enum ad74416h_i_limit i_limit)
{
	int ret;
	ret = ad74416h_reg_update(desc, AD74416H_CH_FUNC_SETUP(ch),
				  AD74416H_I_LIMIT_MSK, i_limit);
	if (ret)
		return ret;

	desc->channel_configs[ch].i_limit = i_limit;

	return 0;
}

/**
 * @brief Set and load a code for the DAC on a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param dac_code - The code for the DAC.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_channel_dac_code(struct ad74416h_desc *desc, uint32_t ch,
				  uint16_t dac_code)
{
	return ad74416h_reg_write(desc, AD74416H_DAC_CODE(ch), dac_code);
}

/**
 * @brief Set which diagnostic value to be loaded in the DIAG_RESULT register
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param diag_code - The diagnostic setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_diag(struct ad74416h_desc *desc, uint32_t ch,
		      enum ad74416h_diag_mode diag_code)
{
	if (desc->id == ID_AD74414H) {
		switch(diag_code) {
		case AD74416H_DIAG_LVIN:
		case AD74416H_VSENSEN_C:
			return -EINVAL;
		default:
			break;
		}
	}

	return ad74416h_reg_update(desc, AD74416H_DIAG_ASSIGN,
				   AD74416H_DIAG_ASSIGN_MSK(ch), diag_code);
}

/**
 * @brief Get the diagnostic value for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param diag_code - The diagnostic setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_get_diag(struct ad74416h_desc *desc, uint32_t ch,
		      uint16_t *diag_code)
{
	int ret;

	ret = ad74416h_reg_read(desc, AD74416H_ADC_DIAG_RESULT(ch), diag_code);
	if (ret)
		return ret;

	*diag_code = no_os_field_get(AD74416H_DIAG_RESULT_MSK, *diag_code);

	return ret;
}

/**
 * @brief Set the debounce mode for the IOx inputs when the ADC is running in digital
 * input mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param mode - The debounce mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_debounce_mode(struct ad74416h_desc *desc, uint32_t ch,
			       enum ad74416h_debounce_mode mode)
{
	return ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch),
				   AD74416H_DEBOUNCE_MODE_MSK, mode);
}

/**
 * @brief Set the debounce settle time for the IOx inputs when the ADC is
 * running in digital input mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param time - The debounce time.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_debounce_time(struct ad74416h_desc *desc, uint32_t ch,
			       uint32_t time)
{
	uint32_t val = AD74416H_DIN_DEBOUNCE_LEN - 1;

	for (uint32_t i = 0; i < AD74416H_DIN_DEBOUNCE_LEN; i++)
		if (time <= ad74416h_debounce_map[i]) {
			val = i;
			break;
		}

	return ad74416h_reg_update(desc, AD74416H_DIN_CONFIG0(ch),
				   AD74416H_DEBOUNCE_TIME_MSK, val);
}

/**
 * @brief Get the GPO value for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The debounce time.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_gpio_get(struct ad74416h_desc *desc, uint32_t ch, uint8_t *val)
{
	int ret;
	uint16_t reg;

	ret = ad74416h_reg_read(desc, AD74416H_DIN_COMP_OUT, &reg);
	if (ret)
		return ret;

	*val = no_os_field_get(NO_OS_BIT(ch), reg);

	return 0;
}

/**
 * @brief Set the GPIO operation mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param config - The configuration setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_gpio_config(struct ad74416h_desc *desc, uint32_t ch,
			     enum ad74416h_gpio_select config)
{
	return ad74416h_reg_update(desc, AD74416H_GPIO_CONFIG(ch),
				   AD74416H_GPIO_SELECT_MSK, config);
}

/**
 * @brief Set the threshold, for which a signal would be considered high,
 * when the ADC is running in digital input mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param threshold - The threshold value (in millivolts).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_threshold(struct ad74416h_desc *desc, uint32_t ch,
			   uint32_t threshold)
{
	int ret;
	uint32_t dac_threshold;

	if (threshold > AD74416H_THRESHOLD_RANGE)
		return -EINVAL;

	/** Set a fixed range (0 - 16 V) for the threshold, so it would not depend on Vadd. */
	ret = ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch),
				  AD74416H_DIN_THRESH_MODE_MSK, 1);
	if (ret)
		return ret;

	dac_threshold = AD74416H_THRESHOLD_DAC_RANGE * threshold /
			AD74416H_THRESHOLD_RANGE;

	return ad74416h_reg_update(desc, AD74416H_DIN_CONFIG1(ch),
				   AD74416H_COMP_THRESH_MSK, dac_threshold);
}

/**
 * @brief Set the DO value of a channel
 * @param desc - The device structure
 * @param ch - The channel index
 * @param val - the output logic state. 0-low 1-high
 * @return 0 in case of success, negative error code otherwise
 */
int ad74416h_do_set(struct ad74416h_desc *desc, uint32_t ch, uint8_t val)
{
	return ad74416h_reg_update(desc, AD74416H_DO_EXT_CONFIG(ch),
				   AD74416H_DO_DATA_MSK, val);
}

/**
 * @brief Set the logic value of a GPO pin
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The output logic state.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_gpio_set(struct ad74416h_desc *desc, uint32_t ch, uint8_t val)
{
	int ret;

	ret = ad74416h_set_gpio_config(desc, ch, AD74416H_GPIO_CONFIG_DATA);
	if (ret)
		return ret;

	return ad74416h_reg_update(desc, AD74416H_GPIO_CONFIG(ch),
				   AD74416H_GPO_DATA_MSK, val);
}

/**
 * @brief Read the live status bits.
 * @param desc - The device structure.
 * @param status - The register's value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_get_live(struct ad74416h_desc *desc,
		      union ad74416h_live_status *status)
{
	return ad74416h_reg_read(desc, AD74416H_LIVE_STATUS, &status->value);
}

/**
 * @brief Configure and enable slew rate control for a DAC on a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param step - Number of codes per increment.
 * @param rate - Number of increments per second.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_dac_slew_enable(struct ad74416h_desc *desc, uint32_t ch,
			     enum ad74416h_slew_lin_step step,
			     enum ad74416h_lin_rate rate)
{
	int ret;

	ret = ad74416h_reg_update(desc, AD74416H_OUTPUT_CONFIG(ch),
				  AD74416H_SLEW_LIN_STEP_MSK, step);
	if (ret)
		return ret;

	ret = ad74416h_reg_update(desc, AD74416H_OUTPUT_CONFIG(ch),
				  AD74416H_SLEW_LIN_RATE_MSK, rate);
	if (ret)
		return ret;

	return ad74416h_reg_update(desc, AD74416H_OUTPUT_CONFIG(ch),
				   AD74416H_SLEW_EN_MSK, 1);
}

/**
 * @brief Disable the slew rate control.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_dac_slew_disable(struct ad74416h_desc *desc, uint32_t ch)
{
	return ad74416h_reg_update(desc, AD74416H_OUTPUT_CONFIG(ch),
				   AD74416H_SLEW_EN_MSK, 0);
}

/**
 * @brief Enable or disable the higher thermal reset.
 * @param desc - The device structure.
 * @param enable - The thermal reset status.
 * 			false: reset at 110 deg. Celsius.
 * 			true: reset at 140 deg. Celsius.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_set_therm_rst(struct ad74416h_desc *desc, bool enable)
{
	return ad74416h_reg_write(desc, AD74416H_THERM_RST, enable);
}

/**
 * @brief Perform software or hardware reset and wait for device reset time.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_reset(struct ad74416h_desc *desc)
{
	int ret;

	if (desc->reset_gpio) {
		ret = no_os_gpio_direction_output(desc->reset_gpio,
						  NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		/* Minimum RESET signal pulse duration */
		no_os_udelay(50);
		ret = no_os_gpio_set_value(desc->reset_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	} else {
		ret = ad74416h_reg_write(desc, AD74416H_CMD_KEY, AD74416H_CMD_KEY_RESET_1);
		if (ret)
			return ret;

		ret = ad74416h_reg_write(desc, AD74416H_CMD_KEY, AD74416H_CMD_KEY_RESET_2);
		if (ret)
			return ret;
	}
	/* Time taken for device reset (datasheet value = 1ms) */
	no_os_mdelay(1);

	return 0;
}

/**
 * @brief Comm test function
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad74416h_scratch_test(struct ad74416h_desc *desc)
{
	int ret;
	uint16_t val;
	uint16_t test_val = 0x1234;

	ret = ad74416h_reg_write(desc, AD74416H_SCRATCH(0), test_val);
	if (ret)
		return ret;

	ret = ad74416h_reg_read(desc, AD74416H_SCRATCH(0), &val);
	if (ret)
		return ret;

	if (val != test_val)
		return -EINVAL;

	return 0;
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74416h_init(struct ad74416h_desc **desc,
		  struct ad74416h_init_param *init_param)
{
	int ret;
	struct ad74416h_desc *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->spi_desc, &init_param->spi_ip);
	if (ret)
		goto err;

	descriptor->id = init_param->id;
	descriptor->dev_addr = init_param->dev_addr;

	no_os_crc8_populate_msb(_crc_table, AD74416H_CRC_POLYNOMIAL);

	ret = no_os_gpio_get_optional(&descriptor->reset_gpio,
				      init_param->reset_gpio_param);
	if (ret)
		goto comm_err;

	ret = ad74416h_reset(descriptor);
	if (ret)
		goto gpio_err;

	ret = ad74416h_scratch_test(descriptor);
	if (ret)
		goto gpio_err;

	*desc = descriptor;

	return 0;

gpio_err:
	no_os_gpio_remove(descriptor->reset_gpio);
comm_err:
	no_os_spi_remove(descriptor->spi_desc);
err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int ad74416h_remove(struct ad74416h_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	if (desc->reset_gpio) {
		ret = no_os_gpio_remove(desc->reset_gpio);
		if (ret)
			return ret;

		desc->reset_gpio = NULL;
	}

	ret = no_os_spi_remove(desc->spi_desc);
	if (ret)
		return ret;

	desc->spi_desc = NULL;

	no_os_free(desc);

	return 0;
}
