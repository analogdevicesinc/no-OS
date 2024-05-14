/***************************************************************************//**
 *   @file   ad74413r.c
 *   @brief  Source file of AD74413r Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "ad74413r.h"
#include "no_os_crc8.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD74413R_FRAME_SIZE 		4
#define AD74413R_CRC_POLYNOMIAL 	0x7
#define AD74413R_DIN_DEBOUNCE_LEN 	NO_OS_BIT(5)

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
NO_OS_DECLARE_CRC8_TABLE(_crc_table);

static const unsigned int ad74413r_debounce_map[AD74413R_DIN_DEBOUNCE_LEN] = {
	0,     13,    18,    24,    32,    42,    56,    75,
	100,   130,   180,   240,   320,   420,   560,   750,
	1000,  1300,  1800,  2400,  3200,  4200,  5600,  7500,
	10000, 13000, 18000, 24000, 32000, 42000, 56000, 75000,
};

/** The time required for an ADC conversion by rejection (us) */
static const uint32_t conv_times_ad74413r[] = { 50000, 208, 100000, 833 };
static const uint32_t conv_times_ad74412r[] = { 50000, 208};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Set the rejection value based on the selected sample rate
 * @param rate - ADC sample rate
 * @param rejection - Rejection register value
 * @return 0 in case of success, -EINVAL otherwise
 */
static int ad74413r_rate_to_rejection(enum ad74413r_adc_sample rate,
				      enum ad74413r_rejection *rejection)
{
	switch (rate) {
	case AD74413R_ADC_SAMPLE_20HZ:
		*rejection = AD74413R_REJECTION_50_60;
		break;
	case AD74413R_ADC_SAMPLE_4800HZ:
		*rejection = AD74413R_REJECTION_NONE;
		break;
	case AD74413R_ADC_SAMPLE_10HZ:
		*rejection = AD74413R_REJECTION_50_60_HART;
		break;
	case AD74413R_ADC_SAMPLE_1200HZ:
		*rejection = AD74413R_REJECTION_HART;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set the sample rate based on the selected rejection value
 * @param rate - ADC sample rate
 * @param rejection - Rejection register value
 * @return 0 in case of success, -EINVAL otherwise
 */
static int ad74413r_rejection_to_rate(enum ad74413r_rejection rejection,
				      enum ad74413r_adc_sample *rate)
{
	switch (rejection) {
	case AD74413R_REJECTION_50_60:
		*rate = AD74413R_ADC_SAMPLE_20HZ;
		break;
	case AD74413R_REJECTION_NONE:
		*rate = AD74413R_ADC_SAMPLE_4800HZ;
		break;
	case AD74413R_REJECTION_50_60_HART:
		*rate = AD74413R_ADC_SAMPLE_10HZ;
		break;
	case AD74413R_REJECTION_HART:
		*rate = AD74413R_ADC_SAMPLE_1200HZ;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Convert the measuring range of the ADC from range enum values to millivolts.
 * @param range - ADC sample rate
 * @param val - Rejection register value
 * @return 0 in case of success, -EINVAL otherwise
 */
int ad74413r_range_to_voltage_range(enum ad74413r_adc_range range,
				    uint32_t *val)
{
	switch (range) {
	case AD74413R_ADC_RANGE_10V:
		*val = 10000;
		break;
	case AD74413R_ADC_RANGE_2P5V_EXT_POW:
	case AD74413R_ADC_RANGE_2P5V_INT_POW:
		*val = 2500;
		break;
	case AD74413R_ADC_RANGE_5V_BI_DIR:
		*val = 5000;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Convert the measuring range of the ADC from range enum values to millivolts.
 * @param range - ADC sample rate
 * @param val - Rejection register value
 * @return 0 in case of success, -EINVAL otherwise
 */
int ad74413r_range_to_voltage_offset(enum ad74413r_adc_range range,
				     int32_t *val)
{
	switch (range) {
	case AD74413R_ADC_RANGE_10V:
	case AD74413R_ADC_RANGE_2P5V_EXT_POW:
	case AD74413R_ADC_RANGE_2P5V_INT_POW:
		*val = 0;
		break;
	case AD74413R_ADC_RANGE_5V_BI_DIR:
		*val = (-(int)AD74413R_ADC_MAX_VALUE / 2);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Converts a millivolt value in the corresponding DAC 13 bit code.
 * @param mvolts - The millivolts value.
 * @param code - The resulting DAC code.
 * @return 0 in case of success, -EINVAL otherwise
 */
int ad74413r_dac_voltage_to_code(uint32_t mvolts, uint32_t *code)
{
	if (mvolts > AD74413R_DAC_RANGE)
		return -EINVAL;

	*code = mvolts * NO_OS_BIT(AD74413R_DAC_RESOLUTION) / AD74413R_DAC_RANGE;

	return 0;
}

/**
 * @brief Load the address and value in a communication buffer using
 * the format that the device expects.
 * @param reg - Register address.
 * @param val - Register value.
 * @param buff - The communication buffer.
 */
static void ad74413r_format_reg_write(uint8_t reg, uint16_t val, uint8_t *buff)
{
	buff[0] = reg;
	no_os_put_unaligned_be16(val, &buff[1]);
	buff[3] = no_os_crc8(_crc_table, buff, 3, 0);
}

/**
 * @brief Read a raw communication frame
 * @param desc - The device structure.
 * @param addr - The register's address.
 * @param val - A raw comm frame.
 * @return 0 in case of success, negative error otherwise.
 */
int ad74413r_reg_read_raw(struct ad74413r_desc *desc, uint32_t addr,
			  uint8_t *val)
{
	int ret;
	/**
	 * Reading a register on AD74413r requires writing the address to the READ_SELECT
	 * register first and then doing another spi read, which will contain the requested
	 * register value.
	 */
	ad74413r_format_reg_write(AD74413R_READ_SELECT, addr, desc->comm_buff);

	ret = no_os_spi_write_and_read(desc->comm_desc, desc->comm_buff,
				       AD74413R_FRAME_SIZE);
	if (ret)
		return ret;

	/* Make sure that NOP sequence is written for the second frame */
	ad74413r_format_reg_write(AD74413R_NOP, AD74413R_NOP, val);

	return no_os_spi_write_and_read(desc->comm_desc, val, AD74413R_FRAME_SIZE);
}

/**
 * @brief Write a register's value
 * @param desc  - The device structure.
 * @param addr - The register's address.
 * @param val - The register's value.
 * @return 0 in case of success, negative error otherwise
 */
int ad74413r_reg_write(struct ad74413r_desc *desc, uint32_t addr, uint16_t val)
{
	ad74413r_format_reg_write(addr, val, desc->comm_buff);

	return no_os_spi_write_and_read(desc->comm_desc, desc->comm_buff,
					AD74413R_FRAME_SIZE);
}

/**
 * @brief Read a register's value
 * @param desc  - The device structure.
 * @param addr - The register's address.
 * @param val - The register's read value.
 * @return 0 in case of success, negative error otherwise
 */
int ad74413r_reg_read(struct ad74413r_desc *desc, uint32_t addr, uint16_t *val)
{
	int ret;
	uint8_t expected_crc;

	ret = ad74413r_reg_read_raw(desc, addr, desc->comm_buff);
	if (ret)
		return ret;

	expected_crc = no_os_crc8(_crc_table, desc->comm_buff, 3, 0);
	if (expected_crc != desc->comm_buff[3])
		return -EINVAL;

	*val = no_os_get_unaligned_be16(&desc->comm_buff[1]);

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
int ad74413r_reg_update(struct ad74413r_desc *desc, uint32_t addr,
			uint16_t mask,
			uint16_t val)
{
	int ret;
	uint16_t c_val;

	ret = ad74413r_reg_read(desc, addr, &c_val);
	if (ret)
		return ret;

	c_val &= ~mask;
	c_val |= no_os_field_prep(mask, val);

	return ad74413r_reg_write(desc, addr, c_val);
}

/**
 * @brief Get the number of active channels.
 * @param desc - The device structure.
 * @param nb_channels - The number of active channels
 * @return 0 in case of success, negative error otherwise.
 */
int ad74413r_nb_active_channels(struct ad74413r_desc *desc,
				uint8_t *nb_channels)
{
	int ret;
	uint16_t reg_val;

	ret = ad74413r_reg_read(desc, AD74413R_ADC_CONV_CTRL, &reg_val);
	if (ret)
		return ret;

	reg_val = no_os_field_get(NO_OS_GENMASK(7, 0), reg_val);
	*nb_channels = no_os_hweight8((uint8_t)reg_val);

	return 0;
}

/**
 * @brief Clear the ALERT_STATUS register.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error otherwise.
 */
int ad74413r_clear_errors(struct ad74413r_desc *desc)
{
	return ad74413r_reg_write(desc, AD74413R_ALERT_STATUS, AD74413R_ERR_CLR_MASK);
}

/**
 * @brief Select which information the device will respond with (in the readback field)
 * when a read operation is performed.
 * @param desc - The device structure.
 * @param mode - Possible values:
 * 			0 - Respond with the readback address.
 * 			1 - Respond with status bits.
 * @return 0 in case of success, negative error otherwise.
 */
int ad74413r_set_info(struct ad74413r_desc *desc, uint16_t mode)
{
	return ad74413r_reg_update(desc, AD74413R_READ_SELECT,
				   AD74413R_SPI_RD_RET_INFO_MASK, mode);
}

/**
 * @brief Comm test function
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad74413r_scratch_test(struct ad74413r_desc *desc)
{
	int ret;
	uint16_t val;
	uint16_t test_val = 0x1234;

	ret = ad74413r_reg_write(desc, AD74413R_SCRATCH, test_val);
	if (ret)
		return ret;

	ret = ad74413r_reg_read(desc, AD74413R_SCRATCH, &val);
	if (ret)
		return ret;

	if (val != test_val)
		return -EINVAL;

	return 0;
}

/**
 * @brief Perform either a software or hardware reset and wait for device reset time.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_reset(struct ad74413r_desc *desc)
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
		ret = ad74413r_reg_write(desc, AD74413R_CMD_KEY, AD74413R_CMD_KEY_RESET_1);
		if (ret)
			return ret;

		ret = ad74413r_reg_write(desc, AD74413R_CMD_KEY, AD74413R_CMD_KEY_RESET_2);
		if (ret)
			return ret;
	}

	/* Time taken for device reset (datasheet value = 1ms) */
	no_os_mdelay(1);

	return 0;
}

/**
 * @brief Set the operation mode for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param ch_func - The operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_channel_function(struct ad74413r_desc *desc,
				  uint32_t ch, enum ad74413r_op_mode ch_func)
{
	int ret;

	ret = ad74413r_set_channel_dac_code(desc, ch, 0);
	if (ret)
		return ret;

	ret = ad74413r_reg_update(desc, AD74413R_CH_FUNC_SETUP(ch),
				  AD74413R_CH_FUNC_SETUP_MASK, AD74413R_HIGH_Z);
	if (ret)
		return ret;

	/* Each function should be selected for at least 130 us. */
	no_os_udelay(130);
	ret = ad74413r_reg_update(desc, AD74413R_CH_FUNC_SETUP(ch),
				  AD74413R_CH_FUNC_SETUP_MASK, ch_func);
	if (ret)
		return ret;

	ret = ad74413r_reg_update(desc, AD74413R_ADC_CONFIG(ch),
				  AD74413R_CH_200K_TO_GND_MASK, 1);
	if (ret)
		return ret;

	/* No writes to the DACx registers may be done for 150 us after changing function */
	no_os_udelay(150);

	desc->channel_configs[ch].function = ch_func;

	return 0;
}

/**
 * @brief Read the raw ADC raw conversion value.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC raw conversion value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_raw_adc_result(struct ad74413r_desc *desc, uint32_t ch,
				uint16_t *val)
{
	return ad74413r_reg_read(desc, AD74413R_ADC_RESULT(ch), val);
}

/**
 * @brief Enable/disable a specific ADC channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param status - Enabled or disabled status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_adc_channel_enable(struct ad74413r_desc *desc, uint32_t ch,
				    bool status)
{
	int ret;

	ret = ad74413r_reg_update(desc, AD74413R_ADC_CONV_CTRL,
				  AD74413R_CH_EN_MASK(ch), status);
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
int ad74413r_set_diag_channel_enable(struct ad74413r_desc *desc, uint32_t ch,
				     bool status)
{
	return ad74413r_reg_update(desc, AD74413R_ADC_CONV_CTRL,
				   AD74413R_DIAG_EN_MASK(ch), status);
}

/**
 * @brief Get the ADC measurement range for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC range value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_adc_range(struct ad74413r_desc *desc, uint32_t ch,
			   uint16_t *val)
{
	int ret;

	ret = ad74413r_reg_read(desc, AD74413R_ADC_CONFIG(ch), val);
	if (ret)
		return ret;

	*val = no_os_field_get(AD74413R_ADC_RANGE_MASK, *val);

	return 0;
}

/**
 * @brief Get the rejection setting for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC rejection setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_adc_rejection(struct ad74413r_desc *desc, uint32_t ch,
			       enum ad74413r_rejection *val)
{
	int ret;
	uint16_t rejection_val;

	ret = ad74413r_reg_read(desc, AD74413R_ADC_CONFIG(ch), &rejection_val);
	if (ret)
		return ret;

	*val = no_os_field_get(AD74413R_ADC_REJECTION_MASK, rejection_val);

	return 0;
}

/**
 * @brief Get the rejection setting for a specific channel.
 * @param desc - The device structure.
 * @param val - The ADC rejection setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_adc_diag_rejection(struct ad74413r_desc *desc,
				    enum ad74413r_rejection *val)
{
	int ret;
	uint16_t reg_val;

	ret = ad74413r_reg_read(desc, AD74413R_ADC_CONV_CTRL, &reg_val);
	if (ret)
		return ret;

	reg_val = no_os_field_get(AD74413R_EN_REJ_DIAG_MASK, reg_val);
	if (reg_val)
		*val = AD74413R_REJECTION_50_60;
	else
		*val = AD74413R_REJECTION_NONE;

	return 0;
}

/**
 * @brief Set the rejection setting for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC rejection setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_adc_rejection(struct ad74413r_desc *desc, uint32_t ch,
			       enum ad74413r_rejection val)
{
	return ad74413r_reg_update(desc, AD74413R_ADC_CONFIG(ch),
				   AD74413R_ADC_REJECTION_MASK, val);
}

/**
 * @brief Get the ADC sample rate.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC sample rate value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_adc_rate(struct ad74413r_desc *desc, uint32_t ch,
			  enum ad74413r_adc_sample *val)
{
	int ret;
	enum ad74413r_rejection rejection;

	ret = ad74413r_get_adc_rejection(desc, ch, &rejection);
	if (ret)
		return ret;

	return ad74413r_rejection_to_rate(rejection, val);
}

/**
 * @brief Set the ADC sample rate.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC sample rate value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_adc_rate(struct ad74413r_desc *desc, uint32_t ch,
			  enum ad74413r_adc_sample val)
{
	int ret;
	enum ad74413r_rejection rejection;

	ret = ad74413r_rate_to_rejection(val, &rejection);
	if (ret)
		return ret;

	return ad74413r_set_adc_rejection(desc, ch, rejection);
}

/**
 * @brief Get the ADC sample rate for the diagnostics channels.
 * @param desc - The device structure.
 * @param ch - The diagnostics channel index.
 * @param val - The ADC sample rate value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_adc_diag_rate(struct ad74413r_desc *desc, uint32_t ch,
			       enum ad74413r_adc_sample *val)
{
	enum ad74413r_rejection rejection;
	int ret;

	ret = ad74413r_get_adc_diag_rejection(desc, &rejection);
	if (ret)
		return ret;

	return ad74413r_rejection_to_rate(rejection, val);
}

/**
 * @brief Set the ADC sample rate for the diagnostics channels.
 * @param desc - The device structure.
 * @param ch - The diagnostics channel index.
 * @param val - The ADC sample rate value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_adc_diag_rate(struct ad74413r_desc *desc, uint32_t ch,
			       enum ad74413r_adc_sample val)
{
	uint16_t reg_val;

	switch (val) {
	case AD74413R_ADC_SAMPLE_20HZ:
		reg_val = 1;
		break;
	case AD74413R_ADC_SAMPLE_4800HZ:
		reg_val = 0;
		break;
	default:
		return -EINVAL;
	}

	return ad74413r_reg_update(desc, AD74413R_ADC_CONV_CTRL,
				   AD74413R_EN_REJ_DIAG_MASK, reg_val);
}

/**
 * @brief Start or stop ADC conversions.
 * @param desc - The device structure.
 * @param status - The ADC conversion sequence.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_adc_conv_seq(struct ad74413r_desc *desc,
			      enum ad74413r_conv_seq status)
{
	int ret;

	ret = ad74413r_reg_update(desc, AD74413R_ADC_CONV_CTRL,
				  AD74413R_CONV_SEQ_MASK, status);
	if (ret)
		return ret;
	/**
	 * The write to CONV_SEQ powers up the ADC. If the ADC was powered down, the user must wait
	 * for 100us before the ADC starts doing conversions.
	 */
	no_os_udelay(100);

	return 0;
}

/**
 * @brief Get a single ADC raw value for a specific channel, then power down the ADC.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC raw result.
 * @param is_diag - Select which channel type does the index refer to (I/O or diagnostics).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_adc_single(struct ad74413r_desc *desc, uint32_t ch,
			    uint16_t *val, bool is_diag)
{
	int ret;
	uint32_t delay;
	uint8_t nb_active_channels;
	enum ad74413r_rejection rejection;

	if (is_diag)
		ret = ad74413r_set_diag_channel_enable(desc, ch, true);
	else
		ret = ad74413r_set_adc_channel_enable(desc, ch, true);
	if (ret)
		return ret;

	ret = ad74413r_nb_active_channels(desc, &nb_active_channels);
	if (ret)
		return ret;

	ret = ad74413r_set_adc_conv_seq(desc, AD74413R_START_SINGLE);
	if (ret)
		return ret;

	if (is_diag)
		ret = ad74413r_get_adc_diag_rejection(desc, &rejection);
	else
		ret = ad74413r_get_adc_rejection(desc, ch, &rejection);
	if (ret)
		return ret;

	if (desc->chip_id == AD74413R)
		delay = conv_times_ad74413r[rejection];
	else
		delay = conv_times_ad74412r[rejection];

	/** Wait for all channels to complete the conversion. */
	if (delay < 1000)
		no_os_udelay(delay * nb_active_channels);
	else
		no_os_mdelay((delay * nb_active_channels) / 1000);

	if (is_diag)
		ret = ad74413r_get_diag(desc, ch, val);
	else
		ret = ad74413r_get_raw_adc_result(desc, ch, val);
	if (ret)
		return ret;

	ret = ad74413r_set_adc_conv_seq(desc, AD74413R_STOP_PWR_DOWN);
	if (ret)
		return ret;

	if (is_diag)
		return ad74413r_set_diag_channel_enable(desc, ch, false);

	return ad74413r_set_adc_channel_enable(desc, ch, false);
}

/**
 * @brief Get the ADC real value, according to the operation mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The ADC real measurement value (the unit depends on
 * the operation mode).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_adc_get_value(struct ad74413r_desc *desc, uint32_t ch,
			   struct ad74413r_decimal *val)
{
	int ret;
	uint16_t adc_code;

	ret = ad74413r_get_adc_single(desc, ch, &adc_code, false);
	if (ret)
		return ret;

	switch (desc->channel_configs[ch].function) {
	case AD74413R_HIGH_Z:
		val->integer = no_os_div_u64_rem(adc_code * AD74413R_RANGE_10V_SCALE,
						 AD74413R_RANGE_10V_SCALE_DIV,
						 (uint32_t *)&val->decimal);
		break;
	case AD74413R_VOLTAGE_OUT:
		/**
		 * I_Rsense = (Vmin + (ADC_CODE/65535) * range) / Rsense
		 */
		val->integer = no_os_div_s64_rem((adc_code + AD74413R_RANGE_5V_OFFSET) *
						 AD74413R_RANGE_5V_SCALE,
						 AD74413R_RSENSE * AD74413R_RANGE_5V_SCALE_DIV,
						 &val->decimal);
		break;
	case AD74413R_CURRENT_OUT:
	case AD74413R_VOLTAGE_IN:
		val->integer = no_os_div_u64_rem(adc_code * AD74413R_RANGE_10V_SCALE,
						 AD74413R_RANGE_10V_SCALE_DIV,
						 (uint32_t *)&val->decimal);
		break;
	case AD74413R_CURRENT_IN_EXT_HART:
		if (desc->chip_id == AD74412R)
			return -ENOTSUP;
	/* fallthrough */
	case AD74413R_CURRENT_IN_LOOP_HART:
		if (desc->chip_id == AD74412R)
			return -ENOTSUP;
	/* fallthrough */
	case AD74413R_CURRENT_IN_EXT:
	case AD74413R_CURRENT_IN_LOOP:
		val->integer = no_os_div_u64_rem(adc_code * AD74413R_RANGE_2V5_SCALE,
						 AD74413R_RANGE_2V5_SCALE_DIV * AD74413R_RSENSE,
						 (uint32_t *)&val->decimal);
		break;
	case AD74413R_RESISTANCE:
		val->integer = no_os_div_u64_rem(adc_code * AD74413R_RTD_PULL_UP,
						 AD74413R_ADC_MAX_VALUE - adc_code,
						 (uint32_t *)&val->decimal);
		break;
	case AD74413R_DIGITAL_INPUT:
	case AD74413R_DIGITAL_INPUT_LOOP:
		val->integer = no_os_div_u64_rem(adc_code * AD74413R_RANGE_10V_SCALE,
						 AD74413R_RANGE_10V_SCALE_DIV,
						 (uint32_t *)&val->decimal);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Read the die's temperature from the diagnostic register.
 * @param desc - The device structure.
 * @param ch - The diagnostic channel on which the temperature reading
 * is assigned and enabled.
 * @param temp - The measured temperature (in degrees Celsius).
 * @return 0 in case of success, -EINVAL otherwise.
 */
int ad74413r_get_temp(struct ad74413r_desc *desc, uint32_t ch, uint16_t *temp)
{
	int ret;

	ret = ad74413r_get_diag(desc, ch, temp);
	if (ret)
		return ret;

	*temp = (*temp + AD74413R_TEMP_OFFSET) * AD74413R_TEMP_SCALE_DIV /
		AD74413R_TEMP_SCALE;

	return 0;
}

/**
 * @brief Set and load a code for the DAC on a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param dac_code - The code for the DAC.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_channel_dac_code(struct ad74413r_desc *desc, uint32_t ch,
				  uint16_t dac_code)
{
	int ret;

	ret = ad74413r_reg_write(desc, AD74413R_DAC_CODE(ch), dac_code);
	if (ret)
		return ret;

	return ad74413r_reg_write(desc, AD74413R_CMD_KEY, AD74413R_CMD_KEY_LDAC);
}

/**
 * @brief Set which diagnostic value to be loaded in the DIAG_RESULT register
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param diag_code - The diagnostic setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_diag(struct ad74413r_desc *desc, uint32_t ch,
		      enum ad74413r_diag_mode diag_code)
{
	return ad74413r_reg_update(desc, AD74413R_DIAG_ASSIGN,
				   AD74413R_DIAG_ASSIGN_MASK(ch), diag_code);
}

/**
 * @brief Get the diagnostic value for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param diag_code - The diagnostic setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_diag(struct ad74413r_desc *desc, uint32_t ch,
		      uint16_t *diag_code)
{
	int ret;

	ret = ad74413r_reg_read(desc, AD74413R_DIAG_RESULT(ch), diag_code);
	if (ret)
		return ret;

	*diag_code = no_os_field_get(AD74413R_DIAG_RESULT_MASK, *diag_code);

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
int ad74413r_set_debounce_mode(struct ad74413r_desc *desc, uint32_t ch,
			       enum ad74413r_debounce_mode mode)
{
	return ad74413r_reg_update(desc, AD74413R_DIN_CONFIG(ch),
				   AD74413R_DEBOUNCE_MODE_MASK, mode);
}

/**
 * @brief Set the debounce settle time for the IOx inputs when the ADC is
 * running in digital input mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param time - The debounce time.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_debounce_time(struct ad74413r_desc *desc, uint32_t ch,
			       uint32_t time)
{
	uint32_t val = AD74413R_DIN_DEBOUNCE_LEN - 1;

	for (uint32_t i = 0; i < AD74413R_DIN_DEBOUNCE_LEN; i++)
		if (time <= ad74413r_debounce_map[i]) {
			val = i;
			break;
		}

	return ad74413r_reg_update(desc, AD74413R_DIN_CONFIG(ch),
				   AD74413R_DEBOUNCE_TIME_MASK, val);
}

/**
 * @brief Get the GPO value for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The debounce time.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_gpo_get(struct ad74413r_desc *desc, uint32_t ch, uint8_t *val)
{
	int ret;
	uint16_t reg;

	ret = ad74413r_reg_read(desc, AD74413R_DIN_COMP_OUT, &reg);
	if (ret)
		return ret;

	*val = no_os_field_get(NO_OS_BIT(ch), reg);

	return 0;
}

/**
 * @brief Set the GPO operation mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param config - The configuration setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_gpo_config(struct ad74413r_desc *desc, uint32_t ch,
			    enum ad74413r_gpo_select config)
{
	return ad74413r_reg_update(desc, AD74413R_GPO_CONFIG(ch),
				   AD74413R_GPO_SELECT_MASK, config);
}

/**
 * @brief Set the threshold, for which a signal would be considered high,
 * when the ADC is running in digital input mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param threshold - The threshold value (in millivolts). The actual threshold
 * set might not match this value (~500mV max. error), since it's fairly
 * low resolution (29 possible values).
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_threshold(struct ad74413r_desc *desc, uint32_t ch,
			   uint32_t threshold)
{
	int ret;
	uint32_t dac_threshold;

	if (threshold > AD74413R_THRESHOLD_RANGE)
		return -EINVAL;

	/** Set a fixed range (0 - 16 V) for the threshold, so it would not depend on Vadd. */
	ret = ad74413r_reg_update(desc, AD74413R_DIN_THRESH,
				  AD74413R_DIN_THRESH_MODE_MASK, 1);
	if (ret)
		return ret;

	dac_threshold= AD74413R_THRESHOLD_DAC_RANGE * threshold /
		       AD74413R_THRESHOLD_RANGE;

	return ad74413r_reg_update(desc, AD74413R_DIN_THRESH,
				   AD74413R_COMP_THRESH_MASK, dac_threshold);
}

/**
 * @brief Set the logic value of a GPO pin
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The output logic state.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_gpo_set(struct ad74413r_desc *desc, uint32_t ch, uint8_t val)
{
	int ret;

	ret = ad74413r_set_gpo_config(desc, ch, AD74413R_GPO_CONFIG_DATA);
	if (ret)
		return ret;

	return ad74413r_reg_update(desc, AD74413R_GPO_CONFIG(ch),
				   AD74413R_GPO_DATA_MASK, val);
}

/**
 * @brief Set multiple GPO values at once.
 * @param desc - The device structure.
 * @param mask - Active channels mask.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_gpo_set_multiple(struct ad74413r_desc *desc, uint32_t mask)
{
	int ret;
	uint32_t bit;

	ret = ad74413r_reg_write(desc, AD74413R_GPO_PARALLEL, mask);
	if (ret)
		return ret;

	while (mask) {
		bit = no_os_find_first_set_bit(mask);

		ret = ad74413r_set_gpo_config(desc, bit, AD74413R_GPO_CONFIG_PAR_DATA);
		if (ret)
			return ret;

		mask >>= bit + 1;
	}

	return 0;
}

/**
 * @brief Read the live status bits.
 * @param desc - The device structure.
 * @param status - The register's value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_get_live(struct ad74413r_desc *desc,
		      union ad74413r_live_status *status)
{
	return ad74413r_reg_read(desc, AD74413R_LIVE_STATUS, &status->value);
}

/**
 * @brief The code value will be loaded into the DACs when the CLR_EN bit in the
 * OUTPUT_CONFIGx registers is asserted and the DAC clear key is written
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param code - The DAC code to be loaded
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_dac_clear_code(struct ad74413r_desc *desc, uint32_t ch,
				uint16_t code)
{
	return ad74413r_reg_write(desc, AD74413R_DAC_CLR_CODE(ch), code);
}

/**
 * @brief Clear the DAC (to the code in DAC_CLR_CODE register)
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_clear_dac(struct ad74413r_desc *desc, uint32_t ch)
{
	int ret;

	ret = ad74413r_reg_update(desc, AD74413R_OUTPUT_CONFIG(ch),
				  AD74413R_CLR_EN_MASK, 1);
	if (ret)
		return ret;

	ret = ad74413r_reg_write(desc, AD74413R_CMD_KEY, AD74413R_CMD_KEY_DAC_CLEAR);
	if (ret)
		return ret;

	return ad74413r_reg_update(desc, AD74413R_OUTPUT_CONFIG(ch),
				   AD74413R_CLR_EN_MASK, 0);
}

/**
 * @brief Configure and enable slew rate control for a DAC on a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param step - Number of codes per increment.
 * @param rate - Number of increments per second.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_dac_slew_enable(struct ad74413r_desc *desc, uint32_t ch,
			     enum ad74413r_slew_lin_step step,
			     enum ad74413r_lin_rate rate)
{
	int ret;

	ret = ad74413r_reg_update(desc, AD74413R_OUTPUT_CONFIG(ch),
				  AD74413R_SLEW_LIN_STEP_MASK, step);
	if (ret)
		return ret;

	ret = ad74413r_reg_update(desc, AD74413R_OUTPUT_CONFIG(ch),
				  AD74413R_SLEW_LIN_RATE_MASK, rate);
	if (ret)
		return ret;

	return ad74413r_reg_update(desc, AD74413R_OUTPUT_CONFIG(ch),
				   AD74413R_SLEW_EN_MASK, 1);
}

/**
 * @brief Disable the slew rate control.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_dac_slew_disable(struct ad74413r_desc *desc, uint32_t ch)
{
	return ad74413r_reg_update(desc, AD74413R_OUTPUT_CONFIG(ch),
				   AD74413R_SLEW_EN_MASK, 0);
}

/**
 * @brief Enable or disable the higher thermal reset.
 * @param desc - The device structure.
 * @param enable - The thermal reset status.
 * 			false: reset at 110 deg. Celsius.
 * 			true: reset at 140 deg. Celsius.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_set_therm_rst(struct ad74413r_desc *desc, bool enable)
{
	return ad74413r_reg_write(desc, AD74413R_THERM_RST, enable);
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad74413r_init(struct ad74413r_desc **desc,
		  struct ad74413r_init_param *init_param)
{
	int ret;
	struct ad74413r_desc *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, &init_param->comm_param);
	if (ret)
		goto err;

	no_os_crc8_populate_msb(_crc_table, AD74413R_CRC_POLYNOMIAL);

	ret = no_os_gpio_get_optional(&descriptor->reset_gpio,
				      init_param->reset_gpio_param);
	if (ret)
		goto comm_err;

	ret = ad74413r_reset(descriptor);
	if (ret)
		goto free_reset;

	ret = ad74413r_clear_errors(descriptor);
	if (ret)
		goto free_reset;

	ret = ad74413r_scratch_test(descriptor);
	if (ret)
		goto free_reset;

	descriptor->chip_id = init_param->chip_id;

	*desc = descriptor;

	return 0;

free_reset:
	no_os_gpio_remove(descriptor->reset_gpio);
comm_err:
	no_os_spi_remove(descriptor->comm_desc);
err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int ad74413r_remove(struct ad74413r_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	/* Perform a reset to bring the device in the default state */
	ret = ad74413r_reset(desc);
	if (ret)
		return ret;

	if (desc->reset_gpio) {
		ret = no_os_gpio_remove(desc->reset_gpio);
		if (ret)
			return ret;
	}

	ret = no_os_spi_remove(desc->comm_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
