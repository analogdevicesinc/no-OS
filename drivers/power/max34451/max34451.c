/****************************************************************************//**
 *   @file   max34451.c
 *   @brief  Implementation of MAX34451 PMBus driver.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
 * ********************************************************************************
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#include <errno.h>
#include <string.h>

#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"

#include "max34451.h"

static const struct max34451_chip_info max34451_info[] = {
	[ID_MAX34451] = {
		.num_channels = 21,
	},
	[ID_MAX34451NA6] = {
		.num_channels = 21,
	},
	[ID_MAX34455] = {
		.num_channels = 17,
	},
	[ID_ADPM12160] = {
		.num_channels = 19,
	},
	[ID_ADPM12200] = {
		.num_channels = 19,
	},
	[ID_ADPM12250] = {
		.num_channels = 19,
	},
};

/**
 * @brief Divide two integers and round to the closest integer.
 * @param n Numerator.
 * @param d Denominator.
 * @return Result of the division rounded to the closest integer.
 */
static int64_t max34451_div_round_closest(int64_t n, int64_t d)
{
	if ((n < 0) == (d < 0))
		return (n + d / 2) / d;

	return (n - d / 2) / d;
}

/**
 * @brief Convert data to register value using direct format.
 * @param data Data value to convert.
 * @param reg Pointer to store register value.
 * @param m_num Numerator for multiplier.
 * @param m_den Denominator for multiplier.
 * @param b Offset.
 * @param R Exponent.
 * @param scale value scaling factor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max34451_data2reg_direct(int data,
				    uint16_t *reg, int m_num, int m_den,
				    int b, int R, int scale)
{
	int i, pow10 = 1;
	int64_t val;

	if (!reg)
		return -EINVAL;

	for (i = 0; i < R; i++)
		pow10 *= 10;

	val = max34451_div_round_closest((int64_t)m_num * data * pow10,
					 (int64_t)m_den * scale) + (int64_t)b * pow10;

	if (val > INT16_MAX || val < INT16_MIN)
		return -ERANGE;

	*reg = (uint16_t)val;
	return 0;
}

/**
 * @brief Convert register value to data using direct format.
 * @param reg Register value to convert.
 * @param data Pointer to store data value.
 * @param m_num Numerator for multiplier.
 * @param m_den Denominator for multiplier.
 * @param b Offset.
 * @param R Exponent.
 * @param scale value scaling factor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max34451_reg2data_direct(uint16_t reg,
				    int *data, int m_num, int m_den, int b, int R, int scale)
{
	int i, pow10 = 1;
	int64_t val;
	if (!data)
		return -EINVAL;

	for (i = 0; i < R; i++)
		pow10 *= 10;

	val = ((int64_t)(int16_t)reg - (int64_t)b * pow10) * m_den * scale;

	*data = max34451_div_round_closest(val, (int64_t)m_num * pow10);

	return 0;
}

/**
 * @brief Check if PMBus command is supported by the device variant.
 * @param dev Device structure pointer.
 * @param cmd PMBus command code to check.
 * @return 0 if command is supported, -EINVAL if not supported by this device variant.
 */
static int max34451_check_id(struct max34451_dev *dev, uint8_t cmd)
{
	switch (cmd) {
	case MAX34451NA6_STATUS_INPUT:
	case MAX34451NA6_READ_VIN:
	case MAX34451NA6_READ_IIN:
	case MAX34451NA6_MFR_CONFIG_VERSION:
	case MAX34451NA6_MFR_WP_CONTROL:
		if (dev->id == ID_MAX34451)
			return -EINVAL;
		break;
	case MAX34451_VOUT_SCALE_MONITOR:
	case MAX34451_IOUT_CAL_GAIN:
	case MAX34451_MFR_DATE:
	case MAX34451_MFR_PSEN_CONFIG:
	case MAX34451_MFR_FW_SERIAL:
	case MAX34451_MFR_CHANNEL_CONFIG:
	case MAX34451_MFR_TON_SEQ_MAX:
	case MAX34451_MFR_PWM_CONFIG:
	case MAX34451_MFR_SEQ_CONFIG:
	case MAX34451_MFR_TEMP_SENSOR_CONFIG:
		if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
		    || dev->id == ID_ADPM12250)
			return -EINVAL;
		break;
	default:
		break;
	}
	return 0;
}

/**
 * @brief Check if the channel is valid for the device variant.
 * @param dev Device structure pointer.
 * @param channel Channel number to check.
 * @return 0 if channel is valid, -EINVAL if not valid for this device variant.
 */
static int max34451_check_channel(struct max34451_dev *dev, uint8_t channel)
{
	if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
	    || dev->id == ID_ADPM12250) {
		if (channel == ADPM12XXX_CHAN_0 || channel == ADPM12XXX_CHAN_1
		    || channel == ADPM12XXX_CHAN_15)
			return -EINVAL;
	}
	return 0;
}

/**
 * @brief Convert data to register value for a given command.
 * @param dev Device structure pointer.
 * @param cmd PMBus command code.
 * @param data Data value to convert.
 * @param reg Pointer to store register value.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max34451_data2reg(struct max34451_dev *dev, uint8_t cmd, int data,
			     uint16_t *reg)
{
	switch (cmd) {
	/* Voltage values in mV */
	case MAX34451_VOUT_MARGIN_HIGH:
	case MAX34451_VOUT_MARGIN_LOW:
	case MAX34451_VOUT_OV_FAULT_LIMIT:
	case MAX34451_VOUT_OV_WARN_LIMIT:
	case MAX34451_VOUT_UV_WARN_LIMIT:
	case MAX34451_VOUT_UV_FAULT_LIMIT:
	case MAX34451_POWER_GOOD_ON:
	case MAX34451_POWER_GOOD_OFF:
	case MAX34451_READ_VOUT:
	case MAX34451_MFR_VOUT_PEAK:
	case MAX34451_MFR_VOUT_MIN:
	case MAX34451NA6_READ_VIN:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_data2reg_direct(data, reg, 1, 1, 0, 0, 1);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return max34451_data2reg_direct(data, reg, 1, 8, 0, 0, 1);
		break;
	/* Voltage scale monitor */
	case MAX34451_VOUT_SCALE_MONITOR:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_data2reg_direct(data, reg, 32767, 1, 0, 0, MICRO);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return -EINVAL;
		break;
	/* Current values in mA */
	case MAX34451_IOUT_OC_FAULT_LIMIT:
	case MAX34451_IOUT_OC_WARN_LIMIT:
	case MAX34451_READ_IOUT:
	case MAX34451_MFR_IOUT_PEAK:
	case MAX34451_MFR_IOUT_AVG:
	case MAX34451NA6_READ_IIN:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_data2reg_direct(data, reg, 1, 1, 0, 2, MILLI);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return max34451_data2reg_direct(data, reg, 1, 4, 0, 2, MILLI);
		break;
	/* Current scaling */
	case MAX34451_IOUT_CAL_GAIN:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_data2reg_direct(data, reg, 1, 1, 0, 1, MILLI);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return -EINVAL;
		break;
	/* Temperature values in degree mCelsius */
	case MAX34451_OT_FAULT_LIMIT:
	case MAX34451_OT_WARN_LIMIT:
	case MAX34451_READ_TEMPERATURE_1:
	case MAX34451_MFR_TEMPERATURE_PEAK:
		return max34451_data2reg_direct(data, reg, 1, 1, 0, 2, MILLI);
	/* Timing values in milliseconds */
	case MAX34451_TON_DELAY:
	case MAX34451_TON_MAX_FAULT_LIMIT:
	case MAX34451_TOFF_DELAY:
	case MAX34451_MFR_FAULT_RETRY:
	case MAX34451_MFR_TON_SEQ_MAX:
		return max34451_data2reg_direct(data, reg, 5, 1, 0, 0, MILLI);
	default:
		return -EINVAL;
	}

	return -EINVAL;
}

/**
 * @brief Convert register value to data for a given command.
 * @param dev Device structure pointer.
 * @param cmd PMBus command code.
 * @param reg Register value to convert.
 * @param data Pointer to store data value.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max34451_reg2data(struct max34451_dev *dev, uint8_t cmd,
			     uint16_t reg, int *data)
{
	switch (cmd) {
	/* Voltage values in mV */
	case MAX34451_VOUT_MARGIN_HIGH:
	case MAX34451_VOUT_MARGIN_LOW:
	case MAX34451_VOUT_OV_FAULT_LIMIT:
	case MAX34451_VOUT_OV_WARN_LIMIT:
	case MAX34451_VOUT_UV_WARN_LIMIT:
	case MAX34451_VOUT_UV_FAULT_LIMIT:
	case MAX34451_POWER_GOOD_ON:
	case MAX34451_POWER_GOOD_OFF:
	case MAX34451_READ_VOUT:
	case MAX34451_MFR_VOUT_PEAK:
	case MAX34451_MFR_VOUT_MIN:
	case MAX34451NA6_READ_VIN:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_reg2data_direct(reg, data, 1, 1, 0, 0, 1);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return max34451_reg2data_direct(reg, data, 1, 8, 0, 0, 1);
		break;
	/* Voltage scale monitor */
	case MAX34451_VOUT_SCALE_MONITOR:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_reg2data_direct(reg, data, 32767, 1, 0, 0, MICRO);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return -EINVAL;
		break;
	/* Current values in mA */
	case MAX34451NA6_READ_IIN:
	case MAX34451_IOUT_OC_FAULT_LIMIT:
	case MAX34451_IOUT_OC_WARN_LIMIT:
	case MAX34451_READ_IOUT:
	case MAX34451_MFR_IOUT_PEAK:
	case MAX34451_MFR_IOUT_AVG:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_reg2data_direct(reg, data, 1, 1, 0, 2, MILLI);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return max34451_reg2data_direct(reg, data, 1, 4, 0, 2, MILLI);
		break;
	/* Current scaling */
	case MAX34451_IOUT_CAL_GAIN:
		if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
		    || dev->id == ID_MAX34455)
			return max34451_reg2data_direct(reg, data, 1, 1, 0, 1, MILLI);
		else if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
			 || dev->id == ID_ADPM12250)
			return -EINVAL;
		break;
	/* Temperature values in degree mCelsius */
	case MAX34451_OT_FAULT_LIMIT:
	case MAX34451_OT_WARN_LIMIT:
	case MAX34451_READ_TEMPERATURE_1:
	case MAX34451_MFR_TEMPERATURE_PEAK:
		return max34451_reg2data_direct(reg, data, 1, 1, 0, 2, MILLI);
	/* Timing values in milliseconds */
	case MAX34451_TON_DELAY:
	case MAX34451_TON_MAX_FAULT_LIMIT:
	case MAX34451_TOFF_DELAY:
	case MAX34451_MFR_FAULT_RETRY:
	case MAX34451_MFR_TON_SEQ_MAX:
		return max34451_reg2data_direct(reg, data, 5, 1, 0, 0, MILLI);
	default:
		return -EINVAL;
	}

	return -EINVAL;
}

/**
 * @brief Initialize the MAX34451 device structure.
 * @param device Pointer to device structure pointer to initialize.
 * @param init_param Initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_init(struct max34451_dev **device,
		  struct max34451_init_param *init_param)
{
	struct max34451_dev *dev;
	int ret, i;

	dev = (struct max34451_dev *)no_os_calloc(1, sizeof(struct max34451_dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto i2c_err;

	dev->page = -1;
	dev->id = init_param->id;
	dev->num_channels = max34451_info[dev->id].num_channels;

	/* Initialize GPIO for ALERT */
	if (init_param->alert_param) {
		ret = no_os_gpio_get_optional(&dev->alert_desc,
					      init_param->alert_param);
		if (ret)
			goto dev_err;

		ret = no_os_gpio_direction_input(dev->alert_desc);
		if (ret)
			goto dev_err;
	}

	/* Initialize GPIO for PGOOD */
	if (init_param->pgood_param) {
		dev->pgood_descs = no_os_calloc(dev->num_channels, sizeof(*dev->pgood_descs));
		if (!dev->pgood_descs) {
			ret = -ENOMEM;
			goto dev_err;
		}

		for (i = 0; i < dev->num_channels; i++) {
			ret = no_os_gpio_get_optional(&dev->pgood_descs[i],
						      init_param->pgood_param[i]);
			if (ret)
				goto dev_err;

			ret = no_os_gpio_direction_input(dev->pgood_descs[i]);
			if (ret)
				goto dev_err;
		}
	}

	/* Initialize GPIO for RUN */
	if (init_param->run_param) {
		dev->run_descs = no_os_calloc(dev->num_channels, sizeof(*dev->run_descs));
		if (!dev->run_descs) {
			ret = -ENOMEM;
			goto dev_err;
		}

		for (i = 0; i < dev->num_channels; i++) {
			ret = no_os_gpio_get_optional(&dev->run_descs[i],
						      init_param->run_param[i]);
			if (ret)
				goto dev_err;

			ret = no_os_gpio_direction_output(dev->run_descs[i],
							  NO_OS_GPIO_HIGH);
			if (ret)
				goto dev_err;
		}
	}

	/* Initialize GPIO for FAULT */
	if (init_param->fault_param) {
		dev->fault_descs = no_os_calloc(dev->num_channels, sizeof(*dev->fault_descs));
		if (!dev->fault_descs) {
			ret = -ENOMEM;
			goto dev_err;
		}

		for (i = 0; i < dev->num_channels; i++) {
			ret = no_os_gpio_get_optional(&dev->fault_descs[i],
						      init_param->fault_param[i]);
			if (ret)
				goto dev_err;

			ret = no_os_gpio_direction_output(dev->fault_descs[i],
							  NO_OS_GPIO_HIGH);
			if (ret)
				goto dev_err;
		}
	}

	/* The part does not respond on the bus until the power-on load of
	 * MFR_STORE_SINGLE data completes. */
	no_os_mdelay(MAX34451_STARTUP_DELAY_MS);

	/* Clear faults */
	ret = max34451_send_byte(dev, MAX34451_CHAN_ALL, MAX34451_CLEAR_FAULTS);
	if (ret)
		goto dev_err;

	*device = dev;

	return 0;

dev_err:
	(void)max34451_remove(dev);
	return ret;
i2c_err:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free or remove a MAX34451 device instance.
 * @param dev Device structure pointer.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_remove(struct max34451_dev *dev)
{
	int ret, i;

	if (!dev)
		return -EINVAL;

	ret = no_os_i2c_remove(dev->i2c_desc);

	if (dev->pgood_descs) {
		for (i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->pgood_descs[i]);
		no_os_free(dev->pgood_descs);
	}
	if (dev->run_descs) {
		for (i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->run_descs[i]);
		no_os_free(dev->run_descs);
	}
	if (dev->fault_descs) {
		for (i = 0; i < dev->num_channels; i++)
			no_os_gpio_remove(dev->fault_descs[i]);
		no_os_free(dev->fault_descs);
	}
	no_os_gpio_remove(dev->alert_desc);
	no_os_free(dev);

	return ret;
}

/**
 * @brief Set PMBus page and phase.
 * @param dev Device structure pointer.
 * @param page Page number to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_set_page(struct max34451_dev *dev, int page)
{
	int ret = 0;
	uint8_t read_page;
	uint8_t tx_buf[3] = {0};

	if (!dev)
		return -EINVAL;

	if (page < 0 || page > MAX34451_CHAN_ALL)
		return -EINVAL;
	if (dev->page == page)
		return 0;

	tx_buf[0] = MAX34451_PAGE;
	tx_buf[1] = (uint8_t)page;

	dev->page = -1;

	ret = no_os_i2c_write(dev->i2c_desc, tx_buf,
			      2, 1);
	if (ret)
		return ret;

	ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, &read_page, 1, 1);
	if (ret)
		return ret;

	if (read_page != page)
		return -EIO;

	dev->page = page;

	return 0;
}

/**
 * @brief Send a PMBus command byte to the device.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to send.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_send_byte(struct max34451_dev *dev, int page, uint8_t cmd)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

	ret = max34451_check_id(dev, cmd);
	if (ret)
		return ret;

	ret = max34451_set_page(dev, page);
	if (ret)
		return ret;

	return no_os_i2c_write(dev->i2c_desc, &cmd, 1, 1);
}

/**
 * @brief Perform a PMBus read_byte operation.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to read.
 * @param data Pointer to store read data.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_read_byte(struct max34451_dev *dev, int page,
		       uint8_t cmd, uint8_t *data)
{
	int ret;

	if (!dev || !data)
		return -EINVAL;

	ret = max34451_check_id(dev, cmd);
	if (ret)
		return ret;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

	ret = max34451_set_page(dev, page);
	if (ret)
		return ret;

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, data, 1, 1);
}

/**
 * @brief Perform a PMBus write_byte operation.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to write.
 * @param value Value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_write_byte(struct max34451_dev *dev, int page,
			uint8_t cmd, uint8_t value)
{
	int ret;
	uint8_t tx_buf[3] = {0};

	if (!dev)
		return -EINVAL;

	ret = max34451_check_id(dev, cmd);
	if (ret)
		return ret;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

	ret = max34451_set_page(dev, page);
	if (ret)
		return ret;

	tx_buf[0] = cmd;
	tx_buf[1] = value;

	return no_os_i2c_write(dev->i2c_desc, tx_buf, 2, 1);
}

/**
 * @brief Perform a PMBus read_word operation.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to read.
 * @param word Pointer to store read word.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_read_word(struct max34451_dev *dev, int page,
		       uint8_t cmd, uint16_t *word)
{
	int ret;
	uint8_t rx_buf[3] = {0};

	if (!dev || !word)
		return -EINVAL;

	ret = max34451_check_id(dev, cmd);
	if (ret)
		return ret;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

	ret = max34451_set_page(dev, page);
	if (ret)
		return ret;

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
	if (ret)
		return ret;

	*word = no_os_get_unaligned_le16(rx_buf);
	return 0;
}

/**
 * @brief Perform a PMBus write_word operation.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to write.
 * @param word Word value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_write_word(struct max34451_dev *dev, int page,
			uint8_t cmd, uint16_t word)
{
	int ret;
	uint8_t tx_buf[4] = {0};

	if (!dev)
		return -EINVAL;

	ret = max34451_check_id(dev, cmd);
	if (ret)
		return ret;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

	ret = max34451_set_page(dev, page);
	if (ret)
		return ret;

	tx_buf[0] = cmd;
	no_os_put_unaligned_le16(word, &tx_buf[1]);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, 3, 1);
}

/**
 * @brief Handle the automatic switching of IOUT_OC_FAULT and IOUT_OC_WARN
 *        register addresses for different device IDs.
 * @param dev Device structure pointer.
 * @param cmd Command byte (either IOUT_OC_FAULT_LIMIT or IOUT_OC_WARN_LIMIT).
 * @return Corrected command byte based on the device ID.
 */
static int max34451_iout_oc_reg(struct max34451_dev *dev, uint8_t cmd)
{
	switch (cmd) {
	case MAX34451_IOUT_OC_FAULT_LIMIT:
		if (dev->id == ID_MAX34451)
			return MAX34451_IOUT_OC_FAULT_LIMIT;
		else
			return MAX34451_IOUT_OC_WARN_LIMIT;
		break;
	case MAX34451_IOUT_OC_WARN_LIMIT:
		if (dev->id == ID_MAX34451)
			return MAX34451_IOUT_OC_WARN_LIMIT;
		else
			return MAX34451_IOUT_OC_FAULT_LIMIT;
		break;
	default:
		return cmd;
	}
}

/**
 * @brief PMBus read word data with conversion to real values.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to read.
 * @param data Pointer to store converted data.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_read_word_data(struct max34451_dev *dev, int page,
			    uint8_t cmd, int *data)
{
	int ret;
	uint16_t reg;

	if (!dev || !data)
		return -EINVAL;

	/* Automatically swaps IOUT_OC_FAULT and WARN addresses for MAX34451 */
	ret = max34451_read_word(dev, page, max34451_iout_oc_reg(dev, cmd), &reg);
	if (ret)
		return ret;

	/* Conversion to real value */
	return max34451_reg2data(dev, cmd, reg, data);
}

/**
 * @brief PMBus write word data with conversion from real values.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to write.
 * @param data Data value to convert and write.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_write_word_data(struct max34451_dev *dev, int page,
			     uint8_t cmd, int data)
{
	int ret;
	uint16_t reg;

	if (!dev)
		return -EINVAL;

	ret = max34451_data2reg(dev, cmd, data, &reg);
	if (ret)
		return ret;

	/* Automatically swaps IOUT_OC_FAULT and WARN addresses for MAX34451 */
	return max34451_write_word(dev, page, max34451_iout_oc_reg(dev, cmd), reg);
}

/**
 * @brief Perform a PMBus read_block operation.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to read.
 * @param data Pointer to buffer for read data.
 * @param nbytes Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_read_block_data(struct max34451_dev *dev, int page, uint8_t cmd,
			     uint8_t *data, size_t nbytes)
{
	int ret;
	uint8_t rxbuf[MAX34451_MAX_BLOCK_LENGTH + 2] = {0};

	if (!dev || !data)
		return -EINVAL;

	if (nbytes > MAX34451_MAX_BLOCK_LENGTH)
		return -EMSGSIZE;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

	ret = max34451_check_id(dev, cmd);
	if (ret)
		return ret;

	ret = max34451_set_page(dev, page);
	if (ret)
		return ret;

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 1, 1);
	if (ret)
		return ret;

	if (rxbuf[0] != nbytes)
		return -EMSGSIZE;

	memcpy(data, &rxbuf[1], nbytes);

	return 0;
}

/**
 * @brief Perform a PMBus write_block operation.
 * @param dev Device structure pointer.
 * @param page Page number.
 * @param cmd Command byte to write.
 * @param data Pointer to data buffer to write.
 * @param nbytes Number of bytes to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_write_block_data(struct max34451_dev *dev, int page, uint8_t cmd,
			      uint8_t *data, size_t nbytes)
{
	int ret;
	uint8_t txbuf[MAX34451_MAX_BLOCK_LENGTH + 2];

	if (!dev || !data)
		return -EINVAL;

	if (nbytes > MAX34451_MAX_BLOCK_LENGTH)
		return -EMSGSIZE;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

	ret = max34451_check_id(dev, cmd);
	if (ret)
		return ret;

	ret = max34451_set_page(dev, page);
	if (ret)
		return ret;

	txbuf[0] = cmd;
	txbuf[1] = nbytes;
	memcpy(&txbuf[2], data, nbytes);

	return no_os_i2c_write(dev->i2c_desc, txbuf, nbytes + 2, 1);
}

/**
 * @brief Update a specific bit or bit field in a PMBus command.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param cmd PMBus command to update.
 * @param mask Bit mask to modify (e.g., NO_OS_BIT(5) or NO_OS_GENMASK(31,16)).
 * @param value Value to set in the bit field (will be masked and shifted automatically).
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_update_bit(struct max34451_dev *dev, uint8_t channel, uint8_t cmd,
			uint32_t mask, uint32_t value)
{
	int ret;
	uint32_t temp_val;
	uint8_t block[4] = {0};
	uint16_t word_val;
	uint8_t byte_val;

	if (!dev)
		return -EINVAL;

	switch (cmd) {
	case MAX34451_ON_OFF_CONFIG:
		ret = max34451_read_byte(dev, channel, cmd, &byte_val);
		if (ret)
			return ret;

		temp_val = byte_val;
		temp_val &= ~mask;
		temp_val |= no_os_field_prep(mask, value);

		return max34451_write_byte(dev, channel, cmd, (uint8_t)temp_val);
	case MAX34451_MFR_MODE:
	case MAX34451_MFR_NV_LOG_CONFIG:
	case MAX34451_MFR_MARGIN_CONFIG:
	case MAX34451_MFR_CHANNEL_CONFIG:
	case MAX34451_MFR_TEMP_SENSOR_CONFIG:
		ret = max34451_read_word(dev, channel, cmd, &word_val);
		if (ret)
			return ret;

		temp_val = word_val;
		temp_val &= ~mask;
		temp_val |= no_os_field_prep(mask, value);

		return max34451_write_word(dev, channel, cmd, (uint16_t)temp_val);
	case MAX34451_MFR_PSEN_CONFIG:
	case MAX34451_MFR_FAULT_RESPONSE:
	case MAX34451_MFR_PWM_CONFIG:
	case MAX34451_MFR_SEQ_CONFIG:
		ret = max34451_read_block_data(dev, channel, cmd,
					       &block[0], 4);
		if (ret)
			return ret;

		temp_val = no_os_get_unaligned_le32(block);
		temp_val &= ~mask;
		temp_val |= no_os_field_prep(mask, value);
		no_os_put_unaligned_le32(temp_val, block);

		return max34451_write_block_data(dev, channel, cmd, block, 4);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read a specific bit or bit field from a PMBus command.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param cmd PMBus command to read from.
 * @param mask Bit mask to extract (e.g., NO_OS_BIT(5) or NO_OS_GENMASK(31,16)).
 * @param value Pointer to store the extracted bit/field value.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_read_bit(struct max34451_dev *dev, uint8_t channel, uint8_t cmd,
		      uint32_t mask, uint32_t *value)
{
	int ret;
	uint32_t block_val;
	uint8_t block[4] = {0};
	uint16_t word_val;
	uint8_t byte_val;

	if (!dev || !value)
		return -EINVAL;

	switch (cmd) {
	case MAX34451_ON_OFF_CONFIG:
	case MAX34451_CAPABILITY:
	case MAX34451_STATUS_VOUT:
	case MAX34451_STATUS_IOUT:
	case MAX34451NA6_STATUS_INPUT:
	case MAX34451_STATUS_TEMPERATURE:
	case MAX34451_STATUS_CML:
	case MAX34451_STATUS_MFR_SPECIFIC:
		ret = max34451_read_byte(dev, channel, cmd, &byte_val);
		if (ret)
			return ret;

		*value = no_os_field_get(mask, byte_val);
		return 0;
	case MAX34451_STATUS_WORD:
	case MAX34451_MFR_MODE:
	case MAX34451_MFR_NV_LOG_CONFIG:
	case MAX34451_MFR_MARGIN_CONFIG:
	case MAX34451_MFR_CHANNEL_CONFIG:
	case MAX34451_MFR_TEMP_SENSOR_CONFIG:
		ret = max34451_read_word(dev, channel, cmd, &word_val);
		if (ret)
			return ret;

		*value = no_os_field_get(mask, word_val);
		return 0;
	case MAX34451_MFR_PSEN_CONFIG:
	case MAX34451_MFR_FAULT_RESPONSE:
	case MAX34451_MFR_PWM_CONFIG:
	case MAX34451_MFR_SEQ_CONFIG:
		ret = max34451_read_block_data(dev, channel, cmd,
					       &block[0], 4);
		if (ret)
			return ret;

		block_val = no_os_get_unaligned_le32(block);
		*value = no_os_field_get(mask, block_val);
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read a specific value type from the device.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param value_type Type of value to read.
 * @param value Pointer to store the read value.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_read_value(struct max34451_dev *dev, uint8_t channel,
			enum max34451_value_type value_type, int *value)
{
	if (!dev || !value)
		return -EINVAL;

	return max34451_read_word_data(dev, channel,
				       (uint8_t)value_type, value);
}

/**
 * @brief Read status from the device.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param status_type Type of status to read.
 * @param status Pointer to status structure to fill.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_read_status(struct max34451_dev *dev,
			 uint8_t channel,
			 enum max34451_status_type status_type,
			 struct max34451_status *status)
{
	int ret;

	if (!dev || !status)
		return -EINVAL;

	if (status_type & MAX34451_STATUS_WORD_TYPE) {
		ret = max34451_read_word(dev, channel,
					 MAX34451_STATUS_WORD, &status->word);
		if (ret)
			return ret;
	}

	if (status_type & MAX34451_STATUS_VOUT_TYPE) {
		ret = max34451_read_byte(dev, channel,
					 MAX34451_STATUS_VOUT, &status->vout);
		if (ret)
			return ret;
	}

	if (status_type & MAX34451_STATUS_IOUT_TYPE) {
		ret = max34451_read_byte(dev, channel,
					 MAX34451_STATUS_IOUT, &status->iout);
		if (ret)
			return ret;
	}

	if (status_type & MAX34451_STATUS_TEMP_TYPE) {
		ret = max34451_read_byte(dev, channel,
					 MAX34451_STATUS_TEMPERATURE, &status->temp);
		if (ret)
			return ret;
	}

	if (status_type & MAX34451_STATUS_CML_TYPE) {
		ret = max34451_read_byte(dev, channel,
					 MAX34451_STATUS_CML, &status->cml);
		if (ret)
			return ret;
	}

	if (status_type & MAX34451_STATUS_MFR_SPECIFIC_TYPE) {
		ret = max34451_read_byte(dev, channel,
					 MAX34451_STATUS_MFR_SPECIFIC, &status->mfr_specific);
		if (ret)
			return ret;
	}

	if (status_type & MAX34451NA6_STATUS_INPUT_TYPE) {
		ret = max34451_read_byte(dev, channel,
					 MAX34451NA6_STATUS_INPUT, &status->input);
		if (ret)
			return ret;
	}
	return 0;
}

/**
 * @brief Set VOUT margin values for a channel.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param margin_low Margin low value.
 * @param margin_high Margin high value.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_vout_margin(struct max34451_dev *dev, uint8_t channel,
			 int margin_low, int margin_high)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (margin_high < margin_low || margin_low < 0 || margin_high < 0)
		return -EINVAL;

	ret = max34451_write_word_data(dev, channel, MAX34451_VOUT_MARGIN_HIGH,
				       margin_high);

	if (ret)
		return ret;

	return max34451_write_word_data(dev, channel, MAX34451_VOUT_MARGIN_LOW,
					margin_low);
}

/**
 * @brief Clear faults on the device.
 * @param dev Device structure pointer.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_clear_faults(struct max34451_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return max34451_send_byte(dev, MAX34451_CHAN_ALL, MAX34451_CLEAR_FAULTS);
}

/**
 * @brief Set timing parameters (TON_DELAY, TOFF_DELAY, etc.) in ms.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param timing_type Timing parameter type.
 * @param timing Timing value in ms.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_set_timing(struct max34451_dev *dev, uint8_t channel,
			enum max34451_timing_type timing_type, int timing)
{
	if (!dev)
		return -EINVAL;

	return max34451_write_word_data(dev, channel, timing_type, timing);
}

/**
 * @brief Set the operation mode for a channel.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param operation Operation type to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_set_operation(struct max34451_dev *dev, uint8_t channel,
			   enum max34451_operation_type operation)
{
	if (!dev)
		return -EINVAL;

	return max34451_write_byte(dev, channel, MAX34451_OPERATION,
				   (uint8_t)operation);
}


/**
 * @brief Perform a software reset of the device.
 * @param dev Device structure pointer.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_software_reset(struct max34451_dev *dev)
{
	uint16_t mode;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = max34451_read_word(dev, MAX34451_CHAN_ALL, MAX34451_MFR_MODE, &mode);
	if (ret)
		return ret;

	/* SOFT_RESET must be set, cleared, then set again within 8 ms; a single
	 * write is ignored. MFR_MODE is read once beforehand so that no read
	 * turnaround lands between the three writes. */
	ret = max34451_write_word(dev, MAX34451_CHAN_ALL, MAX34451_MFR_MODE,
				  mode | MAX34451_MFR_MODE_SOFT_RESET);
	if (ret)
		return ret;

	ret = max34451_write_word(dev, MAX34451_CHAN_ALL, MAX34451_MFR_MODE,
				  mode & ~MAX34451_MFR_MODE_SOFT_RESET);
	if (ret)
		return ret;

	ret = max34451_write_word(dev, MAX34451_CHAN_ALL, MAX34451_MFR_MODE,
				  mode | MAX34451_MFR_MODE_SOFT_RESET);
	if (ret)
		return ret;

	/* The reset returns PAGE to its power-on value. */
	dev->page = -1;

	no_os_mdelay(MAX34451_STARTUP_DELAY_MS);

	return 0;
}
