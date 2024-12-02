/***************************************************************************//**
 *   @file   ltm4686.c
 *   @brief  Source code of the LTM4686 driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>

#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_crc8.h"

#include "ltm4686.h"

NO_OS_DECLARE_CRC8_TABLE(ltm4686_crc_table);

const struct ltm4686_chip_info ltm4686_info[] = {
	[ID_LTM4686] = {
		.num_channels = 2,
	},
	[ID_LTM4686B] = {
		.num_channels = 2,
	},
	[ID_LTM4673] = {
		.num_channels = 4,
	},
};

/**
 * @brief Check if PMBus command is paged
 *
 * @param cmd - PMBus command
 * @return true if paged, false otherwise
 */
static bool ltm4686_cmd_is_paged(uint8_t cmd)
{
	switch (cmd) {
	case LTM4686_OPERATION:
	case LTM4686_ON_OFF_CONFIG:
	case LTM4686_SMBALERT_MASK:
	case LTM4686_VOUT_MODE:
	case LTM4686_VOUT_COMMAND:
	case LTM4686_VOUT_MAX:
	case LTM4686_VOUT_MARGIN_HIGH:
	case LTM4686_VOUT_MARGIN_LOW:
	case LTM4686_VOUT_TRANSITION_RATE:
	case LTM4686_IOUT_CAL_GAIN:
	case LTM4686_VOUT_OV_FAULT_LIMIT:
	case LTM4686_VOUT_OV_FAULT_RESPONSE:
	case LTM4686_VOUT_OV_WARN_LIMIT:
	case LTM4686_VOUT_UV_WARN_LIMIT:
	case LTM4686_VOUT_UV_FAULT_LIMIT:
	case LTM4686_VOUT_UV_FAULT_RESPONSE:
	case LTM4686_IOUT_OC_FAULT_RESPONSE:
	case LTM4686_IOUT_OC_WARN_LIMIT:
	case LTM4686_OT_FAULT_LIMIT:
	case LTM4686_OT_FAULT_RESPONSE:
	case LTM4686_OT_WARN_LIMIT:
	case LTM4686_UT_FAULT_LIMIT:
	case LTM4686_UT_FAULT_RESPONSE:
	case LTM4686_IIN_OC_WARN_LIMIT:
	case LTM4686_TON_DELAY:
	case LTM4686_TON_RISE:
	case LTM4686_TON_MAX_FAULT_LIMIT:
	case LTM4686_TON_MAX_FAULT_RESPONSE:
	case LTM4686_TOFF_DELAY:
	case LTM4686_TOFF_FALL:
	case LTM4686_TOFF_MAX_WARN_LIMIT:
	case LTM4686_STATUS_BYTE:
	case LTM4686_STATUS_WORD:
	case LTM4686_STATUS_VOUT:
	case LTM4686_STATUS_IOUT:
	case LTM4686_STATUS_TEMPERATURE:
	case LTM4686_STATUS_MFR_SPECIFIC:
	case LTM4686_READ_VOUT:
	case LTM4686_READ_IOUT:
	case LTM4686_READ_TEMPERATURE_1:
	case LTM4686_READ_DUTY_CYCLE:
	case LTM4686_READ_POUT:
	case LTM4686_MFR_VOUT_MAX:
	case LTM4686_MFR_USER_DATA_01:
	case LTM4686_MFR_USER_DATA_03:
	case LTM4686_MFR_CHAN_CONFIG:
	case LTM4686_MFR_GPIO_PROPAGATE:
	case LTM4686_MFR_PWM_MODE:
	case LTM4686_MFR_GPIO_RESPONSE:
	case LTM4686_MFR_IOUT_PEAK:
	case LTM4686_MFR_RETRY_DELAY:
	case LTM4686_MFR_RESTART_DELAY:
	case LTM4686_MFR_VOUT_PEAK:
	case LTM4686_MFR_TEMPERATURE_1_PEAK:
	case LTM4686_MFR_IIN_OFFSET:
	case LTM4686_MFR_READ_IIN:
	case LTM4686_MFR_IOUT_CAL_GAIN_TC:
	case LTM4686_MFR_TEMP_1_GAIN:
	case LTM4686_MFR_TEMP_1_OFFSET:
	case LTM4686_MFR_RAIL_ADDRESS:
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
static int ltm4686_data2reg_linear16(struct ltm4686_dev *dev, int data,
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
static int ltm4686_data2reg_linear11(struct ltm4686_dev *dev, int data,
				     uint16_t *reg, int scale)
{
	int exp = 0, mant = 0;
	uint8_t negative = 0;

	if (data < 0) {
		negative = 1;
		data = -data;
	}

	/* If value too high, continuously do m/2 until m < 1023. */
	while (data >= LTM4686_LIN11_MANTISSA_MAX * scale &&
	       exp < LTM4686_LIN11_EXPONENT_MAX) {
		exp++;
		data >>= 1;
	}

	/* If value too low, increase mantissa. */
	while (data < LTM4686_LIN11_MANTISSA_MIN * scale &&
	       exp > LTM4686_LIN11_EXPONENT_MIN) {
		exp--;
		data <<= 1;
	}

	mant = no_os_clamp(NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale),
			   0, 0x3FF);
	if (negative)
		mant = -mant;

	*reg = no_os_field_prep(LTM4686_LIN11_MANTISSA_MSK, mant) |
	       no_os_field_prep(LTM4686_LIN11_EXPONENT_MSK, exp);

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
static int ltm4686_reg2data_linear16(struct ltm4686_dev *dev, uint16_t reg,
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
static int ltm4686_reg2data_linear11(struct ltm4686_dev *dev, uint16_t reg,
				     int *data, int scale)
{
	int val, exp, mant;

	exp = LTM4686_LIN11_EXPONENT(reg);
	mant = LTM4686_LIN11_MANTISSA(reg);

	val = mant * scale;
	if (exp >= 0)
		*data = val << exp;
	else
		*data = val >> -exp;

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
static int ltm4686_data2reg(struct ltm4686_dev *dev, uint8_t cmd,
			    int data, uint16_t *reg)
{
	switch (cmd) {
	case LTM4686_VOUT_COMMAND:
	case LTM4686_VOUT_MAX:
	case LTM4686_VOUT_MARGIN_HIGH:
	case LTM4686_VOUT_MARGIN_LOW:
	case LTM4686_VOUT_OV_FAULT_LIMIT:
	case LTM4686_VOUT_OV_WARN_LIMIT:
	case LTM4686_VOUT_UV_WARN_LIMIT:
	case LTM4686_VOUT_UV_FAULT_LIMIT:
	case LTM4686_READ_VOUT:
	case LTM4686_MFR_VOUT_MAX:
	case LTM4686_MFR_VOUT_PEAK:
		return ltm4686_data2reg_linear16(dev, data, reg,
						 MILLIVOLT_PER_VOLT);
	case LTM4686_VOUT_TRANSITION_RATE:
		return ltm4686_data2reg_linear11(dev, data, reg,
						 MICROVOLT_PER_VOLT);
	case LTM4686_TON_DELAY:
	case LTM4686_TON_RISE:
	case LTM4686_TON_MAX_FAULT_LIMIT:
	case LTM4686_TOFF_DELAY:
	case LTM4686_TOFF_FALL:
	case LTM4686_TOFF_MAX_WARN_LIMIT:
	case LTM4686_MFR_RETRY_DELAY:
	case LTM4686_MFR_RESTART_DELAY:
	case LTM4686_FREQUENCY_SWITCH:
	case LTM4686_VIN_ON:
	case LTM4686_VIN_OFF:
	case LTM4686_IOUT_CAL_GAIN:
	case LTM4686_IOUT_OC_FAULT_LIMIT:
	case LTM4686_IOUT_OC_WARN_LIMIT:
	case LTM4686_OT_FAULT_LIMIT:
	case LTM4686_OT_WARN_LIMIT:
	case LTM4686_UT_FAULT_LIMIT:
	case LTM4686_VIN_OV_FAULT_LIMIT:
	case LTM4686_VIN_UV_WARN_LIMIT:
	case LTM4686_IIN_OC_WARN_LIMIT:
	case LTM4686_READ_VIN:
	case LTM4686_READ_IIN:
	case LTM4686_READ_IOUT:
	case LTM4686_READ_TEMPERATURE_1:
	case LTM4686_READ_TEMPERATURE_2:
	case LTM4686_READ_DUTY_CYCLE:
	case LTM4686_MFR_IOUT_PEAK:
	case LTM4686_MFR_VIN_PEAK:
	case LTM4686_MFR_TEMPERATURE_1_PEAK:
	case LTM4686_MFR_IIN_OFFSET:
	case LTM4686_MFR_READ_IIN:
	case LTM4686_MFR_TEMPERATURE_2_PEAK:
	case LTM4686_MFR_TEMP_1_OFFSET:
		return ltm4686_data2reg_linear11(dev, data, reg, MILLI);
	case LTM4686_READ_POUT:
		return ltm4686_data2reg_linear11(dev, data, reg,
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
static int ltm4686_reg2data(struct ltm4686_dev *dev, uint8_t cmd,
			    uint16_t reg, int *data)
{
	switch (cmd) {
	case LTM4686_VOUT_COMMAND:
	case LTM4686_VOUT_MAX:
	case LTM4686_VOUT_MARGIN_HIGH:
	case LTM4686_VOUT_MARGIN_LOW:
	case LTM4686_VOUT_OV_FAULT_LIMIT:
	case LTM4686_VOUT_OV_WARN_LIMIT:
	case LTM4686_VOUT_UV_WARN_LIMIT:
	case LTM4686_VOUT_UV_FAULT_LIMIT:
	case LTM4686_READ_VOUT:
	case LTM4686_MFR_VOUT_MAX:
	case LTM4686_MFR_VOUT_PEAK:
		return ltm4686_reg2data_linear16(dev, reg, data,
						 MILLIVOLT_PER_VOLT);
	case LTM4686_VOUT_TRANSITION_RATE:
		return ltm4686_reg2data_linear11(dev, reg, data,
						 MICROVOLT_PER_VOLT);
	case LTM4686_TON_DELAY:
	case LTM4686_TON_RISE:
	case LTM4686_TON_MAX_FAULT_LIMIT:
	case LTM4686_TOFF_DELAY:
	case LTM4686_TOFF_FALL:
	case LTM4686_TOFF_MAX_WARN_LIMIT:
	case LTM4686_MFR_RETRY_DELAY:
	case LTM4686_MFR_RESTART_DELAY:
	case LTM4686_FREQUENCY_SWITCH:
	case LTM4686_VIN_ON:
	case LTM4686_VIN_OFF:
	case LTM4686_IOUT_CAL_GAIN:
	case LTM4686_IOUT_OC_FAULT_LIMIT:
	case LTM4686_IOUT_OC_WARN_LIMIT:
	case LTM4686_OT_FAULT_LIMIT:
	case LTM4686_OT_WARN_LIMIT:
	case LTM4686_UT_FAULT_LIMIT:
	case LTM4686_VIN_OV_FAULT_LIMIT:
	case LTM4686_VIN_UV_WARN_LIMIT:
	case LTM4686_IIN_OC_WARN_LIMIT:
	case LTM4686_READ_VIN:
	case LTM4686_READ_IIN:
	case LTM4686_READ_IOUT:
	case LTM4686_READ_TEMPERATURE_1:
	case LTM4686_READ_TEMPERATURE_2:
	case LTM4686_READ_DUTY_CYCLE:
	case LTM4686_MFR_IOUT_PEAK:
	case LTM4686_MFR_VIN_PEAK:
	case LTM4686_MFR_TEMPERATURE_1_PEAK:
	case LTM4686_MFR_IIN_OFFSET:
	case LTM4686_MFR_READ_IIN:
	case LTM4686_MFR_TEMPERATURE_2_PEAK:
	case LTM4686_MFR_TEMP_1_OFFSET:
		return ltm4686_reg2data_linear11(dev, reg, data, MILLI);
	case LTM4686_READ_POUT:
		return ltm4686_reg2data_linear11(dev, reg, data,
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
static uint8_t ltm4686_pec(struct ltm4686_dev *dev, uint8_t cmd, uint8_t *buf,
			   size_t nbytes, uint8_t op)
{
	uint8_t crc_buf[nbytes + op + 2];

	crc_buf[0] = (dev->i2c_desc->slave_address << 1);
	crc_buf[1] = cmd;
	if (op)
		crc_buf[2] = (dev->i2c_desc->slave_address << 1) | 1;

	memcpy(&crc_buf[2 + op], buf, nbytes);

	return no_os_crc8(ltm4686_crc_table, crc_buf, nbytes + op + 2, 0);
}

/**
 * @brief Match device ID
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
static int ltm4686_match_id(struct ltm4686_dev *dev)
{
	int ret;
	uint16_t word;

	ret = ltm4686_read_word(dev, LTM4686_CHAN_ALL, LTM4686_MFR_SPECIAL_ID,
				&word);
	if (ret)
		return ret;

	switch (dev->id) {
	case ID_LTM4686:
	case ID_LTM4686B:
		if ((word & LTM4686_ID_MSK) != LTM4686_SPECIAL_ID_VALUE)
			return -EIO;
		break;
	case ID_LTM4673:
		if ((word & LTM4686_ID_MSK) != LTM4673_SPECIAL_ID_VALUE_REV_1)
			return 0;
		if ((word & LTM4686_ID_MSK) != LTM4673_SPECIAL_ID_VALUE_REV_2)
			return 0;
		break;
	default:
		return -EIO;
	}

	return 0;
}

/**
 * @brief Initialize the device structure
 *
 * @param device - Device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_init(struct ltm4686_dev **device,
		 struct ltm4686_init_param *init_param)
{
	struct ltm4686_dev *dev;
	int ret, i;
	uint8_t byte;

	dev = (struct ltm4686_dev *)no_os_calloc(1, sizeof(struct ltm4686_dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto i2c_err;

	dev->page = LTM4686_CHAN_ALL;
	dev->id = init_param->id;
	dev->num_channels = ltm4686_info[dev->id].num_channels;
	dev->lin16_exp = LTM4686_LIN16_EXPONENT;

	/* Identify device */
	ret = ltm4686_match_id(dev);
	if (ret)
		goto dev_err;

	/* Configure packet error check and syncing enable */
	ret = ltm4686_read_byte(dev, LTM4686_CHAN_ALL,
				LTM4686_MFR_CONFIG_ALL, &byte);
	if (ret)
		goto dev_err;

	byte &= ~(LTM4686_CONFIG_ALL_PEC_BIT |
		  LTM4686_CONFIG_ALL_DIS_SYNC_BIT);
	byte |= no_os_field_prep(LTM4686_CONFIG_ALL_PEC_BIT,
				 (uint8_t)init_param->crc_en) |
		no_os_field_prep(LTM4686_CONFIG_ALL_DIS_SYNC_BIT,
				 (uint8_t)init_param->external_clk_en);

	ret = ltm4686_write_byte(dev, LTM4686_CHAN_ALL,
				 LTM4686_MFR_CONFIG_ALL, byte);
	if (ret)
		goto dev_err;

	dev->crc_en = init_param->crc_en;

	/* Populate CRC table for PEC */
	if (dev->crc_en)
		no_os_crc8_populate_msb(ltm4686_crc_table,
					LTM4686_CRC_POLYNOMIAL);

	/* Initialize GPIO for ALERT */
	ret = no_os_gpio_get_optional(&dev->alert_desc,
				      init_param->alert_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->alert_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for PGOOD */
	if (init_param->pgood_params) {
		for (i = 0; i < dev->num_channels; i++) {
			ret = no_os_gpio_get_optional(&dev->pgood_descs[i],
						      init_param->pgood_params[i]);
			if (ret)
				goto dev_err;

			ret = no_os_gpio_direction_input(dev->pgood_descs[i]);
			if (ret)
				goto dev_err;
		}
	}

	/* Initialize GPIO for RUN */
	if (init_param->run_params) {
		for (i = 0; i < dev->num_channels; i++) {
			ret = no_os_gpio_get_optional(&dev->run_descs[i],
						      init_param->run_params[i]);
			if (ret)
				goto dev_err;

			ret = no_os_gpio_direction_output(dev->run_descs[i],
							  NO_OS_GPIO_HIGH);
			if (ret)
				goto dev_err;
		}
	}

	/* Initialize GPIO for FAULT */
	if (init_param->fault_params) {
		for (i = 0; i < dev->num_channels; i++) {
			ret = no_os_gpio_get_optional(&dev->fault_descs[i],
						      init_param->fault_params[i]);
			if (ret)
				goto dev_err;

			ret = no_os_gpio_direction_output(dev->fault_descs[i],
							  NO_OS_GPIO_HIGH);
			if (ret)
				goto dev_err;
		}
	}

	/* Clear faults */
	ret = ltm4686_send_byte(dev, LTM4686_CHAN_ALL, LTM4686_CLEAR_FAULTS);
	if (ret)
		goto dev_err;

	*device = dev;

	return 0;

dev_err:
	no_os_i2c_remove(dev->i2c_desc);
	for (i = 0; i < dev->num_channels; i++) {
		no_os_gpio_remove(dev->pgood_descs[i]);
		no_os_gpio_remove(dev->run_descs[i]);
		no_os_gpio_remove(dev->fault_descs[i]);
	}
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
int ltm4686_remove(struct ltm4686_dev *dev)
{
	int ret, i;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	for (i = 0; i < dev->num_channels; i++) {
		no_os_gpio_remove(dev->pgood_descs[i]);
		no_os_gpio_remove(dev->run_descs[i]);
		no_os_gpio_remove(dev->fault_descs[i]);
	}

	no_os_free(dev);

	return ret;
}

/**
 * @brief Set page of the device.
 * 	  Page 0x0 - Channel 0
 * 	  Page 0x1 - Channel 1
 * 	  Page 0x2 - Channel 2
 * 	  Page 0x3 - Channel 3
 * 	  Page 0xff - All channels
 *
 * @param dev - Device structure
 * @param page - Page to set
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_set_page(struct ltm4686_dev *dev, int page)
{
	int ret = 0;
	uint8_t read_page;
	uint8_t tx_buf[3] = {0};
	uint8_t rx_buf[2] = {0};


	if (dev->page != page) {
		tx_buf[0] = LTM4686_PAGE;
		tx_buf[1] = page & 0xFF;

		if (dev->crc_en)
			tx_buf[2] = ltm4686_pec(dev, LTM4686_PAGE,
						(uint8_t *)&page, 1, 0);

		ret = no_os_i2c_write(dev->i2c_desc, tx_buf,
				      dev->crc_en + 2, 1);
		if (ret)
			return ret;

		ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
		if (ret)
			return ret;

		if (dev->crc_en) {
			ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
			if (ret)
				return ret;

			if (ltm4686_pec(dev, LTM4686_PAGE, rx_buf, 1, 1) !=
			    rx_buf[1])
				return -EBADMSG;

			read_page = rx_buf[0];

		} else {
			ret = no_os_i2c_read(dev->i2c_desc, &read_page, 1, 1);
			if (ret)
				return ret;
		}

		if (read_page != page)
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
int ltm4686_send_byte(struct ltm4686_dev *dev, int page, uint8_t cmd)
{
	int ret;

	if (ltm4686_cmd_is_paged(cmd)) {
		ret = ltm4686_set_page(dev, page);
		if (ret)
			return ret;
	}

	return no_os_i2c_write(dev->i2c_desc, &cmd, 1, 1);
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
int ltm4686_read_byte(struct ltm4686_dev *dev, int page,
		      uint8_t cmd, uint8_t *data)
{
	int ret;
	uint8_t rx_buf[2];

	if (ltm4686_cmd_is_paged(cmd)) {
		ret = ltm4686_set_page(dev, page);
		if (ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		if (ltm4686_pec(dev, cmd, rx_buf, 1, 1) != rx_buf[1])
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
int ltm4686_write_byte(struct ltm4686_dev *dev, int page,
		       uint8_t cmd, uint8_t value)
{
	int ret;
	uint8_t tx_buf[3] = {0};

	if (ltm4686_cmd_is_paged(cmd)) {
		ret = ltm4686_set_page(dev, page);
		if (ret)
			return ret;
	}

	tx_buf[0] = cmd;
	tx_buf[1] = value;

	if (dev->crc_en)
		tx_buf[2] = ltm4686_pec(dev, cmd, &value, 1, 0);

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
int ltm4686_read_word(struct ltm4686_dev *dev, int page,
		      uint8_t cmd, uint16_t *word)
{
	int ret;
	uint8_t rx_buf[3] = {0};

	if (ltm4686_cmd_is_paged(cmd)) {
		ret = ltm4686_set_page(dev, page);
		if (ret)
			return ret;
	}

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 3, 1);
		if (ret)
			return ret;

		if (ltm4686_pec(dev, cmd, rx_buf, 2, 1) != rx_buf[2])
			return -EBADMSG;
	} else {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
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
int ltm4686_write_word(struct ltm4686_dev *dev, int page,
		       uint8_t cmd, uint16_t word)
{
	int ret;
	uint8_t tx_buf[4] = {0};

	if (ltm4686_cmd_is_paged(cmd)) {
		ret = ltm4686_set_page(dev, page);
		if (ret)
			return ret;
	}

	tx_buf[0] = cmd;
	no_os_put_unaligned_le16(word, &tx_buf[1]);

	if (dev->crc_en)
		tx_buf[3] = ltm4686_pec(dev, cmd, &tx_buf[1], 2, 0);

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
int ltm4686_read_word_data(struct ltm4686_dev *dev, int page,
			   uint8_t cmd, int *data)
{
	int ret;
	uint16_t reg;

	ret = ltm4686_read_word(dev, page, cmd, &reg);
	if (ret)
		return ret;

	return ltm4686_reg2data(dev, cmd, reg, data);
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
int ltm4686_write_word_data(struct ltm4686_dev *dev, int page,
			    uint8_t cmd, int data)
{
	int ret;
	uint16_t reg;

	ret = ltm4686_data2reg(dev, cmd, data, &reg);
	if (ret)
		return ret;

	return ltm4686_write_word(dev, page, cmd, reg);
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
int ltm4686_read_block_data(struct ltm4686_dev *dev, int page, uint8_t cmd,
			    uint8_t *data, size_t nbytes)
{
	int ret;
	uint8_t rxbuf[nbytes + 2];

	ret = ltm4686_set_page(dev, page);
	if (ret)
		return ret;

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 2, 1);
		if (ret)
			return ret;

		if ((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;

		if (ltm4686_pec(dev, cmd, rxbuf, nbytes + 1, 1) !=
		    rxbuf[nbytes + 1])
			return -EBADMSG;

		memcpy(data, &rxbuf[1], nbytes);
	} else {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 1, 1);
		if (ret)
			return ret;

		if ((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;

		memcpy(data, &rxbuf[1], nbytes);
	}

	return 0;
}


/**
 * @brief Read a value
 *
 * @param dev - Device structure
 * @param channel - Channel selected
 * @param value_type - Value type.
 * 		       Example values: LTM4686_VIN
 * 				       LTM4686_VOUT
 * 				       LTM4686_IIN
 * 				       LTM4686_IOUT
 * 				       LTM4686_TEMP
 * @param value - Address of the read value
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_read_value(struct ltm4686_dev *dev,
		       uint8_t channel,
		       enum ltm4686_value_type value_type,
		       int *value)
{
	return ltm4686_read_word_data(dev, channel,
				      (uint8_t)value_type, value);
}

/**
 * @brief Read statuses
 *
 * @param dev - Device structure
 * @param channel - Channel of the status to read
 * @param status_type - Status type.
 * 			Example values: LTM4686_STATUS_BYTE_TYPE
 * 					LTM4686_STATUS_VOUT_TYPE
 * 					LTM4686_STATUS_IOUT_TYPE
 * 					LTM4686_STATUS_INPUT_TYPE
 * 					LTM4686_STATUS_CML_TYPE
 * @param status - Address of the status structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_read_status(struct ltm4686_dev *dev,
			uint8_t channel,
			enum ltm4686_status_type status_type,
			struct ltm4686_status *status)
{
	int ret;

	if (status_type & LTM4686_STATUS_WORD_TYPE) {
		ret = ltm4686_read_word(dev, channel,
					LTM4686_STATUS_WORD, &status->word);
		if (ret)
			return ret;
	}

	if (status_type & LTM4686_STATUS_BYTE_TYPE) {
		ret = ltm4686_read_byte(dev, channel,
					LTM4686_STATUS_BYTE, &status->byte);
		if (ret)
			return ret;
	}

	if (status_type & LTM4686_STATUS_VOUT_TYPE) {
		ret = ltm4686_read_byte(dev, channel,
					LTM4686_STATUS_VOUT, &status->vout);
		if (ret)
			return ret;
	}

	if (status_type & LTM4686_STATUS_IOUT_TYPE) {
		ret = ltm4686_read_byte(dev, channel,
					LTM4686_STATUS_IOUT, &status->iout);
		if (ret)
			return ret;
	}

	if (status_type & LTM4686_STATUS_INPUT_TYPE) {
		ret = ltm4686_read_byte(dev, LTM4686_CHAN_ALL,
					LTM4686_STATUS_INPUT, &status->input);
		if (ret)
			return ret;
	}

	if (status_type & LTM4686_STATUS_TEMP_TYPE) {
		ret = ltm4686_read_byte(dev, channel,
					LTM4686_STATUS_TEMPERATURE,
					&status->temp);
		if (ret)
			return ret;
	}

	if (status_type & LTM4686_STATUS_CML_TYPE) {
		ret = ltm4686_read_byte(dev, LTM4686_CHAN_ALL,
					LTM4686_STATUS_CML, &status->cml);
		if (ret)
			return ret;
	}

	if (status_type & LTM4686_STATUS_MFR_SPECIFIC_TYPE) {
		ret = ltm4686_read_byte(dev, channel,
					LTM4686_STATUS_MFR_SPECIFIC,
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
int ltm4686_vout_value(struct ltm4686_dev *dev, uint8_t channel,
		       int vout_command, int vout_max)
{
	int ret;

	ret = ltm4686_write_word_data(dev, channel,
				      LTM4686_VOUT_COMMAND, vout_command);
	if (ret)
		return ret;

	return ltm4686_write_word_data(dev, channel,
				       LTM4686_VOUT_MAX, vout_max);
}

/**
 * @brief Set output voltage transition rate
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param tr - Transition rate in microV/ms
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_vout_tr(struct ltm4686_dev *dev, uint8_t channel, int tr)
{
	return ltm4686_write_word_data(dev, channel,
				       LTM4686_VOUT_TRANSITION_RATE, tr);
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
int ltm4686_vout_margin(struct ltm4686_dev *dev, uint8_t channel,
			int margin_low, int margin_high)
{
	int ret;

	if (margin_high < margin_low)
		return -EINVAL;

	ret = ltm4686_write_word_data(dev, channel,
				      LTM4686_VOUT_MARGIN_HIGH, margin_high);
	if (ret)
		return ret;

	return ltm4686_write_word_data(dev, channel,
				       LTM4686_VOUT_MARGIN_LOW, margin_low);
}

/**
 * @brief Set input voltage window at which power conversion will proceed
 *
 * @param dev - Device structure
 * @param vin_on - Input voltage in millivolts at which conversion will start
 * @param vin_off - Input voltage in millivolts at which conversion will stop
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_set_vin(struct ltm4686_dev *dev, int vin_on, int vin_off)
{
	int ret;

	if (vin_on < vin_off)
		return -EINVAL;

	ret = ltm4686_write_word_data(dev, LTM4686_CHAN_ALL,
				      LTM4686_VIN_ON, vin_on);
	if (ret)
		return ret;

	return ltm4686_write_word_data(dev, LTM4686_CHAN_ALL,
				       LTM4686_VIN_OFF, vin_off);
}

/**
 * @brief Set timing values
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param timing_type - Timing value type.
 * 			Example: LTM4686_TON_DELAY_TYPE
 * 				 LTM4686_TON_RISE_TYPE
 * 				 LTM4686_TOFF_DELAY_TYPE
 * 				 LTM4686_RETRY_DELAY_TYPE
 * @param time - Time value in microseconds
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_set_timing(struct ltm4686_dev *dev, uint8_t channel,
		       enum ltm4686_timing_type timing_type, int time) // in us
{
	return ltm4686_write_word_data(dev, channel, timing_type, time);
}

/**
 * @brief Set switching frequency
 *
 * @param dev - Device structure
 * @param freq - Frequency to set.
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_switch_freq(struct ltm4686_dev *dev, enum ltm4686_freq freq)
{
	int ret;

	if (dev->id == ID_LTM4673)
		return -ENOTSUP;

	ret = ltm4686_set_operation(dev, LTM4686_CHAN_ALL,
				    LTM4686_OPERATION_OFF);
	if (ret)
		return ret;

	ret = ltm4686_write_word(dev, LTM4686_CHAN_ALL,
				 LTM4686_FREQUENCY_SWITCH, (uint16_t)freq);
	if (ret)
		return ret;

	return ltm4686_set_operation(dev, LTM4686_CHAN_ALL,
				     LTM4686_OPERATION_ON);
}


/**
 * @brief Sets the PWM mode for a specific channel of the LTM4686 device.
 *
 * @param dev The LTM4686 device structure.
 * @param channel The channel number.
 * @param pwm_mode The PWM mode to be set.
 * @return 0 on success, negative error code on failure.
 */
int ltm4686_pwm_mode(struct ltm4686_dev *dev, uint8_t channel,
		     enum ltm4686_pwm_mode pwm_mode)
{
	int ret;
	uint8_t temp_val;

	if (dev->id == ID_LTM4673)
		return -ENOTSUP;

	ret = ltm4686_read_byte(dev, channel,
				LTM4686_MFR_PWM_MODE, &temp_val);
	if (ret)
		return ret;

	temp_val &= ~(LTM4686_PWM_OP_MODE_BIT);
	temp_val |= no_os_field_prep(LTM4686_PWM_OP_MODE_BIT, pwm_mode);

	return ltm4686_write_byte(dev, channel, LTM4686_MFR_PWM_MODE, temp_val);
}

/**
 * @brief Set channel operation
 *
 * @param dev - Device structure
 * @param channel - Channel
 * @param operation - Operation.
 * 		      Accepted values are: LTM4686_OPERATION_OFF
 * 					   LTM4686_OPERATION_ON
 * 					   LTM4686_OPERATION_MARGIN_HIGH
 * 					   LTM4686_OPERATION_MARGIN_LOW
 * 					   LTM4686_OPERATION_SEQ_OFF
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_set_operation(struct ltm4686_dev *dev, uint8_t channel,
			  enum ltm4686_operation_type operation)
{
	return ltm4686_write_byte(dev, channel, LTM4686_OPERATION,
				  (uint8_t)operation);
}

/**
 * @brief Perform commands for non-volatile memory/EEPROM
 *
 * @param dev - Device structure
 * @param cmd - NVM commands.
 * 		Example: LTM4686_STORE_USER
 * 			 LTM4686_COMPARE_USER
 * 			 LTM4686_RESTORE_USER
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_nvm_cmd(struct ltm4686_dev *dev, enum ltm4686_nvm_cmd_type cmd)
{
	return ltm4686_send_byte(dev, LTM4686_CHAN_ALL, (uint8_t)cmd);
}

/**
 * @brief Perform a device software reset
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4686_software_reset(struct ltm4686_dev *dev)
{
	return ltm4686_send_byte(dev, LTM4686_CHAN_ALL, LTM4686_MFR_RESET);
}
