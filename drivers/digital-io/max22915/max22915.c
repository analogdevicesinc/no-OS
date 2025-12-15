/***************************************************************************//**
 *   @file   max22915.c
 *   @brief  Source file of MAX22915 Driver.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "max22915.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include <stdio.h>
#include <errno.h>

/**
 * @brief Compute the CRC5 value for an array of bytes
 * @param data - Array of data to encode
 * @param encode - True for encoding (write), false for validation (read)
 *               - for read validation, excludes the first bit (MSB) of Rx data
 * @return The resulted CRC5
 */
static uint8_t max22915_crc(uint8_t *data, bool encode)
{
	uint8_t crc5_start = 0x1f;
	uint8_t crc5_poly = 0x15;
	uint8_t crc5_result = crc5_start;
	uint8_t extra_byte = 0x00;
	uint8_t data_bit;
	uint8_t result_bit;
	int i;

	for (i = (encode) ? 0 : 1; i < 8; i++) {
		data_bit = (data[0] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (i = 0; i < 8; i++) {
		data_bit = (data[1] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (i = 0; i < 3; i++) {
		data_bit = (extra_byte >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	return crc5_result;
}

/**
 * @brief Read register data from MAX22915
 * @param desc - Device descriptor for the MAX22915
 * @param reg_addr - Register address to read from
 * @param reg_data - Pointer to store the read data
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_reg_read(struct max22915_desc *desc, uint8_t reg_addr,
		      uint8_t *reg_data)
{
	int ret;

	if (!desc || !reg_data)
		return -EINVAL;

	struct no_os_spi_msg spi_msg = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX22915_SPI_FRAME_SIZE,
		.cs_change = 1,
	};

	desc->buff[0] = no_os_field_prep(MAX22915_CHIP_ADDR_MASK, desc->chip_address) |
			no_os_field_prep(MAX22915_ADDR_MASK, reg_addr) |
			no_os_field_prep(MAX22915_RW_MASK, 0);
	desc->buff[1] = 0x0;

	if (desc->crc_en) {
		spi_msg.bytes_number++;
		desc->buff[2] = max22915_crc(&desc->buff[0], true);
	}

	ret = no_os_spi_transfer(desc->spi_desc, &spi_msg, 1);
	if (ret)
		return ret;

	if (desc->crc_en) {
		if (max22915_crc(&desc->buff[0], false) != desc->buff[2]) {
			pr_err("CRC Mismatch while reading register 0x%02X\r\n", reg_addr);
			return -EINVAL;
		}
	}

	*reg_data = desc->buff[1];

	return 0;
}

/**
 * @brief Write register data to MAX22915
 * @param desc - Device descriptor for the MAX22915
 * @param reg_addr - Register address to write to
 * @param reg_data - Data to be written
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_reg_write(struct max22915_desc *desc, uint8_t reg_addr,
		       uint8_t reg_data)
{
	if (!desc)
		return -EINVAL;

	struct no_os_spi_msg spi_msg = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX22915_SPI_FRAME_SIZE,
		.cs_change = 1,
	};

	desc->buff[0] = no_os_field_prep(MAX22915_CHIP_ADDR_MASK, desc->chip_address) |
			no_os_field_prep(MAX22915_ADDR_MASK, reg_addr) |
			no_os_field_prep(MAX22915_RW_MASK, 1);
	desc->buff[1] = reg_data;

	if (desc->crc_en) {
		spi_msg.bytes_number += 1;
		desc->buff[2] = max22915_crc(&desc->buff[0], true);
	}

	return no_os_spi_transfer(desc->spi_desc, &spi_msg, 1);
}

/**
 * @brief Write register data to MAX22915 with mask
 * @param desc - Device descriptor for the MAX22915
 * @param reg_addr - Register address to write to
 * @param mask - Bit mask for the operation
 * @param reg_data - Data to be written
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_reg_write_mask(struct max22915_desc *desc,
			    uint8_t reg_addr,
			    uint8_t mask,
			    uint8_t reg_data)
{
	int ret;
	uint8_t reg_val;

	ret = max22915_reg_read(desc, reg_addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= (reg_data & mask);

	return max22915_reg_write(desc, reg_addr, reg_val);
}

/**
 * @brief Set the switch state of a channel
 * @param dev - Device descriptor for the MAX22915
 * @param channel - Channel index
 * @param state - Switch state to be set
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_set_sw_state(struct max22915_desc *dev,
			  uint8_t channel,
			  enum max22915_sw_state state)
{
	int ret;

	if (!dev || (channel > (MAX22915_MAX_CHANNELS - 1)))
		return -EINVAL;

	ret = max22915_reg_write_mask(dev,
				      MAX22915_REG_SET_OUTPUT,
				      MAX22915_CH_MASK(channel),
				      (state) ? MAX22915_CH_MASK(channel) : 0);
	if (ret)
		return ret;

	dev->sw_state[channel] = state;

	return 0;
}

/**
 * @brief Set the open-wire on detection configuration for a channel
 * @param dev - Device descriptor for the MAX22915
 * @param channel - Channel index
 * @param owon_enable - True to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_set_owon_config(struct max22915_desc *dev,
			     uint8_t channel,
			     bool owon_enable)
{
	int ret;

	if (!dev || (channel > (MAX22915_MAX_CHANNELS - 1)))
		return -EINVAL;

	ret = max22915_reg_write_mask(dev,
				      MAX22915_REG_OWON_ENABLE,
				      MAX22915_CH_MASK(channel),
				      owon_enable ? MAX22915_CH_MASK(channel) : 0);
	if (ret)
		return ret;

	dev->ow_config[channel].owon_enable = owon_enable;

	return 0;
}

/**
 * @brief Set the open-wire off detection configuration for a channel
 * @param dev - Device descriptor for the MAX22915
 * @param channel - Channel index
 * @param owoff_enable - True to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_set_owoff_config(struct max22915_desc *dev,
			      uint8_t channel,
			      bool owoff_enable)
{
	int ret;

	if (!dev || (channel > (MAX22915_MAX_CHANNELS - 1)))
		return -EINVAL;

	ret = max22915_reg_write_mask(dev,
				      MAX22915_REG_OWOFF_ENABLE,
				      MAX22915_CH_MASK(channel),
				      owoff_enable ? MAX22915_CH_MASK(channel) : 0);
	if (ret)
		return ret;

	dev->ow_config[channel].owoff_enable = owoff_enable;

	return 0;
}

/**
 * @brief Set the short to VDD detection configuration for a channel
 * @param dev - Device descriptor for the MAX22915
 * @param channel - Channel index
 * @param shtvdd_enable - True to enable, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_set_shvdd_config(struct max22915_desc *dev,
			      uint8_t channel,
			      bool shtvdd_enable)
{
	int ret;

	if (!dev || (channel > (MAX22915_MAX_CHANNELS - 1)))
		return -EINVAL;

	ret = max22915_reg_write_mask(dev,
				      MAX22915_REG_SHTVDD_ENABLE,
				      MAX22915_CH_MASK(channel),
				      shtvdd_enable ? MAX22915_CH_MASK(channel) : 0);
	if (ret)
		return ret;

	dev->shtvdd_enable[channel] = shtvdd_enable;

	return 0;
}

/**
 * @brief Set the current measurement configuration
 * @param dev - Device descriptor for the MAX22915
 * @param config - Pointer to current configuration structure
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_set_current_config(struct max22915_desc *dev,
				struct max22915_current_config *config)
{
	uint8_t mask;
	uint8_t new_val;

	if (!dev || !config)
		return -EINVAL;

	mask = MAX22915_CURR_AVG_MASK | MAX22915_CURR_SINGLE_MASK |
	       MAX22915_CURR_CONT_MASK;

	new_val = no_os_field_prep(MAX22915_CURR_AVG_MASK, config->curr_average) |
		  (config->curr_one_shot_en ? MAX22915_CURR_SINGLE_MASK : 0) |
		  (config->curr_cont ? MAX22915_CURR_CONT_MASK : 0);

	return max22915_reg_write_mask(dev, MAX22915_REG_ADC_CONFIG2, mask, new_val);
}

/**
 * @brief Set the temperature measurement configuration
 * @param dev - Device descriptor for the MAX22915
 * @param config - Pointer to temperature configuration structure
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_set_temperature_config(struct max22915_desc *dev,
				    struct max22915_temp_config *config)
{
	uint8_t mask;
	uint8_t new_val;

	if (!dev || !config)
		return -EINVAL;

	mask = MAX22915_TEMP_AVG_MASK | MAX22915_THERM_WARN_EN_MASK |
	       MAX22915_TEMP_CONT_MASK;

	new_val = no_os_field_prep(MAX22915_TEMP_AVG_MASK, config->temp_average) |
		  (config->therm_warn_en ? MAX22915_THERM_WARN_EN_MASK : 0) |
		  (config->temp_cont ? MAX22915_TEMP_CONT_MASK : 0);

	return max22915_reg_write_mask(dev, MAX22915_REG_ADC_CONFIG1, mask, new_val);
}

/**
 * @brief Initialize and configure the MAX22915 device
 * @param device - Device descriptor for the MAX22915 that will be initialized
 * @param init_param - Initialization parameter for the device
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_init(struct max22915_desc **device,
		  struct max22915_init_param *init_param)
{
	struct max22915_desc *dev;
	int ret;
	uint8_t i;
	uint8_t reg_data;

	dev = (struct max22915_desc *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto err;

	dev->crc_en = init_param->crc_en;
	dev->chip_address = init_param->chip_address;

	/* Clear all latched faults generated at power up */
	ret = max22915_reg_read(dev, MAX22915_REG_OVL_FLT, &reg_data);
	if (ret)
		goto err;

	ret = max22915_reg_read(dev, MAX22915_REG_OWOFF_FLT, &reg_data);
	if (ret)
		goto err;

	ret = max22915_reg_read(dev, MAX22915_REG_OWON_FLT, &reg_data);
	if (ret)
		goto err;

	ret = max22915_reg_read(dev, MAX22915_REG_SHTVDD_FLT, &reg_data);
	if (ret)
		goto err;

	ret = max22915_reg_read(dev, MAX22915_REG_GLOBAL_ERR, &reg_data);
	if (ret)
		goto err;

	for (i = 0; i < MAX22915_MAX_CHANNELS; i++) {
		ret = max22915_set_sw_state(dev, i, init_param->sw_state[i]);
		if (ret)
			goto err;

		ret = max22915_set_owon_config(dev, i, init_param->ow_config[i].owon_enable);
		if (ret)
			goto err;

		ret = max22915_set_owoff_config(dev, i, init_param->ow_config[i].owoff_enable);
		if (ret)
			goto err;

		ret = max22915_set_shvdd_config(dev, i, init_param->shtvdd_enable[i]);
		if (ret)
			goto err;
	}

	ret = max22915_set_current_config(dev, &init_param->curr_config);
	if (ret)
		goto err;

	ret = max22915_set_temperature_config(dev, &init_param->temp_config);
	if (ret)
		goto err;

	*device = dev;

	return 0;

err:
	if (dev->spi_desc)
		no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}

/**
 * @brief Remove resources allocated for MAX22915
 * @param dev - device descriptor for the MAX22915
 * @return 0 in case of success, negative error code otherwise
 */
int max22915_remove(struct max22915_desc *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_spi_remove(dev->spi_desc);
	no_os_free(dev);

	return 0;
}
