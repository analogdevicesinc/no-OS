/***************************************************************************//**
 *   @file   iio_max34451.c
 *   @brief  Source file for the MAX34451 IIO Driver
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

/* DIRECT format coefficients for data conversion */
struct max34451_direct_coeff {
	int m_num;	/* Slope coefficient numerator */
	int m_den;	/* Slope coefficient denominator */
	int b;	/* Offset coefficient */
	int R;	/* Exponent */
	int scale;	/* Scale coefficient to convert to milliunit value */
};

/* Referenced from respective device datasheets */
static const struct max34451_direct_coeff MAX34451_COEFF_VOLTAGE_MV = {
	.m_num = 1,
	.m_den = 1,
	.b = 0,
	.R = 0,
	.scale = 1,
};

static const struct max34451_direct_coeff ADPM12XXX_COEFF_VOLTAGE_MV = {
	.m_num = 1,
	.m_den = 8,
	.b = 0,
	.R = 0,
	.scale = 1,
};

static const struct max34451_direct_coeff MAX34451_COEFF_VOUT_SCALE = {
	.m_num = INT16_MAX,
	.m_den = 1,
	.b = 0,
	.R = 0,
	.scale = 1000,	/* unitless, returned in milli-units */
};

static const struct max34451_direct_coeff MAX34451_COEFF_CURRENT_MA = {
	.m_num = 1,
	.m_den = 1,
	.b = 0,
	.R = 2,
	.scale = 1000,
};

static const struct max34451_direct_coeff ADPM12XXX_COEFF_CURRENT_MA = {
	.m_num = 1,
	.m_den = 4,
	.b = 0,
	.R = 2,
	.scale = 1000,
};

static const struct max34451_direct_coeff MAX34451_COEFF_IOUT_CAL_GAIN = {
	.m_num = 1,
	.m_den = 1,
	.b = 0,
	.R = 1,
	.scale = 1000,	/* mOhm, returned in uOhm */
};

static const struct max34451_direct_coeff MAX34451_COEFF_TEMPERATURE_MC = {
	.m_num = 1,
	.m_den = 1,
	.b = 0,
	.R = 2,
	.scale = 1000,
};

static const struct max34451_direct_coeff MAX34451_COEFF_TIMING_MS = {
	.m_num = 5,
	.m_den = 1,
	.b = 0,
	.R = 0,
	.scale = 1,
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
 * @param coeff Direct format coefficients to apply.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max34451_data2reg_direct(int data, uint16_t *reg,
				    const struct max34451_direct_coeff *coeff)
{
	int i, pow10 = 1;
	int64_t val;

	if (!reg)
		return -EINVAL;

	for (i = 0; i < coeff->R; i++)
		pow10 *= 10;

	val = max34451_div_round_closest((int64_t)coeff->m_num * data * pow10,
					 (int64_t)coeff->m_den * coeff->scale) +
	      (int64_t)coeff->b * pow10;

	if (val > INT16_MAX || val < INT16_MIN)
		return -ERANGE;

	*reg = (uint16_t)val;
	return 0;
}

/**
 * @brief Convert register value to data using direct format.
 * @param reg Register value to convert.
 * @param data Pointer to store data value.
 * @param coeff Direct format coefficients to apply.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max34451_reg2data_direct(uint16_t reg, int *data,
				    const struct max34451_direct_coeff *coeff)
{
	int16_t Y = (int16_t)reg;  /* Sign-extend to handle 2's complement */
	int i, pow10 = 1;
	int64_t val;

	if (!data)
		return -EINVAL;

	for (i = 0; i < coeff->R; i++)
		pow10 *= 10;

	val = ((int64_t)Y - ((int64_t)coeff->b * pow10)) * (int64_t)coeff->m_den *
	      (int64_t)coeff->scale;
	*data = (int)max34451_div_round_closest(val, (int64_t)coeff->m_num * pow10);

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
static int max34451_check_channel(struct max34451_dev *dev, int channel)
{
	if (channel != MAX34451_CHAN_ALL && channel >= dev->num_channels)
		return -EINVAL;

	if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
	    || dev->id == ID_ADPM12250) {
		if (channel == ADPM12XXX_CHAN_0 || channel == ADPM12XXX_CHAN_1
		    || channel == ADPM12XXX_CHAN_15)
			return -EINVAL;
	}
	return 0;
}

/**
 * @brief Get the direct format coefficients for a command and device variant.
 * @param dev Device structure pointer.
 * @param cmd PMBus command code.
 * @return Pointer to the coefficients, NULL if the command is not a direct
 *         format command on this device variant.
 */
static const struct max34451_direct_coeff *max34451_get_coeff(
	struct max34451_dev *dev,
	uint8_t cmd)
{
	const struct max34451_direct_coeff *max344xx;
	const struct max34451_direct_coeff *adpm12xxx;

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
		max344xx = &MAX34451_COEFF_VOLTAGE_MV;
		adpm12xxx = &ADPM12XXX_COEFF_VOLTAGE_MV;
		break;
	/* Voltage scale monitor */
	case MAX34451_VOUT_SCALE_MONITOR:
		max344xx = &MAX34451_COEFF_VOUT_SCALE;
		adpm12xxx = NULL;
		break;
	/* Current values in mA */
	case MAX34451_IOUT_OC_FAULT_LIMIT:
	case MAX34451_IOUT_OC_WARN_LIMIT:
	case MAX34451_READ_IOUT:
	case MAX34451_MFR_IOUT_PEAK:
	case MAX34451_MFR_IOUT_AVG:
	case MAX34451NA6_READ_IIN:
		max344xx = &MAX34451_COEFF_CURRENT_MA;
		adpm12xxx = &ADPM12XXX_COEFF_CURRENT_MA;
		break;
	/* Current scaling */
	case MAX34451_IOUT_CAL_GAIN:
		max344xx = &MAX34451_COEFF_IOUT_CAL_GAIN;
		adpm12xxx = NULL;
		break;
	/* Temperature values in m°C */
	case MAX34451_OT_FAULT_LIMIT:
	case MAX34451_OT_WARN_LIMIT:
	case MAX34451_READ_TEMPERATURE_1:
	case MAX34451_MFR_TEMPERATURE_PEAK:
		max344xx = &MAX34451_COEFF_TEMPERATURE_MC;
		adpm12xxx = &MAX34451_COEFF_TEMPERATURE_MC;
		break;
	/* Timing values in milliseconds */
	case MAX34451_TON_DELAY:
	case MAX34451_TON_MAX_FAULT_LIMIT:
	case MAX34451_TOFF_DELAY:
	case MAX34451_MFR_FAULT_RETRY:
	case MAX34451_MFR_TON_SEQ_MAX:
		max344xx = &MAX34451_COEFF_TIMING_MS;
		adpm12xxx = &MAX34451_COEFF_TIMING_MS;
		break;
	default:
		return NULL;
	}

	switch (dev->id) {
	case ID_MAX34451:
	case ID_MAX34451NA6:
	case ID_MAX34455:
		return max344xx;
	case ID_ADPM12160:
	case ID_ADPM12200:
	case ID_ADPM12250:
		return adpm12xxx;
	default:
		return NULL;
	}
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
	const struct max34451_direct_coeff *coeff;

	coeff = max34451_get_coeff(dev, cmd);
	if (!coeff)
		return -EINVAL;

	return max34451_data2reg_direct(data, reg, coeff);
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
	const struct max34451_direct_coeff *coeff;

	coeff = max34451_get_coeff(dev, cmd);
	if (!coeff)
		return -EINVAL;

	return max34451_reg2data_direct(reg, data, coeff);
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

	if (!device || !init_param)
		return -EINVAL;

	if (init_param->id >= NO_OS_ARRAY_SIZE(max34451_info))
		return -EINVAL;

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
	ret = no_os_gpio_get_optional(&dev->alert_desc, init_param->alert_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->alert_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for RST, kept deasserted */
	ret = no_os_gpio_get_optional(&dev->rst_desc, init_param->rst_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_output(dev->rst_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for CONTROL0/CONTROL1 */
	for (i = 0; i < MAX34451_NUM_CONTROL_PINS; i++) {
		ret = no_os_gpio_get_optional(&dev->control_descs[i],
					      init_param->control_param[i]);
		if (ret)
			goto dev_err;

		ret = no_os_gpio_direction_output(dev->control_descs[i],
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto dev_err;
	}

	/* Initialize GPIO for FAULT0..FAULT2. These are open-drain lines the
	 * device pulls low, so the host only monitors them. */
	for (i = 0; i < MAX34451_NUM_FAULT_PINS; i++) {
		ret = no_os_gpio_get_optional(&dev->fault_descs[i],
					      init_param->fault_param[i]);
		if (ret)
			goto dev_err;

		ret = no_os_gpio_direction_input(dev->fault_descs[i]);
		if (ret)
			goto dev_err;
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

	for (i = 0; i < MAX34451_NUM_FAULT_PINS; i++)
		no_os_gpio_remove(dev->fault_descs[i]);
	for (i = 0; i < MAX34451_NUM_CONTROL_PINS; i++)
		no_os_gpio_remove(dev->control_descs[i]);
	no_os_gpio_remove(dev->rst_desc);
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
	uint8_t tx_buf[2] = {0};

	if (!dev)
		return -EINVAL;

	if (page < 0 || page > MAX34451_CHAN_ALL)
		return -EINVAL;
	if (dev->page == page)
		return 0;

	ret = max34451_check_channel(dev, page);
	if (ret)
		return ret;

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
	uint8_t tx_buf[2] = {0};

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
 * @brief Handle the automatic switching of IOUT_OC_FAULT and IOUT_OC_WARN
 *        register addresses for ID_MAX34451.
 * @param dev Device structure pointer.
 * @param cmd Command byte to send.
 * @return cmd, with IOUT_OC_FAULT and IOUT_OC_WARN swapped for ID_MAX34451.
 */
static uint8_t max34451_iout_oc_reg(struct max34451_dev *dev, uint8_t cmd)
{
	if (dev->id != ID_MAX34451)
		return cmd;

	switch (cmd) {
	case MAX34451_IOUT_OC_FAULT_LIMIT:
		return MAX34451_IOUT_OC_WARN_LIMIT;
	case MAX34451_IOUT_OC_WARN_LIMIT:
		return MAX34451_IOUT_OC_FAULT_LIMIT;
	default:
		return cmd;
	}
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

	cmd = max34451_iout_oc_reg(dev, cmd);

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

	tx_buf[0] = max34451_iout_oc_reg(dev, cmd);
	no_os_put_unaligned_le16(word, &tx_buf[1]);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, 3, 1);
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

	ret = max34451_read_word(dev, page, cmd, &reg);
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

	return max34451_write_word(dev, page, cmd, reg);
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
 * @brief Update a specific bit or bit field in a byte-wide PMBus command.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param cmd PMBus command to update.
 * @param mask Bit mask to modify (e.g., NO_OS_BIT(5) or NO_OS_GENMASK(3,0)).
 * @param value Value to set in the bit field (will be masked and shifted automatically).
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_update_byte_bit(struct max34451_dev *dev, uint8_t channel,
			     uint8_t cmd,
			     uint8_t mask, uint8_t value)
{
	int ret;
	uint8_t byte_val;

	if (!dev)
		return -EINVAL;

	ret = max34451_read_byte(dev, channel, cmd, &byte_val);
	if (ret)
		return ret;

	byte_val &= ~mask;
	byte_val |= no_os_field_prep(mask, value);

	return max34451_write_byte(dev, channel, cmd, byte_val);
}

/**
 * @brief Update a specific bit or bit field in a word-wide PMBus command.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param cmd PMBus command to update.
 * @param mask Bit mask to modify (e.g., NO_OS_BIT(5) or NO_OS_GENMASK(15,8)).
 * @param value Value to set in the bit field (will be masked and shifted automatically).
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_update_word_bit(struct max34451_dev *dev, uint8_t channel,
			     uint8_t cmd,
			     uint16_t mask, uint16_t value)
{
	int ret;
	uint16_t word_val;

	if (!dev)
		return -EINVAL;

	ret = max34451_read_word(dev, channel, cmd, &word_val);
	if (ret)
		return ret;

	word_val &= ~mask;
	word_val |= no_os_field_prep(mask, value);

	return max34451_write_word(dev, channel, cmd, word_val);
}

/**
 * @brief Update a specific bit or bit field in a 4-byte block PMBus command.
 * @param dev Device structure pointer.
 * @param channel Channel number.
 * @param cmd PMBus command to update.
 * @param mask Bit mask to modify (e.g., NO_OS_BIT(5) or NO_OS_GENMASK(31,16)).
 * @param value Value to set in the bit field (will be masked and shifted automatically).
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_update_block_bit(struct max34451_dev *dev, uint8_t channel,
			      uint8_t cmd, uint32_t mask, uint32_t value)
{
	int ret;
	uint32_t block_val;
	uint8_t block[4] = {0};

	if (!dev)
		return -EINVAL;

	ret = max34451_read_block_data(dev, channel, cmd, block, 4);
	if (ret)
		return ret;

	block_val = no_os_get_unaligned_le32(block);
	block_val &= ~mask;
	block_val |= no_os_field_prep(mask, value);
	no_os_put_unaligned_le32(block_val, block);

	return max34451_write_block_data(dev, channel, cmd, block, 4);
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
	if (!dev)
		return -EINVAL;

	switch (cmd) {
	case MAX34451_ON_OFF_CONFIG:
		return max34451_update_byte_bit(dev, channel, cmd, (uint8_t)mask,
						(uint8_t)value);
	case MAX34451_MFR_MODE:
	case MAX34451_MFR_NV_LOG_CONFIG:
	case MAX34451_MFR_MARGIN_CONFIG:
	case MAX34451_MFR_CHANNEL_CONFIG:
	case MAX34451_MFR_TEMP_SENSOR_CONFIG:
		return max34451_update_word_bit(dev, channel, cmd, (uint16_t)mask,
						(uint16_t)value);
	case MAX34451_MFR_PSEN_CONFIG:
	case MAX34451_MFR_FAULT_RESPONSE:
	case MAX34451_MFR_PWM_CONFIG:
	case MAX34451_MFR_SEQ_CONFIG:
		return max34451_update_block_bit(dev, channel, cmd, mask, value);
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
 * @brief Get the register-to-data scale of a command as a fraction.
 *
 * Returns the factor that converts a raw register value into the driver API
 * unit (mV, mA, mC, ms; milli-units for VOUT_SCALE_MONITOR, uOhm for
 * IOUT_CAL_GAIN), expressed as num / den so the caller keeps full precision.
 * Derived from the direct data format coefficients, so no bus access is
 * performed.
 *
 * @param dev Device structure pointer.
 * @param cmd PMBus command code.
 * @param num Pointer to store the scale numerator.
 * @param den Pointer to store the scale denominator.
 * @return 0 in case of success, negative error code otherwise.
 */
int max34451_get_scale(struct max34451_dev *dev, uint8_t cmd, int32_t *num,
		       int32_t *den)
{
	const struct max34451_direct_coeff *coeff;
	int i, pow10 = 1;

	if (!dev || !num || !den)
		return -EINVAL;

	coeff = max34451_get_coeff(dev, cmd);
	if (!coeff)
		return -EINVAL;

	for (i = 0; i < coeff->R; i++)
		pow10 *= 10;

	/* coeff->scale is the real-world-to-API unit factor, not this scale */
	*num = coeff->m_den * coeff->scale;
	*den = coeff->m_num * pow10;

	return 0;
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

	if (margin_high == margin_low || margin_high < margin_low || margin_low < 0
	    || margin_high < 0)
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
