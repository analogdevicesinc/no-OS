/***************************************************************************//**
 *   @file   ad5460.c
 *   @brief  Source file of AD5460 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "ad5460.h"
#include "no_os_crc8.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD5460_CRC_POLYNOMIAL 	0x7
#define AD5460_DIN_DEBOUNCE_LEN 	NO_OS_BIT(5)
#define AD77416H_DEV_ADDRESS_MSK	NO_OS_GENMASK(5, 4)

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
NO_OS_DECLARE_CRC8_TABLE(_crc_table);

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
int ad5460_dac_voltage_to_code(struct ad5460_desc *desc, int32_t mvolts,
			       uint16_t *code, uint32_t ch)
{
	uint32_t range, offset;

	switch (desc->channel_configs[ch].vout_range) {
	case AD5460_VOUT_RANGE_0_12V:
		if (mvolts > AD5460_DAC_RANGE || mvolts < 0)
			return -EINVAL;
		range = AD5460_DAC_RANGE;
		offset = 0;
		break;
	case AD5460_VOUT_RANGE_NEG12_12V:
		if (mvolts > AD5460_DAC_RANGE || mvolts < -AD5460_DAC_RANGE)
			return -EINVAL;
		range = AD5460_DAC_RANGE * 2;
		offset = AD5460_DAC_RANGE;
		break;
	default:
		return -EINVAL;
	}

	*code = (mvolts + offset) * NO_OS_BIT(AD5460_DAC_RESOLUTION) / range;

	return 0;
}

/**
 * @brief Convers a microamp value in the corresponding DAC 16 bit code
 * @param desc - The device structure.
 * @param uamps - The microamps value
 * @param code - The resulting DAC code
 * @param ch - Channel Number
 * @return 0 in case of success, -EINVAL otherwise
 */
int ad5460_dac_current_to_code(struct ad5460_desc *desc, uint32_t uamps,
			       uint16_t *code, uint32_t ch)
{
	uint32_t range, offset;

	switch (desc->channel_configs[ch].iout_range) {
	case AD5460_IOUT_RANGE_0_25MA:
		if (uamps > AD5460_DAC_CURRENT_RANGE_25MA || uamps < 0)
			return -EINVAL;
		range = AD5460_DAC_CURRENT_RANGE_25MA;
		offset = 0;
		break;
	case AD5460_IOUT_RANGE_0_20MA:
		if (uamps > AD5460_DAC_RANGE || uamps < -AD5460_DAC_RANGE)
			return -EINVAL;
		range = AD5460_DAC_CURRENT_RANGE_20MA;
		offset = 0;
		break;
	case AD5460_IOUT_RANGE_4_20MA:
		if (uamps > AD5460_DAC_CURRENT_RANGE_20MA ||
		    uamps < AD5460_DAC_CURRENT_RANGE_4MA)
			return -EINVAL;
		range = AD5460_DAC_CURRENT_RANGE_20MA - AD5460_DAC_CURRENT_RANGE_4MA;
		offset = AD5460_DAC_CURRENT_RANGE_4MA;
		break;
	default:
		return -EINVAL;
	}

	*code = (uamps - offset) * NO_OS_BIT(AD5460_DAC_RESOLUTION) / range;
	return 0;
}

/**
 * @brief Load the address and value in a communication buffer using
 * the format that the device expects.
 * @param reg - Register address.
 * @param val - Register value.
 * @param buff - The communication buffer.
 */
static void ad5460_format_reg_write(uint8_t addr, uint8_t reg, uint16_t val,
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
int ad5460_reg_read_raw(struct ad5460_desc *desc, uint32_t addr,
			uint8_t *val)
{
	int ret;
	/**
	 * Reading a register on AD5460 requires writing the address to the READ_SELECT
	 * register first and then doing another spi read, which will contain the requested
	 * register value.
	 */
	ad5460_format_reg_write(desc->dev_addr, AD5460_READ_SELECT, addr,
				desc->comm_buff);

	ret = no_os_spi_write_and_read(desc->spi_desc, desc->comm_buff,
				       AD5460_FRAME_SIZE);
	if (ret)
		return ret;

	/* Make sure that NOP sequence is written for the second frame */
	ad5460_format_reg_write(desc->dev_addr, AD5460_NOP, AD5460_NOP,
				val);

	return no_os_spi_write_and_read(desc->spi_desc, val, AD5460_FRAME_SIZE);
}

/**
 * @brief Write a register's value
 * @param desc  - The device structure.
 * @param addr - The register's address.
 * @param val - The register's value.
 * @return 0 in case of success, negative error otherwise
 */
int ad5460_reg_write(struct ad5460_desc *desc, uint32_t addr, uint16_t val)
{
	ad5460_format_reg_write(desc->dev_addr, addr, val, desc->comm_buff);

	return no_os_spi_write_and_read(desc->spi_desc, desc->comm_buff,
					AD5460_FRAME_SIZE);
}

/**
 * @brief Read a register's value
 * @param desc  - The device structure.
 * @param addr - The register's address.
 * @param val - The register's read value.
 * @return 0 in case of success, negative error otherwise
 */
int ad5460_reg_read(struct ad5460_desc *desc, uint32_t addr, uint16_t *val)
{
	int ret;
	uint8_t expected_crc;

	ret = ad5460_reg_read_raw(desc, addr, desc->comm_buff);
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
int ad5460_reg_update(struct ad5460_desc *desc, uint32_t addr,
		      uint16_t mask,
		      uint16_t val)
{
	int ret;
	uint16_t data;

	ret = ad5460_reg_read(desc, addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= no_os_field_prep(mask, val);

	return ad5460_reg_write(desc, addr, data);
}

/**
 * @brief Set the operation mode for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param ch_func - The operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_set_channel_function(struct ad5460_desc *desc,
				uint32_t ch, enum ad5460_op_mode ch_func)
{
	int ret;

	ret = ad5460_reg_update(desc, AD5460_CH_FUNC_SETUP(ch),
				AD5460_CH_FUNC_SETUP_MSK, AD5460_HIGH_Z);
	if (ret)
		return ret;

	/* Datasheet delay required before transition to new desired mode */
	no_os_udelay(200);

	ret = ad5460_reg_update(desc, AD5460_CH_FUNC_SETUP(ch),
				AD5460_CH_FUNC_SETUP_MSK, ch_func);
	if (ret)
		return ret;

	desc->channel_configs[ch].function = ch_func;

	return 0;
}

/**
 * @brief Set the voltage range for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param vout_range - The voltage range.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_set_channel_vout_range(struct ad5460_desc *desc, uint32_t ch,
				  enum ad5460_vout_range vout_range)
{
	int ret;

	ret = ad5460_reg_update(desc, AD5460_OUTPUT_CONFIG(ch),
				AD5460_VOUT_RANGE_MSK, vout_range);
	if (ret)
		return ret;

	desc->channel_configs[ch].vout_range = vout_range;

	return 0;
}

/**
 * @brief Set the current range for a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param iout_range - The current range.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_set_channel_iout_range(struct ad5460_desc *desc, uint32_t ch,
				  enum ad5460_iout_range iout_range)
{
	int ret;

	ret = ad5460_reg_update(desc, AD5460_OUTPUT_CONFIG(ch),
				AD5460_IOUT_RANGE_MSK, iout_range);
	if (ret)
		return ret;

	desc->channel_configs[ch].iout_range = iout_range;

	return 0;
}

/**
 * @brief Set the current limit for a specific DAC channel in vout mode
 * @param desc - The devices structure
 * @param ch - The channel index
 * @param i_limit - The current limit
 * @return 0 in case of success, negative error otherwise
 */
int ad5460_set_channel_i_limit(struct ad5460_desc *desc,
			       uint32_t ch, enum ad5460_i_limit i_limit)
{
	int ret;
	ret = ad5460_reg_update(desc, AD5460_OUTPUT_CONFIG(ch),
				AD5460_I_LIMIT_MSK, i_limit);
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
int ad5460_set_channel_dac_code(struct ad5460_desc *desc, uint32_t ch,
				uint16_t dac_code)
{
	return ad5460_reg_write(desc, AD5460_DAC_CODE(ch), dac_code);
}

/**
 * @brief Set which diagnostic value to be loaded in the DIAG_RESULT register
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param diag_code - The diagnostic setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_set_diag(struct ad5460_desc *desc, uint32_t ch,
		    enum ad5460_diag_mode diag_code)
{
	return ad5460_reg_write(desc, AD5460_DIAG_CONFIG, diag_code);
}

/**
 * @brief Set the GPIO operation mode.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param config - The configuration setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_set_gpio_config(struct ad5460_desc *desc, uint32_t ch,
			   enum ad5460_gpio_select config)
{
	return ad5460_reg_update(desc, AD5460_GPIO_CONFIG(ch),
				 AD5460_GPIO_SELECT_MSK, config);
}

/**
 * @brief Set the logic value of a GPO pin
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The output logic state.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_gpio_set(struct ad5460_desc *desc, uint32_t ch, uint8_t val)
{
	int ret;

	ret = ad5460_set_gpio_config(desc, ch, AD5460_GPIO_SEL_GPIO);
	if (ret)
		return ret;

	return ad5460_reg_update(desc, AD5460_GPIO_CONFIG(ch),
				 AD5460_GPO_DATA_MSK, val);
}

/**
 * @brief Get the GPO value for a specific channel.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param val - The debounce time.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_gpio_get(struct ad5460_desc *desc, uint32_t ch, uint8_t *val)
{
	int ret;
	uint16_t reg;

	ret = ad5460_reg_read(desc, AD5460_GPIO_CONFIG(ch), &reg);
	if (ret)
		return ret;

	*val = no_os_field_get(AD5460_GPI_DATA_MSK, reg);

	return 0;
}

/**
 * @brief Read the live status bits.
 * @param desc - The device structure.
 * @param status - The register's value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_get_live(struct ad5460_desc *desc,
		    union ad5460_live_status *status)
{
	return ad5460_reg_read(desc, AD5460_LIVE_STATUS, &status->value);
}

/**
 * @brief Configure and enable slew rate control for a DAC on a specific channel
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @param step - Number of codes per increment.
 * @param rate - Number of increments per second.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_dac_slew_enable(struct ad5460_desc *desc, uint32_t ch,
			   enum ad5460_slew_lin_step step,
			   enum ad5460_lin_rate rate)
{
	int ret;

	ret = ad5460_reg_update(desc, AD5460_OUTPUT_CONFIG(ch),
				AD5460_SLEW_LIN_STEP_MSK, step);
	if (ret)
		return ret;

	ret = ad5460_reg_update(desc, AD5460_OUTPUT_CONFIG(ch),
				AD5460_SLEW_LIN_RATE_MSK, rate);
	if (ret)
		return ret;

	return ad5460_reg_update(desc, AD5460_OUTPUT_CONFIG(ch),
				 AD5460_SLEW_EN_MSK, 1);
}

/**
 * @brief Disable the slew rate control.
 * @param desc - The device structure.
 * @param ch - The channel index.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_dac_slew_disable(struct ad5460_desc *desc, uint32_t ch)
{
	return ad5460_reg_update(desc, AD5460_OUTPUT_CONFIG(ch),
				 AD5460_SLEW_EN_MSK, 0);
}

/**
 * @brief Enable or disable the higher thermal reset.
 * @param desc - The device structure.
 * @param enable - The thermal reset status.
 * 			false: reset at 110 deg. Celsius.
 * 			true: reset at 140 deg. Celsius.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_set_therm_rst(struct ad5460_desc *desc, bool enable)
{
	return ad5460_reg_write(desc, AD5460_THERM_RST, enable);
}

/**
 * @brief Perform software or hardware reset and wait for device reset time.
 * @param desc - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5460_reset(struct ad5460_desc *desc)
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
		ret = ad5460_reg_write(desc, AD5460_CMD_KEY, AD5460_CMD_KEY_RESET_1);
		if (ret)
			return ret;

		ret = ad5460_reg_write(desc, AD5460_CMD_KEY, AD5460_CMD_KEY_RESET_2);
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
static int ad5460_scratch_test(struct ad5460_desc *desc)
{
	int ret;
	uint16_t val;
	uint16_t test_val = 0x1234;

	ret = ad5460_reg_write(desc, AD5460_SCRATCH(0), test_val);
	if (ret)
		return ret;

	ret = ad5460_reg_read(desc, AD5460_SCRATCH(0), &val);
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
int ad5460_init(struct ad5460_desc **desc,
		struct ad5460_init_param *init_param)
{
	int ret;
	struct ad5460_desc *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->spi_desc, &init_param->spi_ip);
	if (ret)
		goto err;

	descriptor->dev_addr = init_param->dev_addr;

	no_os_crc8_populate_msb(_crc_table, AD5460_CRC_POLYNOMIAL);

	ret = no_os_gpio_get_optional(&descriptor->reset_gpio,
				      init_param->reset_gpio_param);
	if (ret)
		goto comm_err;

	ret = ad5460_reset(descriptor);
	if (ret)
		goto gpio_err;

	ret = ad5460_scratch_test(descriptor);
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
int ad5460_remove(struct ad5460_desc *desc)
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
