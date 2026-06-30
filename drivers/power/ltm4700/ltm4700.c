/***************************************************************************//**
 *   @file   ltm4700.c
 *   @brief  Source code of the LTM4700 driver
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_crc8.h"

#include "ltm4700.h"

NO_OS_DECLARE_CRC8_TABLE(ltm4700_crc_table);

/**
 * @brief Check if PMBus command is paged
 *
 * @param cmd - PMBus command
 * @return true if paged, false otherwise
 */
static bool ltm4700_cmd_is_paged(uint8_t cmd)
{
	switch (cmd) {
	case LTM4700_OPERATION:
	case LTM4700_ON_OFF_CONFIG:
	case LTM4700_SMBALERT_MASK:
	case LTM4700_VOUT_MODE:
	case LTM4700_VOUT_COMMAND:
	case LTM4700_VOUT_MAX:
	case LTM4700_VOUT_MARGIN_HIGH:
	case LTM4700_VOUT_MARGIN_LOW:
	case LTM4700_VOUT_OV_FAULT_LIMIT:
	case LTM4700_VOUT_OV_FAULT_RESPONSE:
	case LTM4700_VOUT_OV_WARN_LIMIT:
	case LTM4700_VOUT_UV_WARN_LIMIT:
	case LTM4700_VOUT_UV_FAULT_LIMIT:
	case LTM4700_VOUT_UV_FAULT_RESPONSE:
	case LTM4700_IOUT_OC_FAULT_LIMIT:
	case LTM4700_IOUT_OC_FAULT_RESPONSE:
	case LTM4700_IOUT_OC_WARN_LIMIT:
	case LTM4700_TON_DELAY:
	case LTM4700_TOFF_DELAY:
	case LTM4700_STATUS_BYTE:
	case LTM4700_STATUS_WORD:
	case LTM4700_STATUS_VOUT:
	case LTM4700_STATUS_IOUT:
	case LTM4700_STATUS_TEMPERATURE:
	case LTM4700_STATUS_MFR_SPECIFIC:
	case LTM4700_READ_VOUT:
	case LTM4700_READ_IOUT:
	case LTM4700_READ_TEMPERATURE_1:
	case LTM4700_READ_FREQUENCY:
	case LTM4700_READ_POUT:
	case LTM4700_READ_PIN:
	case LTM4700_MFR_VOUT_MAX:
	case LTM4700_MFR_CHAN_CONFIG:
	case LTM4700_MFR_FAULT_PROPAGATE:
	case LTM4700_MFR_PWM_COMP:
	case LTM4700_MFR_PWM_MODE:
	case LTM4700_MFR_FAULT_RESPONSE:
	case LTM4700_MFR_IOUT_PEAK:
	case LTM4700_MFR_RETRY_DELAY:
	case LTM4700_MFR_RESTART_DELAY:
	case LTM4700_MFR_VOUT_PEAK:
	case LTM4700_MFR_TEMPERATURE_1_PEAK:
	case LTM4700_MFR_PWM_CONFIG:
	case LTM4700_MFR_IOUT_CAL_GAIN_TC:
	case LTM4700_MFR_TEMP_1_GAIN:
	case LTM4700_MFR_TEMP_1_OFFSET:
	case LTM4700_MFR_RAIL_ADDRESS:
		return true;
	default:
		return false;
	}
}

/**
 * @brief Convert linear 16 format to microunits
 *
 * @param data - Raw PMBus data
 * @param exp - Linear format exponent
 * @return Converted value in microunits
 */
static int ltm4700_lin16_to_uval(uint16_t data, int exp)
{
	int mantissa = (int16_t)data;
	int val;

	if (exp >= 0)
		val = mantissa << exp;
	else
		val = mantissa >> (-exp);

	return val;
}

/**
 * @brief Convert microunits to linear 16 format
 *
 * @param uval - Value in microunits
 * @param exp - Linear format exponent
 * @return Converted PMBus data
 */
static uint16_t ltm4700_uval_to_lin16(int uval, int exp)
{
	int mantissa;

	if (exp >= 0)
		mantissa = uval >> exp;
	else
		mantissa = uval << (-exp);

	return (uint16_t)mantissa;
}

/**
 * @brief Convert linear 11 format to microunits
 *
 * @param data - Raw PMBus data
 * @return Converted value in microunits
 */
static int ltm4700_lin11_to_uval(uint16_t data)
{
	int exponent = LTM4700_LIN11_EXPONENT(data);
	int mantissa = LTM4700_LIN11_MANTISSA(data);
	int val;

	if (exponent >= 0)
		val = mantissa << exponent;
	else
		val = mantissa >> (-exponent);

	return val;
}

/**
 * @brief Convert microunits to linear 11 format
 *
 * @param uval - Value in microunits
 * @return Converted PMBus data
 */
static uint16_t ltm4700_uval_to_lin11(int uval)
{
	int exponent = 0;
	int mantissa = uval;

	/* Normalize mantissa to 11-bit range */
	while (mantissa > LTM4700_LIN11_MANTISSA_MAX
	       || mantissa < LTM4700_LIN11_MANTISSA_MIN) {
		if (mantissa > LTM4700_LIN11_MANTISSA_MAX) {
			mantissa >>= 1;
			exponent++;
		} else {
			mantissa <<= 1;
			exponent--;
		}

		if (exponent > LTM4700_LIN11_EXPONENT_MAX
		    || exponent < LTM4700_LIN11_EXPONENT_MIN)
			break;
	}

	return (uint16_t)((exponent << 11) | (mantissa & LTM4700_LIN11_MANTISSA_MSK));
}

/**
 * @brief Verify manufacturer ID
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int ltm4700_verify_manufacturer_id(struct ltm4700_dev *dev)
{
	uint8_t mfr_id[LTM4700_MFR_ID_SIZE];
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltm4700_read_block_data(dev, 0, LTM4700_MFR_ID,
				      mfr_id, LTM4700_MFR_ID_SIZE);
	if (ret)
		return ret;

	if (strncmp((char *)mfr_id, LTM4700_MFR_ID_VALUE,
		    LTM4700_MFR_ID_SIZE) != 0)
		return -ENODEV;

	return 0;
}

/**
 * @brief Verify manufacturer model
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int ltm4700_verify_manufacturer_model(struct ltm4700_dev *dev)
{
	uint8_t mfr_model[LTM4700_MFR_MODEL_SIZE];
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltm4700_read_block_data(dev, 0, LTM4700_MFR_MODEL,
				      mfr_model, LTM4700_MFR_MODEL_SIZE);
	if (ret)
		return ret;

	if (strncmp((char *)mfr_model, LTM4700_MFR_MODEL_VALUE,
		    LTM4700_MFR_MODEL_SIZE) != 0)
		return -ENODEV;

	return 0;
}

/**
 * @brief Initialize the device structure
 *
 * @param device - The device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_init(struct ltm4700_dev **device,
		 struct ltm4700_init_param *init_param)
{
	struct ltm4700_dev *dev;
	uint16_t special_id;
	int ret;

	if (!device || !init_param || !init_param->i2c_init)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto error_dev;

	if (init_param->crc_en) {
		no_os_crc8_populate_msb(ltm4700_crc_table, LTM4700_CRC_POLYNOMIAL);
		dev->crc_en = true;
	}

	dev->page = -1;
	dev->lin16_exp = LTM4700_LIN16_EXPONENT;

	/* Read device ID to determine variant */
	ret = ltm4700_read_word(dev, 0, LTM4700_MFR_SPECIAL_ID, &special_id);
	if (ret)
		goto error_i2c;

	/* Determine device variant */
	if (LTM4700_SPECIAL_ID_VALUE != (special_id & LTM4700_ID_MSK)) {
		ret = -ENODEV;
		goto error_i2c;
	}

	ret = ltm4700_verify_manufacturer_id(dev);
	if (ret)
		goto error_i2c;

	ret = ltm4700_verify_manufacturer_model(dev);
	if (ret)
		goto error_i2c;

	dev->num_channels = 2;

	if (init_param->alert_param) {
		ret = no_os_gpio_get(&dev->alert_desc, init_param->alert_param);
		if (ret)
			goto error_i2c;
	}

	if (init_param->pgood_params) {
		dev->pgood_descs = no_os_calloc(dev->num_channels, sizeof(*dev->pgood_descs));
		if (!dev->pgood_descs) {
			ret = -ENOMEM;
			goto error_alert;
		}

		for (int i = 0; i < dev->num_channels; i++) {
			if (init_param->pgood_params[i]) {
				ret = no_os_gpio_get(&dev->pgood_descs[i],
						     init_param->pgood_params[i]);
				if (ret)
					goto error_pgood;
			}
		}
	}

	if (init_param->run_params) {
		dev->run_descs = no_os_calloc(dev->num_channels, sizeof(*dev->run_descs));
		if (!dev->run_descs) {
			ret = -ENOMEM;
			goto error_pgood;
		}

		for (int i = 0; i < dev->num_channels; i++) {
			if (init_param->run_params[i]) {
				ret = no_os_gpio_get(&dev->run_descs[i],
						     init_param->run_params[i]);
				if (ret)
					goto error_run;
			}
		}
	}

	if (init_param->fault_params) {
		dev->fault_descs = no_os_calloc(dev->num_channels, sizeof(*dev->fault_descs));
		if (!dev->fault_descs) {
			ret = -ENOMEM;
			goto error_run;
		}

		for (int i = 0; i < dev->num_channels; i++) {
			if (init_param->fault_params[i]) {
				ret = no_os_gpio_get(&dev->fault_descs[i],
						     init_param->fault_params[i]);
				if (ret)
					goto error_fault;
			}
		}
	}

	*device = dev;

	return 0;

error_fault:
	if (dev->fault_descs) {
		for (int i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->fault_descs[i]);
		no_os_free(dev->fault_descs);
	}
error_run:
	if (dev->run_descs) {
		for (int i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->run_descs[i]);
		no_os_free(dev->run_descs);
	}
error_pgood:
	if (dev->pgood_descs) {
		for (int i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->pgood_descs[i]);
		no_os_free(dev->pgood_descs);
	}
error_alert:
	no_os_gpio_remove(dev->alert_desc);
error_i2c:
	no_os_i2c_remove(dev->i2c_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free or remove device instance
 *
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_remove(struct ltm4700_dev *dev)
{
	int ret = 0;
	int tmp_ret;

	if (!dev)
		return -EINVAL;

	if (dev->fault_descs) {
		for (int i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->fault_descs[i]);
		no_os_free(dev->fault_descs);
	}

	if (dev->run_descs) {
		for (int i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->run_descs[i]);
		no_os_free(dev->run_descs);
	}

	if (dev->pgood_descs) {
		for (int i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->pgood_descs[i]);
		no_os_free(dev->pgood_descs);
	}

	if (dev->alert_desc) {
		tmp_ret = no_os_gpio_remove(dev->alert_desc);
		if (tmp_ret && !ret)
			ret = tmp_ret;
	}

	if (dev->i2c_desc) {
		tmp_ret = no_os_i2c_remove(dev->i2c_desc);
		if (tmp_ret && !ret)
			ret = tmp_ret;
	}

	no_os_free(dev);

	return ret;
}

/**
 * @brief Set PMBus page
 *
 * @param dev - The device structure
 * @param page - Page number (0 or 1 for dual channel device, or 0xFF for broadcast)
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_set_page(struct ltm4700_dev *dev, int page)
{
	uint8_t buffer[2];
	int ret;

	if (!dev)
		return -EINVAL;

	if (page != LTM4700_CHAN_ALL && (page < 0 || page >= dev->num_channels))
		return -EINVAL;

	if (dev->page == page)
		return 0;

	buffer[0] = LTM4700_PAGE;
	buffer[1] = (uint8_t)page;
	ret = no_os_i2c_write(dev->i2c_desc, buffer, 2, true);
	if (ret)
		return ret;

	dev->page = page;

	return 0;
}

/**
 * @brief Send a PMBus command to the device
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_send_byte(struct ltm4700_dev *dev, int page, uint8_t cmd)
{
	if (!dev)
		return -EINVAL;

	if (ltm4700_cmd_is_paged(cmd)) {
		int ret = ltm4700_set_page(dev, page);
		if (ret)
			return ret;
	}

	return no_os_i2c_write(dev->i2c_desc, &cmd, 1, 1);
}

/**
 * @brief Perform a PMBus read_byte operation
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @param data - Read data
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_byte(struct ltm4700_dev *dev, int page, uint8_t cmd,
		      uint8_t *data)
{
	int ret;

	if (!dev || !data)
		return -EINVAL;

	if (ltm4700_cmd_is_paged(cmd)) {
		ret = ltm4700_set_page(dev, page);
		if (ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 1);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, data, 1, 1);
}

/**
 * @brief Perform a PMBus write_byte operation
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @param value - Data to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_write_byte(struct ltm4700_dev *dev, int page, uint8_t cmd,
		       uint8_t value)
{
	uint8_t data[2] = {cmd, value};

	if (!dev)
		return -EINVAL;

	if (ltm4700_cmd_is_paged(cmd)) {
		int ret = ltm4700_set_page(dev, page);
		if (ret)
			return ret;
	}

	return no_os_i2c_write(dev->i2c_desc, data, 2, true);
}

/**
 * @brief Perform a PMBus read_word operation
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @param word - Read word data
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_word(struct ltm4700_dev *dev, int page, uint8_t cmd,
		      uint16_t *word)
{
	int ret;
	uint8_t data[2];

	if (!dev || !word)
		return -EINVAL;

	if (ltm4700_cmd_is_paged(cmd)) {
		ret = ltm4700_set_page(dev, page);
		if (ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 1);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, data, 2, 1);
	if (ret)
		return ret;

	*word = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

	return 0;
}

/**
 * @brief Perform a PMBus write_word operation
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @param word - Word data to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_write_word(struct ltm4700_dev *dev, int page, uint8_t cmd,
		       uint16_t word)
{
	uint8_t data[3] = {cmd, (uint8_t)(word & 0xFF), (uint8_t)(word >> 8)};

	if (!dev)
		return -EINVAL;

	if (ltm4700_cmd_is_paged(cmd)) {
		int ret = ltm4700_set_page(dev, page);
		if (ret)
			return ret;
	}

	return no_os_i2c_write(dev->i2c_desc, data, 3, true);
}

/**
 * @brief Perform a PMBus read_word operation then perform conversion
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @param data - Converted data in microunits
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_word_data(struct ltm4700_dev *dev, int page, uint8_t cmd,
			   int *data)
{
	uint16_t word;
	int ret;

	if (!dev || !data)
		return -EINVAL;

	ret = ltm4700_read_word(dev, page, cmd, &word);
	if (ret)
		return ret;

	/* Determine format based on command */
	switch (cmd) {
	case LTM4700_VOUT_COMMAND:
	case LTM4700_VOUT_MAX:
	case LTM4700_VOUT_MARGIN_HIGH:
	case LTM4700_VOUT_MARGIN_LOW:
	case LTM4700_VOUT_OV_FAULT_LIMIT:
	case LTM4700_VOUT_OV_WARN_LIMIT:
	case LTM4700_VOUT_UV_WARN_LIMIT:
	case LTM4700_VOUT_UV_FAULT_LIMIT:
	case LTM4700_READ_VOUT:
	case LTM4700_MFR_VOUT_MAX:
	case LTM4700_MFR_VOUT_PEAK:
		/* Linear 16 format for voltage commands */
		*data = ltm4700_lin16_to_uval(word, dev->lin16_exp);
		break;
	default:
		/* Linear 11 format for other commands */
		*data = ltm4700_lin11_to_uval(word);
		break;
	}

	return 0;
}

/**
 * @brief Perform conversion then perform a PMBus write_word operation
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @param data - Data in microunits to convert and write
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_write_word_data(struct ltm4700_dev *dev, int page, uint8_t cmd,
			    int data)
{
	uint16_t word;

	if (!dev)
		return -EINVAL;

	/* Determine format based on command */
	switch (cmd) {
	case LTM4700_VOUT_COMMAND:
	case LTM4700_VOUT_MAX:
	case LTM4700_VOUT_MARGIN_HIGH:
	case LTM4700_VOUT_MARGIN_LOW:
	case LTM4700_VOUT_OV_FAULT_LIMIT:
	case LTM4700_VOUT_OV_WARN_LIMIT:
	case LTM4700_VOUT_UV_WARN_LIMIT:
	case LTM4700_VOUT_UV_FAULT_LIMIT:
	case LTM4700_MFR_VOUT_MAX:
		/* Linear 16 format for voltage commands */
		word = ltm4700_uval_to_lin16(data, dev->lin16_exp);
		break;
	default:
		/* Linear 11 format for other commands */
		word = ltm4700_uval_to_lin11(data);
		break;
	}

	return ltm4700_write_word(dev, page, cmd, word);
}

/**
 * @brief Read a block of bytes
 *
 * @param dev - The device structure
 * @param page - Page number for paged commands
 * @param cmd - PMBus command
 * @param data - Read data buffer
 * @param nbytes - Number of bytes to read
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_block_data(struct ltm4700_dev *dev, int page, uint8_t cmd,
			    uint8_t *data, size_t nbytes)
{
	int ret;

	if (!dev || !data || !nbytes)
		return -EINVAL;

	if (ltm4700_cmd_is_paged(cmd)) {
		ret = ltm4700_set_page(dev, page);
		if (ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 1);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, data, nbytes, 1);
}

/**
 * @brief Read specific value type
 *
 * @param dev - The device structure
 * @param channel - Channel number
 * @param value_type - Type of value to read
 * @param value - Read value in microunits
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_value(struct ltm4700_dev *dev, uint8_t channel,
		       enum ltm4700_value_type value_type, int *value)
{
	if (!dev || !value)
		return -EINVAL;

	if (channel >= dev->num_channels)
		return -EINVAL;

	return ltm4700_read_word_data(dev, channel, (uint8_t)value_type, value);
}

/**
 * @brief Read status
 *
 * @param dev - The device structure
 * @param channel - Channel number
 * @param status_type - Type of status to read
 * @param status - Status structure to fill
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_read_status(struct ltm4700_dev *dev, uint8_t channel,
			enum ltm4700_status_type status_type, struct ltm4700_status *status)
{
	int ret = 0;

	if (!dev || !status)
		return -EINVAL;

	if (channel >= dev->num_channels)
		return -EINVAL;

	memset(status, 0, sizeof(*status));

	if (status_type & LTM4700_STATUS_BYTE_TYPE_MSK) {
		ret = ltm4700_read_byte(dev, channel, LTM4700_STATUS_BYTE, &status->byte);
		if (ret)
			return ret;
	}

	if (status_type & LTM4700_STATUS_WORD_TYPE_MSK) {
		ret = ltm4700_read_word(dev, channel, LTM4700_STATUS_WORD, &status->word);
		if (ret)
			return ret;
	}

	if (status_type & LTM4700_STATUS_VOUT_TYPE_MSK) {
		ret = ltm4700_read_byte(dev, channel, LTM4700_STATUS_VOUT, &status->vout);
		if (ret)
			return ret;
	}

	if (status_type & LTM4700_STATUS_IOUT_TYPE_MSK) {
		ret = ltm4700_read_byte(dev, channel, LTM4700_STATUS_IOUT, &status->iout);
		if (ret)
			return ret;
	}

	if (status_type & LTM4700_STATUS_INPUT_TYPE_MSK) {
		ret = ltm4700_read_byte(dev, 0, LTM4700_STATUS_INPUT, &status->input);
		if (ret)
			return ret;
	}

	if (status_type & LTM4700_STATUS_TEMP_TYPE_MSK) {
		ret = ltm4700_read_byte(dev, channel, LTM4700_STATUS_TEMPERATURE,
					&status->temp);
		if (ret)
			return ret;
	}

	if (status_type & LTM4700_STATUS_CML_TYPE_MSK) {
		ret = ltm4700_read_byte(dev, 0, LTM4700_STATUS_CML, &status->cml);
		if (ret)
			return ret;
	}

	if (status_type & LTM4700_STATUS_MFR_SPECIFIC_TYPE_MSK) {
		ret = ltm4700_read_byte(dev, channel, LTM4700_STATUS_MFR_SPECIFIC,
					&status->mfr_specific);
	}

	return ret;
}

/**
 * @brief Set VOUT parameters
 *
 * @param dev - The device structure
 * @param channel - Channel number
 * @param vout_command - VOUT command value in microvolts
 * @param vout_max - VOUT max value in microvolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_vout_value(struct ltm4700_dev *dev, uint8_t channel,
		       int vout_command, int vout_max)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (channel >= dev->num_channels)
		return -EINVAL;

	ret = ltm4700_write_word_data(dev, channel, LTM4700_VOUT_COMMAND, vout_command);
	if (ret)
		return ret;

	return ltm4700_write_word_data(dev, channel, LTM4700_VOUT_MAX, vout_max);
}

/**
 * @brief Set VOUT margins
 *
 * @param dev - The device structure
 * @param channel - Channel number
 * @param margin_low - Low margin value in microvolts
 * @param margin_high - High margin value in microvolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_vout_margin(struct ltm4700_dev *dev, uint8_t channel,
			int margin_low, int margin_high)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (channel >= dev->num_channels)
		return -EINVAL;

	ret = ltm4700_write_word_data(dev, channel, LTM4700_VOUT_MARGIN_LOW,
				      margin_low);
	if (ret)
		return ret;

	return ltm4700_write_word_data(dev, channel, LTM4700_VOUT_MARGIN_HIGH,
				       margin_high);
}

/**
 * @brief Set timing values
 *
 * @param dev - The device structure
 * @param channel - Channel number
 * @param timing_type - Type of timing to set
 * @param time - Time value in microseconds
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_set_timing(struct ltm4700_dev *dev, uint8_t channel,
		       enum ltm4700_timing_type timing_type, int time)
{
	if (!dev)
		return -EINVAL;

	if (channel >= dev->num_channels)
		return -EINVAL;

	/* Convert microseconds to milliseconds for PMBus */
	return ltm4700_write_word_data(dev, channel, (uint8_t)timing_type, time / 1000);
}

/**
 * @brief Set operation mode
 *
 * @param dev - The device structure
 * @param channel - Channel number (0, 1, or LTM4700_CHAN_ALL for broadcast)
 * @param operation - Operation type
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_set_operation(struct ltm4700_dev *dev, uint8_t channel,
			  enum ltm4700_operation_type operation)
{
	if (!dev)
		return -EINVAL;

	if (channel != LTM4700_CHAN_ALL && channel >= dev->num_channels)
		return -EINVAL;

	return ltm4700_write_byte(dev, channel, LTM4700_OPERATION, (uint8_t)operation);
}

/**
 * @brief NVM/EEPROM user commands
 *
 * @param dev - The device structure
 * @param cmd - NVM command type
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_nvm_cmd(struct ltm4700_dev *dev, enum ltm4700_nvm_cmd_type cmd)
{
	if (!dev)
		return -EINVAL;

	return ltm4700_send_byte(dev, 0, (uint8_t)cmd);
}

/**
 * @brief Software reset
 *
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_software_reset(struct ltm4700_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ltm4700_send_byte(dev, 0, LTM4700_MFR_RESET);
}

/**
 * @brief Clear peak values
 *
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_clear_peaks(struct ltm4700_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ltm4700_send_byte(dev, 0, LTM4700_MFR_CLEAR_PEAKS);
}
