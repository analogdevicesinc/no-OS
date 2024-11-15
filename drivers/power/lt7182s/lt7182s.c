/*******************************************************************************
*   @file   lt7182s.c
*   @brief  Source code of the LT7182S Driver
*   @authors Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_pwm.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_crc8.h"

#include "lt7182s.h"

NO_OS_DECLARE_CRC8_TABLE(lt7182s_crc_table);

static const struct lt7182s_chip_info lt7182s_info[] = {
	[ID_LT7182S] = {
		.name = "LT7182S",
	},
};

/**
 * @brief Check if PMBus command is paged
 *
 * @param cmd - PMBus command
 * @return true if paged, false otherwise
 */
static bool lt7182s_cmd_is_paged(uint8_t cmd)
{
	switch (cmd) {
	case LT7182S_OPERATION:
	case LT7182S_ON_OFF_CONFIG:
	case LT7182S_ZONE_CONFIG:
	case LT7182S_SMBALERT_MASK:
	case LT7182S_VOUT_MODE:
	case LT7182S_VOUT_COMMAND:
	case LT7182S_VOUT_MAX:
	case LT7182S_VOUT_MARGIN_HIGH:
	case LT7182S_VOUT_MARGIN_LOW:
	case LT7182S_VOUT_TRANSITION_RATE:
	case LT7182S_VIN_ON:
	case LT7182S_VIN_OFF:
	case LT7182S_VOUT_OV_FAULT_LIMIT:
	case LT7182S_VOUT_OV_FAULT_RESPONSE:
	case LT7182S_VOUT_OV_WARN_LIMIT:
	case LT7182S_VOUT_UV_WARN_LIMIT:
	case LT7182S_VOUT_UV_FAULT_LIMIT:
	case LT7182S_VOUT_UV_FAULT_RESPONSE:
	case LT7182S_IOUT_OC_FAULT_RESPONSE:
	case LT7182S_IOUT_OC_WARN_LIMIT:
	case LT7182S_VIN_OV_FAULT_RESPONSE:
	case LT7182S_VIN_UV_WARN_LIMIT:
	case LT7182S_IIN_OC_WARN_LIMIT:
	case LT7182S_TON_DELAY:
	case LT7182S_TON_RISE:
	case LT7182S_TON_MAX_FAULT_LIMIT:
	case LT7182S_TON_MAX_FAULT_RESPONSE:
	case LT7182S_TOFF_DELAY:
	case LT7182S_TOFF_FALL:
	case LT7182S_TOFF_MAX_WARN_LIMIT:
	case LT7182S_STATUS_BYTE:
	case LT7182S_STATUS_WORD:
	case LT7182S_STATUS_VOUT:
	case LT7182S_STATUS_IOUT:
	case LT7182S_STATUS_INPUT:
	case LT7182S_STATUS_MFR_SPECIFIC:
	case LT7182S_READ_VIN:
	case LT7182S_READ_IIN:
	case LT7182S_READ_VOUT:
	case LT7182S_READ_IOUT:
	case LT7182S_READ_FREQUENCY:
	case LT7182S_READ_POUT:
	case LT7182S_MFR_READ_ITH:
	case LT7182S_MFR_CHAN_CONFIG:
	case LT7182S_MFR_FAULT_PROPAGATE:
	case LT7182S_MFR_PWM_MODE:
	case LT7182S_MFR_FAULT_RESPONSE:
	case LT7182S_MFR_IOUT_PEAK:
	case LT7182S_MFR_RETRY_DELAY:
	case LT7182S_MFR_RESTART_DELAY:
	case LT7182S_MFR_VOUT_PEAK:
	case LT7182S_MFR_VIN_PEAK:
	case LT7182S_MFR_DISCHARGE_THRESHOLD:
	case LT7182S_MFR_CHANNEL_STATE:
	case LT7182S_MFR_PGOOD_DELAY:
	case LT7182S_MFR_NOT_PGOOD_DELAY:
	case LT7182S_MFR_PWM_PHASE:
	case LT7182S_MFR_RAIL_ADDRESS:
		return true;
	default:
		return false;
	}
}

/**
 * @brief Converts value to LINEAR16 register data
 *
 * @param dev - Device structure
 * @param data - Value to convert
 * @param reg - Address of converted register data
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_data2reg_linear16(struct lt7182s_dev *dev, int data,
				     uint16_t *reg, int scale)
{
	if (data <= 0)
		return -EINVAL;
	data <<= -(dev->lin16_exp);

	data = NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale);
	*reg = (uint16_t)no_os_clamp(data, 0, 0xFFFF);

	return 0;
}

/**
 * @brief Converts value to LINEAR11 register data
 *
 * @param dev - Device structure
 * @param data - Value to convert
 * @param reg - Address of converted register data
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_data2reg_linear11(struct lt7182s_dev *dev, int data,
				     uint16_t *reg, int scale)
{
	int exp = 0, mant = 0;
	uint8_t negative = 0;

	if (data < 0) {
		negative = 1;
		data = -data;
	}

	/* If value too high, continuously do m/2 until m < 1023. */
	while (data >= LT7182S_LIN11_MANTISSA_MAX * scale &&
	       exp < LT7182S_LIN11_EXPONENT_MAX) {
		exp++;
		data >>= 1;
	}

	/* If value too low, increase mantissa. */
	while (data < LT7182S_LIN11_MANTISSA_MIN * scale &&
	       exp > LT7182S_LIN11_EXPONENT_MIN) {
		exp--;
		data <<= 1;
	}

	mant = no_os_clamp(NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale),
			   0, 0x3FF);
	if (negative)
		mant = -mant;

	*reg = no_os_field_prep(LT7182S_LIN11_MANTISSA_MSK, mant) |
	       no_os_field_prep(LT7182S_LIN11_EXPONENT_MSK, exp);

	return 0;
}

/**
 * @brief Converts value to IEEE754 register data
 *
 * @param dev - Device structure
 * @param data - Value to convert
 * @param reg - Address of converted register data
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_data2reg_ieee754(struct lt7182s_dev *dev, int data,
				    uint16_t *reg, int scale)
{
	uint16_t exponent = (15 + 10);
	uint16_t sign = 0;
	int mantissa;

	/* simple case */
	if (data == 0)
		return 0;

	if (data < 0) {
		sign = 1;
		data = -data;
	}

	if (scale > (int)MILLI)
		data = NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale/MILLI);
	else
		data *= (int)MILLI / scale;

	/* Reduce large mantissa until it fits into 10 bit */
	while ((data > LT7182S_IEEE754_MAX_MANTISSA * scale) && exponent < 30) {
		exponent++;
		data >>= 1;
	}
	/*
	 * Increase small mantissa to generate valid 'normal'
	 * number
	 */
	while ((data < LT7182S_IEEE754_MIN_MANTISSA * scale) && exponent > 1) {
		exponent--;
		data <<= 1;
	}

	/* Convert mantissa from scaled-units to units */
	mantissa = NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale);

	mantissa = no_os_clamp(mantissa, LT7182S_IEEE754_MIN_MANTISSA,
			       LT7182S_IEEE754_MAX_MANTISSA);

	/* Convert to sign, 5 bit exponent, 10 bit mantissa */
	*reg = no_os_field_prep(LT7182S_IEEE754_SIGN_BIT, sign) |
	       no_os_field_prep(LT7182S_IEEE754_MANTISSA_MSK, mantissa) |
	       no_os_field_prep(LT7182S_IEEE754_EXPONENT_MSK, exponent);

	return 0;
}

/**
 * @brief Converts raw LINEAR16 register data to its actual value
 *
 * @param dev - Device structure
 * @param reg - LINEAR16 data to convert
 * @param data - Address of value
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_reg2data_linear16(struct lt7182s_dev *dev, uint16_t reg,
				     int *data, int scale)
{
	int exp = dev->lin16_exp;
	if (exp < 0)
		exp = -exp;

	*data = ((int)(reg) * scale) >> exp;

	return 0;
}

/**
 * @brief Converts raw LINEAR11 register data to its actual value
 *
 * @param dev - Device structure
 * @param reg - LINEAR11 data to convert
 * @param data - Address of value
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_reg2data_linear11(struct lt7182s_dev *dev, uint16_t reg,
				     int *data, int scale)
{
	int val, exp, mant;

	exp = LT7182S_LIN11_EXPONENT(reg);
	mant = LT7182S_LIN11_MANTISSA(reg);

	val = mant * scale;
	if (exp >= 0)
		*data = val << exp;
	else
		*data = val >> -exp;

	return 0;
}

/**
 * @brief Converts raw IEEE754 register data to its actual value
 *
 * @param dev - Device structure
 * @param reg - IEEE754 data to convert
 * @param data - Address of value
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_reg2data_ieee754(struct lt7182s_dev *dev, uint16_t reg,
				    int *data, int scale)
{
	int val;
	int exponent;
	bool sign;

	sign = no_os_field_get(LT7182S_IEEE754_SIGN_BIT, reg);
	exponent = no_os_field_get(LT7182S_IEEE754_EXPONENT_MSK, reg);
	val = no_os_field_get(LT7182S_IEEE754_MANTISSA_MSK, reg);

	if (exponent == 0) {			/* subnormal */
		exponent = -(14 + 10);
	} else if (exponent ==  0x1f) {		/* NaN, convert to min/max */
		exponent = 0;
		val = 65504;
	} else {
		exponent -= (15 + 10);		/* normal */
		val |= 0x400;
	}

	val *= scale;

	if (exponent >= 0)
		val <<= exponent;
	else
		val >>= -exponent;

	if (sign)
		val = -val;

	*data = val;

	return 0;
}

/**
 * @brief Convert value to register data
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Value to convert
 * @param reg - Address of register data
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_data2reg(struct lt7182s_dev *dev, uint8_t cmd,
			    int data, uint16_t *reg)
{
	switch (cmd) {
	case LT7182S_VOUT_COMMAND:
	case LT7182S_VOUT_MAX:
	case LT7182S_VOUT_MARGIN_HIGH:
	case LT7182S_VOUT_MARGIN_LOW:
	case LT7182S_VOUT_OV_FAULT_LIMIT:
	case LT7182S_VOUT_OV_WARN_LIMIT:
	case LT7182S_VOUT_UV_WARN_LIMIT:
	case LT7182S_VOUT_UV_FAULT_LIMIT:
	case LT7182S_READ_VOUT:
	case LT7182S_MFR_VOUT_PEAK:
		if (dev->format == LT7182S_DATA_FORMAT_LINEAR)
			return lt7182s_data2reg_linear16(dev, data, reg,
							 MILLIVOLT_PER_VOLT);
		else
			return lt7182s_data2reg_ieee754(dev, data, reg,
							MILLIVOLT_PER_VOLT);
	case LT7182S_FREQUENCY_SWITCH:
	case LT7182S_VIN_ON:
	case LT7182S_VIN_OFF:
	case LT7182S_IOUT_OC_WARN_LIMIT:
	case LT7182S_OT_FAULT_LIMIT:
	case LT7182S_OT_WARN_LIMIT:
	case LT7182S_VIN_UV_WARN_LIMIT:
	case LT7182S_IIN_OC_WARN_LIMIT:
	case LT7182S_READ_VIN:
	case LT7182S_READ_IIN:
	case LT7182S_READ_IOUT:
	case LT7182S_READ_TEMPERATURE_1:
	case LT7182S_READ_FREQUENCY:
	case LT7182S_MFR_READ_EXTVCC:
	case LT7182S_MFR_READ_ITH:
	case LT7182S_MFR_IOUT_PEAK:
	case LT7182S_MFR_TEMPERATURE_1_PEAK:
	case LT7182S_MFR_RETRY_DELAY:
	case LT7182S_MFR_RESTART_DELAY:
	case LT7182S_MFR_VIN_PEAK:
	case LT7182S_MFR_DISCHARGE_THRESHOLD:
	case LT7182S_MFR_PGOOD_DELAY:
	case LT7182S_MFR_NOT_PGOOD_DELAY:
	case LT7182S_MFR_PWM_PHASE:
		if (dev->format == LT7182S_DATA_FORMAT_LINEAR)
			return lt7182s_data2reg_linear11(dev, data, reg,
							 MILLI);
		else
			return lt7182s_data2reg_ieee754(dev, data, reg,
							MILLI);
	case LT7182S_READ_POUT:
		if (dev->format == LT7182S_DATA_FORMAT_LINEAR)
			return lt7182s_data2reg_linear11(dev, data, reg,
							 MICROWATT_PER_WATT);
		else
			return lt7182s_data2reg_ieee754(dev, data, reg,
							MICROWATT_PER_WATT);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Convert register data to actual value
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param reg - Register data to convert
 * @param data - Address of converted value
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7182s_reg2data(struct lt7182s_dev *dev, uint8_t cmd,
			    uint16_t reg, int *data)
{
	switch (cmd) {
	case LT7182S_VOUT_COMMAND:
	case LT7182S_VOUT_MAX:
	case LT7182S_VOUT_MARGIN_HIGH:
	case LT7182S_VOUT_MARGIN_LOW:
	case LT7182S_VOUT_OV_FAULT_LIMIT:
	case LT7182S_VOUT_OV_WARN_LIMIT:
	case LT7182S_VOUT_UV_WARN_LIMIT:
	case LT7182S_VOUT_UV_FAULT_LIMIT:
	case LT7182S_READ_VOUT:
	case LT7182S_MFR_VOUT_PEAK:
		if (dev->format == LT7182S_DATA_FORMAT_LINEAR)
			return lt7182s_reg2data_linear16(dev, reg, data,
							 MILLIVOLT_PER_VOLT);
		else
			return lt7182s_reg2data_ieee754(dev, reg, data,
							MILLIVOLT_PER_VOLT);
	case LT7182S_FREQUENCY_SWITCH:
	case LT7182S_VIN_ON:
	case LT7182S_VIN_OFF:
	case LT7182S_IOUT_OC_WARN_LIMIT:
	case LT7182S_OT_FAULT_LIMIT:
	case LT7182S_OT_WARN_LIMIT:
	case LT7182S_VIN_UV_WARN_LIMIT:
	case LT7182S_IIN_OC_WARN_LIMIT:
	case LT7182S_READ_VIN:
	case LT7182S_READ_IIN:
	case LT7182S_READ_IOUT:
	case LT7182S_READ_TEMPERATURE_1:
	case LT7182S_READ_FREQUENCY:
	case LT7182S_MFR_READ_EXTVCC:
	case LT7182S_MFR_READ_ITH:
	case LT7182S_MFR_IOUT_PEAK:
	case LT7182S_MFR_TEMPERATURE_1_PEAK:
	case LT7182S_MFR_RETRY_DELAY:
	case LT7182S_MFR_RESTART_DELAY:
	case LT7182S_MFR_VIN_PEAK:
	case LT7182S_MFR_DISCHARGE_THRESHOLD:
	case LT7182S_MFR_PGOOD_DELAY:
	case LT7182S_MFR_NOT_PGOOD_DELAY:
	case LT7182S_MFR_PWM_PHASE:
		if (dev->format == LT7182S_DATA_FORMAT_LINEAR)
			return lt7182s_reg2data_linear11(dev, reg, data,
							 MILLI);
		else
			return lt7182s_reg2data_ieee754(dev, reg, data,
							MILLI);
	case LT7182S_READ_POUT:
		if (dev->format == LT7182S_DATA_FORMAT_LINEAR)
			return lt7182s_reg2data_linear11(dev, reg, data,
							 MICROWATT_PER_WATT);
		else
			return lt7182s_reg2data_ieee754(dev, reg, data,
							MICROWATT_PER_WATT);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Perform packet-error checking via CRC
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param buf - Pointer to the first packet buffer
 * @param nbytes - Packet buffer size
 * @param op - Operation performed. 0 for write, 1 for read
 * @return PEC code of the PMBus packet
 */
static uint8_t lt7182s_pec(struct lt7182s_dev *dev, uint8_t cmd, uint8_t *buf,
			   size_t nbytes, uint8_t op)
{
	uint8_t crc_buf[nbytes + op + 2];

	crc_buf[0] = (dev->i2c_desc->slave_address << 1);
	crc_buf[1] = cmd;
	if (op)
		crc_buf[2] = (dev->i2c_desc->slave_address << 1) | 1;

	if (buf != NULL && nbytes > 0)
		memcpy(&crc_buf[2 + op], buf, nbytes);

	return no_os_crc8(lt7182s_crc_table, crc_buf, nbytes + op + 2, 0);
}

/**
 * @brief Initialize the device structure
 *
 * @param device - Device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_init(struct lt7182s_dev **device,
		 struct lt7182s_init_param *init_param)
{
	struct lt7182s_dev *dev;
	int ret;
	uint16_t word;
	uint8_t block[7];

	dev = (struct lt7182s_dev *)no_os_calloc(1, sizeof(struct lt7182s_dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto i2c_err;

	dev->page = LT7182S_CHAN_ALL;

	/* Identify device */
	ret = lt7182s_read_word(dev, LT7182S_CHAN_ALL,
				LT7182S_MFR_SPECIAL_ID, &word);
	if (ret)
		goto dev_err;
	if (word != LT7182S_SPECIAL_ID_VALUE) {
		ret = -EIO;
		goto dev_err;
	}

	ret = lt7182s_read_block_data(dev, LT7182S_CHAN_ALL,
				      LT7182S_IC_DEVICE_ID, block, 7);
	if (ret)
		goto dev_err;
	if (strncmp((char *)block, lt7182s_info[init_param->chip_id].name, 7)) {
		ret = -EIO;
		goto dev_err;
	}

	dev->chip_id = init_param->chip_id;

	/* Set data format and PEC */
	word = no_os_field_prep(LT7182S_CONFIG_ALL_FORMAT_BIT,
				(uint8_t)init_param->format) |
	       no_os_field_prep(LT7182S_CONFIG_ALL_PEC_BIT,
				(uint8_t)init_param->crc_en);
	ret = lt7182s_write_word(dev, LT7182S_CHAN_ALL,
				 LT7182S_MFR_CONFIG_ALL, word);
	if (ret)
		goto dev_err;

	dev->crc_en = init_param->crc_en;
	dev->format = init_param->format;

	/* Populate CRC table for PEC */
	if (dev->crc_en)
		no_os_crc8_populate_msb(lt7182s_crc_table,
					LT7182S_CRC_POLYNOMIAL);

	if (dev->format == LT7182S_DATA_FORMAT_LINEAR)
		dev->lin16_exp = LT7182S_LIN16_EXPONENT;

	/* Initialize GPIO for PGOOD0 */
	ret = no_os_gpio_get_optional(&dev->pg0_desc,
				      init_param->pg0_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->pg0_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for PGOOD1 */
	ret = no_os_gpio_get_optional(&dev->pg1_desc,
				      init_param->pg1_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->pg1_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for RUN0 */
	ret = no_os_gpio_get_optional(&dev->run0_desc,
				      init_param->run0_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_output(dev->run0_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for RUN1 */
	ret = no_os_gpio_get_optional(&dev->run1_desc,
				      init_param->run1_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_output(dev->run1_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for ALERT */
	ret = no_os_gpio_get_optional(&dev->alert_desc,
				      init_param->alert_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->alert_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for FAULT0 */
	ret = no_os_gpio_get_optional(&dev->fault0_desc,
				      init_param->fault0_param);
	if (ret)
		goto dev_err;

	if (init_param->fault0_cfg == LT7182S_FAULT_PIN_OUTPUT) {
		ret = no_os_gpio_direction_output(dev->fault0_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto dev_err;
	} else {
		ret = no_os_gpio_direction_input(dev->fault0_desc);
		if (ret)
			goto dev_err;
	}

	/* Initialize GPIO for FAULT1 */
	ret = no_os_gpio_get_optional(&dev->fault1_desc,
				      init_param->fault1_param);
	if (ret)
		goto dev_err;

	if (init_param->fault1_cfg == LT7182S_FAULT_PIN_OUTPUT) {
		ret = no_os_gpio_direction_output(dev->fault1_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto dev_err;
	} else {
		ret = no_os_gpio_direction_input(dev->fault1_desc);
		if (ret)
			goto dev_err;
	}

	/* Initialize external clock for synchronization if used */
	if (init_param->external_clk_en) {
		ret = no_os_pwm_init(&dev->sync_desc, init_param->sync_param);
		if (ret)
			goto dev_err;

		ret = lt7182s_sync_config(dev, false, true);
		if (ret)
			goto pwm_err;
	}

	/* Set operation */
	ret = lt7182s_set_operation(dev, LT7182S_CHAN_ALL,
				    LT7182S_OPERATION_ON);
	if (ret)
		goto pwm_err;

	/* Clear faults */
	ret = lt7182s_clear_faults(dev);
	if (ret)
		goto pwm_err;

	*device = dev;

	return 0;

pwm_err:
	no_os_pwm_remove(dev->sync_desc);
dev_err:
	no_os_gpio_remove(dev->fault1_desc);
	no_os_gpio_remove(dev->fault0_desc);
	no_os_gpio_remove(dev->alert_desc);
	no_os_gpio_remove(dev->run1_desc);
	no_os_gpio_remove(dev->run0_desc);
	no_os_gpio_remove(dev->pg1_desc);
	no_os_gpio_remove(dev->pg0_desc);
	no_os_i2c_remove(dev->i2c_desc);
i2c_err:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free or remove device instance
 *
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_remove(struct lt7182s_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->pg0_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->pg1_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->run0_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->run1_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->alert_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->fault0_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->fault1_desc);
	if (ret)
		return ret;

	if (dev->sync_desc) {
		ret = no_os_pwm_remove(dev->sync_desc);
		if (ret)
			return ret;
	}

	no_os_free(dev);

	return ret;
}

/**
 * @brief Set page of the device.
 * 	  Page 0x0 - Channel 0
 * 	  Page 0x1 - Channel 1
 * 	  Page 0xff - Both channels
 *
 * @param dev - Device structure
 * @param page - Page to set
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_set_page(struct lt7182s_dev *dev, int page)
{
	int ret = 0;
	uint8_t read_page;
	uint8_t tx_buf[3] = {0};
	uint8_t rx_buf[2] = {0};

	if (!(page == LT7182S_CHAN_0 || page == LT7182S_CHAN_1 ||
	      page == LT7182S_CHAN_ALL))
		return -EINVAL;

	if (dev->page != page) {
		tx_buf[0] = LT7182S_PAGE;
		tx_buf[1] = page & 0xFF;

		if (dev->crc_en)
			tx_buf[2] = lt7182s_pec(dev, LT7182S_PAGE,
						(uint8_t *)&page, 1, 0);

		ret = no_os_i2c_write(dev->i2c_desc, tx_buf,
				      dev->crc_en + 2, 1);
		if(ret)
			return ret;

		ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
		if(ret)
			return ret;

		if (dev->crc_en) {
			ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
			if (ret)
				return ret;

			if (lt7182s_pec(dev, LT7182S_PAGE, rx_buf, 1, 1) !=
			    rx_buf[1])
				return -EBADMSG;

			read_page = rx_buf[0];

		} else {
			ret = no_os_i2c_read(dev->i2c_desc, &read_page, 1, 1);
			if(ret)
				return ret;
		}

		if(read_page != page)
			return -EIO;

		dev->page = page;
	}

	return ret;
}

/**
 * @brief Send a PMBus command to the device
 *
 * @param dev - Device structure
 * @param page - Page or channel of the command
 * @param cmd - PMBus command
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_send_byte(struct lt7182s_dev *dev, int page, uint8_t cmd)
{
	int ret;
	uint8_t tx_buf[2] = {0};

	if (lt7182s_cmd_is_paged(cmd)) {
		ret = lt7182s_set_page(dev, page);
		if(ret)
			return ret;
	}

	tx_buf[0] = cmd;
	if (dev->crc_en)
		tx_buf[1] = lt7182s_pec(dev, cmd, NULL, 0, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 1, 1);
}

/**
 * @brief Perform a raw PMBus read byte operation
 *
 * @param dev - Device structure
 * @param page - Page or channel of the command
 * @param cmd - PMBus command
 * @param data - Address of the byte read
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_read_byte(struct lt7182s_dev *dev, int page,
		      uint8_t cmd, uint8_t *data)
{
	int ret;
	uint8_t rx_buf[2];

	if (lt7182s_cmd_is_paged(cmd)) {
		ret = lt7182s_set_page(dev, page);
		if(ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		if (lt7182s_pec(dev, cmd, rx_buf, 1, 1) != rx_buf[1])
			return -EBADMSG;

		*data = rx_buf[0];
		return ret;
	} else
		return no_os_i2c_read(dev->i2c_desc, data, 1, 1);
}

/**
 * @brief Perform a raw PMBus write byte operation
 *
 * @param dev - Device structure
 * @param page - Page/channel of the command
 * @param cmd - PMBus command
 * @param value - Byte to be written
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_write_byte(struct lt7182s_dev *dev, int page,
		       uint8_t cmd, uint8_t value)
{
	int ret;
	uint8_t tx_buf[3] = {0};

	if (lt7182s_cmd_is_paged(cmd)) {
		ret = lt7182s_set_page(dev, page);
		if(ret)
			return ret;
	}

	tx_buf[0] = cmd;
	tx_buf[1] = value;

	if (dev->crc_en)
		tx_buf[2] = lt7182s_pec(dev, cmd, &value, 1, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 2, 1);
}

/**
 * @brief Perform a raw PMBus read word operation
 *
 * @param dev - Device structure
 * @param page - Page/channel of the command
 * @param cmd - PMBus command
 * @param word - Address of the read word
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_read_word(struct lt7182s_dev *dev, int page,
		      uint8_t cmd, uint16_t *word)
{
	int ret;
	uint8_t rx_buf[3] = {0};

	if (lt7182s_cmd_is_paged(cmd)) {
		ret = lt7182s_set_page(dev, page);
		if(ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 3, 1);
		if (ret)
			return ret;

		if (lt7182s_pec(dev, cmd, rx_buf, 2, 1) != rx_buf[2])
			return -EBADMSG;
	} else {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if(ret)
			return ret;
	}

	*word = no_os_get_unaligned_le16(rx_buf);
	return 0;
}

/**
 * @brief Perform a raw PMBus write word operation
 *
 * @param dev - Device structure
 * @param page - Page/channel of the command
 * @param cmd - PMBus command
 * @param word - Word to write
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_write_word(struct lt7182s_dev *dev, int page,
		       uint8_t cmd, uint16_t word)
{
	int ret;
	uint8_t tx_buf[4] = {0};

	if (lt7182s_cmd_is_paged(cmd)) {
		ret = lt7182s_set_page(dev, page);
		if(ret)
			return ret;
	}

	tx_buf[0] = cmd;
	no_os_put_unaligned_le16(word, &tx_buf[1]);

	if (dev->crc_en)
		tx_buf[3] = lt7182s_pec(dev, cmd, &tx_buf[1], 2, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 3, 1);
}

/**
 * @brief Perform a PMBus read word operation and converts to actual value
 *
 * @param dev - Device structure
 * @param page - Page/channel of the command
 * @param cmd - PMBus command
 * @param data - Address of data read
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_read_word_data(struct lt7182s_dev *dev, int page,
			   uint8_t cmd, int *data)
{
	int ret;
	uint16_t reg;

	ret = lt7182s_read_word(dev, page, cmd, &reg);
	if (ret)
		return ret;

	return lt7182s_reg2data(dev, cmd, reg, data);
}

/**
 * @brief Converts value to register data and do PMBus write word operation
 *
 * @param dev - Device structure
 * @param page - Page/channel of the command
 * @param cmd - PMBus command
 * @param data - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_write_word_data(struct lt7182s_dev *dev, int page,
			    uint8_t cmd, int data)
{
	int ret;
	uint16_t reg;

	ret = lt7182s_data2reg(dev, cmd, data, &reg);
	if (ret)
		return ret;

	return lt7182s_write_word(dev, page, cmd, reg);
}

/**
 * @brief Perform a PMBus read block operation
 *
 * @param dev - Device structure
 * @param page - Page/channel of the command
 * @param cmd - PMBus command
 * @param data - Address of the read block
 * @param nbytes - Size of the block in bytes
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_read_block_data(struct lt7182s_dev *dev, int page, uint8_t cmd,
			    uint8_t *data, size_t nbytes)
{
	int ret;
	uint8_t rxbuf[nbytes + 2];

	if (lt7182s_cmd_is_paged(cmd)) {
		ret = lt7182s_set_page(dev, page);
		if(ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if(ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 2, 1);
		if (ret)
			return ret;

		if ((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;

		if (lt7182s_pec(dev, cmd, rxbuf, nbytes + 1, 1) !=
		    rxbuf[nbytes + 1])
			return -EBADMSG;
	} else {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 1, 1);
		if(ret)
			return ret;

		if((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;
	}

	memcpy(data, &rxbuf[1], nbytes);

	return 0;
}

/**
 * @brief Read a value
 *
 * @param dev - Device structure
 * @param channel - Channel selected
 * @param value_type - Value type.
 * 		       Example values: LT7182S_VIN
 * 				       LT7182S_VOUT
 * 				       LT7182S_IIN
 * 				       LT7182S_IOUT
 * 				       LT7182S_TEMP
 * 				       LT7182S_FREQUENCY
 * @param value - Address of the read value
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_read_value(struct lt7182s_dev *dev,
		       int channel,
		       enum lt7182s_value_type value_type,
		       int *value)
{
	return lt7182s_read_word_data(dev, channel,
				      (uint8_t)value_type, value);
}

/**
 * @brief Read statuses
 *
 * @param dev - Device structure
 * @param channel - Channel of the status to read
 * @param status_type - Status type.
 * 			Example values: LT7182S_STATUS_BYTE_TYPE
 * 					LT7182S_STATUS_VOUT_TYPE
 * 					LT7182S_STATUS_IOUT_TYPE
 * 					LT7182S_STATUS_INPUT_TYPE
 * 					LT7182S_STATUS_CML_TYPE
 * @param status - Address of the status structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_read_status(struct lt7182s_dev *dev, int channel,
			enum lt7182s_status_type status_type,
			struct lt7182s_status *status)
{
	int ret;

	if (status_type & LT7182S_STATUS_WORD_TYPE) {
		ret = lt7182s_read_word(dev, channel, LT7182S_STATUS_WORD,
					&status->word);
		if (ret)
			return ret;
	}

	if (status_type & LT7182S_STATUS_BYTE_TYPE) {
		ret = lt7182s_read_byte(dev, channel, LT7182S_STATUS_BYTE,
					&status->byte);
		if (ret)
			return ret;
	}

	if (status_type & LT7182S_STATUS_VOUT_TYPE) {
		ret = lt7182s_read_byte(dev, channel, LT7182S_STATUS_VOUT,
					&status->vout);
		if (ret)
			return ret;
	}

	if (status_type & LT7182S_STATUS_IOUT_TYPE) {
		ret = lt7182s_read_byte(dev, channel, LT7182S_STATUS_IOUT,
					&status->iout);
		if (ret)
			return ret;
	}

	if (status_type & LT7182S_STATUS_INPUT_TYPE) {
		ret = lt7182s_read_byte(dev, channel, LT7182S_STATUS_INPUT,
					&status->input);
		if (ret)
			return ret;
	}

	if (status_type & LT7182S_STATUS_TEMP_TYPE) {
		ret = lt7182s_read_byte(dev, LT7182S_CHAN_ALL,
					LT7182S_STATUS_TEMPERATURE,
					&status->temp);
		if (ret)
			return ret;
	}

	if (status_type & LT7182S_STATUS_CML_TYPE) {
		ret = lt7182s_read_byte(dev, LT7182S_CHAN_ALL,
					LT7182S_STATUS_CML, &status->cml);
		if (ret)
			return ret;
	}

	if (status_type & LT7182S_STATUS_MFR_SPECIFIC_TYPE) {
		ret = lt7182s_read_byte(dev, channel,
					LT7182S_STATUS_MFR_SPECIFIC,
					&status->mfr_specific);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set output voltage and its upper limit
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param vout_command - Output voltage in millivolts
 * @param vout_max - Output voltage upper limit in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_vout_value(struct lt7182s_dev *dev, int channel,
		       int vout_command, int vout_max)
{
	int ret;

	ret = lt7182s_write_word_data(dev, channel,
				      LT7182S_VOUT_COMMAND, vout_command);
	if (ret)
		return ret;

	return lt7182s_write_word_data(dev, channel,
				       LT7182S_VOUT_MAX, vout_max);
}

/**
 * @brief Set output voltage transition rate
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param tr - Transition rate in V/s or mV/ms
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_vout_tr(struct lt7182s_dev *dev, int channel, int tr)
{
	return lt7182s_write_word_data(dev, channel,
				       LT7182S_VOUT_TRANSITION_RATE, tr);
}

/**
 * @brief Set output voltage margin
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param margin_high - Upper margin in millivolts
 * @param margin_low - Lower margin in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_vout_margin(struct lt7182s_dev *dev, int channel,
			int margin_high, int margin_low)
{
	int ret;

	if (margin_high < margin_low)
		return -EINVAL;

	ret = lt7182s_write_word_data(dev, channel,
				      LT7182S_VOUT_MARGIN_HIGH, margin_high);
	if (ret)
		return ret;

	return lt7182s_write_word_data(dev, channel,
				       LT7182S_VOUT_MARGIN_LOW, margin_low);
}

/**
 * @brief Set input voltage window at which power conversion will proceed
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param vin_on - Input voltage in millivolts at which conversion will start
 * @param vin_off - Input voltage in millivolts at which conversion will stop
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_set_vin(struct lt7182s_dev *dev, int channel,
		    int vin_on, int vin_off)
{
	int ret;

	if (vin_on < vin_off)
		return -EINVAL;

	if (vin_on < LT7182S_VIN_ON_MIN || vin_on > LT7182S_VIN_ON_MAX)
		return -EINVAL;

	if (vin_off < LT7182S_VIN_OFF_MIN || vin_off > LT7182S_VIN_OFF_MAX)
		return -EINVAL;

	ret = lt7182s_write_word_data(dev, channel,
				      LT7182S_VIN_ON, vin_on);
	if (ret)
		return ret;

	return lt7182s_write_word_data(dev, channel,
				       LT7182S_VIN_OFF, vin_off);
}

/**
 * @brief Set timing values
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param timing_type - Timing value type.
 * 			Example: LT7182S_TON_DELAY_TYPE
 * 				 LT7182S_TON_RISE_TYPE
 * 				 LT7182S_TOFF_DELAY_TYPE
 * 				 LT7182_RETRY_DELAY_TYPE
 * @param time - Time value in microseconds
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_set_timing(struct lt7182s_dev *dev, int channel,
		       enum lt7182s_timing_type timing_type, int time)
{
	return lt7182s_write_word_data(dev, channel,
				       timing_type, time);
}

/**
 * @brief Set switching frequency
 *
 * @param dev - Device structure
 * @param freq - Frequency to set in Hz. Value should be between 400 kHz and
 * 		 4000 kHz.
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_switch_freq(struct lt7182s_dev *dev, int freq)
{
	if (freq < LT7182S_FREQ_MIN || freq > LT7182S_FREQ_MAX)
		return -EINVAL;

	return lt7182s_write_word_data(dev, LT7182S_CHAN_ALL,
				       LT7182S_FREQUENCY_SWITCH, freq);
}

/**
 * @brief Set output PWM mode
 *
 * @param dev - Device structure
 * @param channel - Output channel
 * @param pwm_mode -
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_pwm_mode(struct lt7182s_dev *dev, int channel,
		     enum lt7182s_pwm_mode pwm_mode)
{
	int ret;
	uint16_t temp_word;

	if (channel == LT7182S_CHAN_ALL) {
		ret = lt7182s_read_word(dev, LT7182S_CHAN_0,
					LT7182S_MFR_PWM_MODE, &temp_word);
		if (ret)
			return ret;

		temp_word &= ~(LT7182S_PWM_OP_MODE_BIT);
		temp_word |= no_os_field_prep(LT7182S_PWM_OP_MODE_BIT,
					      pwm_mode);

		ret = lt7182s_write_word(dev, LT7182S_CHAN_0,
					 LT7182S_MFR_PWM_MODE, temp_word);
		if (ret)
			return ret;

		channel = LT7182S_CHAN_1;
	}

	ret = lt7182s_read_word(dev, channel,
				LT7182S_MFR_PWM_MODE, &temp_word);
	if (ret)
		return ret;

	temp_word &= ~(LT7182S_PWM_OP_MODE_BIT);
	temp_word |= no_os_field_prep(LT7182S_PWM_OP_MODE_BIT, pwm_mode);

	return lt7182s_write_word(dev, channel,
				  LT7182S_MFR_PWM_MODE, temp_word);
}

/**
 * @brief Set output PWM phase
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param phase - Phase in milli-degrees
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_pwm_phase(struct lt7182s_dev *dev, int channel,
		      int phase)
{
	phase %= LT7182S_PHASE_FULL_MILLI;
	if (phase < 0)
		phase += LT7182S_PHASE_FULL_MILLI;

	return lt7182s_write_word_data(dev, channel,
				       LT7182S_MFR_PWM_PHASE, phase);
}

/**
 * @brief Set overvalue and undervalue limits
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param limit - Limit value type.
 * 			Example: LT7182S_VOUT_OV_FAULT_LIMIT_TYPE
 * 				 LT7182S_OT_WARN_LIMIT_TYPE
 * 				 LT7182S_TON_MAX_FAULT_LIMIT_TYPE
 * 				 LT7182S_IIN_OC_WARN_LIMIT_TYPE
 * @param limit_val - Limit value in milli-units for voltage and current, and
 * 		      microseconds for timing.
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_set_limit(struct lt7182s_dev *dev, int channel,
		      enum lt7182s_limit_type limit, int limit_val)
{
	return lt7182s_write_word_data(dev, channel, (uint8_t)limit, limit_val);
}

/**
 * @brief Set channel operation
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param operation - Operation.
 * 		      Accepted values are: LT7182S_OPERATION_OFF
 * 					   LT7182S_OPERATION_ON
 * 					   LT7182S_OPERATION_MARGIN_HIGH
 * 					   LT7182S_OPERATION_MARGIN_LOW
 * 					   LT7182S_OPERATION_SEQ_OFF
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_set_operation(struct lt7182s_dev *dev, int channel,
			  enum lt7182s_operation_type operation)
{
	return lt7182s_write_byte(dev, channel, LT7182S_OPERATION,
				  (uint8_t)operation);
}

/**
 * @brief Set channel state using the RUN pin.
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param state - true for ON, false for OFF
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_set_channel_state(struct lt7182s_dev *dev, int channel,
			      bool state)
{
	switch (channel) {
	case LT7182S_CHAN_0:
		return no_os_gpio_direction_output(dev->run0_desc, state);
	case LT7182S_CHAN_1:
		return no_os_gpio_direction_output(dev->run1_desc, state);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Configure SYNC pin for clock synchronization
 *
 * @param dev - Device structure
 * @param input_clk - Enable to use SYNC clock input
 * @param output_clk - Enable to use SYNC output clock
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_sync_config(struct lt7182s_dev *dev,
			bool input_clk,
			bool output_clk)
{
	int ret;
	uint8_t temp;

	ret = lt7182s_read_byte(dev, LT7182S_CHAN_ALL,
				LT7182S_MFR_SYNC_CONFIG, &temp);
	if (ret)
		return ret;

	temp &= ~(LT7182S_SYNC_CLK_INPUT_BIT | LT7182S_SYNC_CLK_OUTPUT_BIT);
	temp |= no_os_field_prep(LT7182S_SYNC_CLK_INPUT_BIT, !input_clk) |
		no_os_field_prep(LT7182S_SYNC_CLK_OUTPUT_BIT, output_clk);

	return lt7182s_write_byte(dev, LT7182S_CHAN_ALL,
				  LT7182S_MFR_SYNC_CONFIG, temp);
}

/**
 * @brief Configure ADC control
 *
 * @param dev - Device structure
 * @param low_freq_telemetry - Enable for low frequency telemetry (every 100ms)
 * @param debug_telemetry - Enable for debug telemetry with EXTVcc and Ith
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_adc_control(struct lt7182s_dev *dev, bool low_freq_telemetry,
			bool debug_telemetry)
{
	return lt7182s_write_byte(dev, LT7182S_CHAN_ALL,
				  LT7182S_MFR_ADC_CONTROL,
				  no_os_field_prep(LT7182S_ADC_CTRL_DEBUG_BIT,
						  (uint8_t)debug_telemetry) |
				  no_os_field_prep(LT7182S_ADC_CTRL_LOW_FREQ_BIT,
						  (uint8_t)low_freq_telemetry));
}

/**
 * @brief Perform commands for non-volatile memory/EEPROM
 *
 * @param dev - Device structure
 * @param cmd - NVM commands.
 * 		Example: LT7182S_LOCK_USER
 * 			 LT7182S_UNLOCK_USER
 * 			 LT7182S_STORE_USER
 * 			 LT7182S_COMPARE_USER
 * 			 LT7182S_RESTORE_USER
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_nvm_cmd(struct lt7182s_dev *dev, enum lt7182s_nvm_cmd_type cmd)
{
	int ret;

	switch (cmd) {
	case LT7182S_LOCK_USER:
		return lt7182s_write_byte(dev, LT7182S_CHAN_ALL,
					  LT7182S_MFR_EE_USER_WP, 1);
	case LT7182S_UNLOCK_USER:
		return lt7182s_write_byte(dev, LT7182S_CHAN_ALL,
					  LT7182S_MFR_EE_USER_WP, 0);
	case LT7182S_STORE_USER:
	case LT7182S_COMPARE_USER:
		return lt7182s_send_byte(dev, LT7182S_CHAN_ALL,
					 (uint8_t)cmd);
	case LT7182S_RESTORE_USER:
		ret = lt7182s_send_byte(dev, LT7182S_CHAN_ALL,
					(uint8_t)cmd);
		if (ret)
			return ret;

		dev->format = LT7182S_DATA_FORMAT_IEEE754;
		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}

/**
 * @brief Clear all asserted faults
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_clear_faults(struct lt7182s_dev *dev)
{
	return lt7182s_send_byte(dev, LT7182S_CHAN_ALL, LT7182S_CLEAR_FAULTS);
}

/**
 * @brief Perform a device software reset
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7182s_software_reset(struct lt7182s_dev *dev)
{
	int ret;

	ret = lt7182s_send_byte(dev, LT7182S_CHAN_ALL, LT7182S_MFR_RESET);
	if (ret)
		return ret;

	dev->format = LT7182S_DATA_FORMAT_IEEE754;

	return 0;
}
