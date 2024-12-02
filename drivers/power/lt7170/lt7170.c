/*******************************************************************************
*   @file   lt7170.c
*   @brief  Source code of the LT7170 Driver
*   @authors Cherrence Sarip (cherrence.sarip@analog.com)
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

#include "lt7170.h"

NO_OS_DECLARE_CRC8_TABLE(lt7170_crc_table);

static const struct lt7170_chip_info lt7170_info[] = {
	[ID_LT7170] = {
		.name = "LT7170",
		.name_size = 6,
	},
	[ID_LT7170_1] = {
		.name = "LT7170-1",
		.name_size = 8,
	},
	[ID_LT7171] = {
		.name = "LT7171",
		.name_size = 6,
	},
	[ID_LT7171_1] = {
		.name = "LT7171-1",
		.name_size = 8
	},
};

/**
 * @brief Converts value to IEEE754 register data
 * @param dev - Device structure
 * @param data - Value to convert
 * @param reg - Address of converted register data
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7170_data2reg_ieee754(struct lt7170_dev *dev, int data,
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
		data = NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale / MILLI);
	else
		data *= (int)MILLI / scale;

	/* Reduce large mantissa until it fits into 10 bit */
	while ((data > LT7170_IEEE754_MAX_MANTISSA * scale) && exponent < 30) {
		exponent++;
		data >>= 1;
	}
	/*
	 * Increase small mantissa to generate valid 'normal'
	 * number
	 */
	while ((data < LT7170_IEEE754_MIN_MANTISSA * scale) && exponent > 1) {
		exponent--;
		data <<= 1;
	}

	/* Convert mantissa from scaled-units to units */
	mantissa = NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale);

	mantissa = no_os_clamp(mantissa, LT7170_IEEE754_MIN_MANTISSA,
			       LT7170_IEEE754_MAX_MANTISSA);

	/* Convert to sign, 5 bit exponent, 10 bit mantissa */
	*reg = no_os_field_prep(LT7170_IEEE754_SIGN_BIT, sign) |
	       no_os_field_prep(LT7170_IEEE754_MANTISSA_MSK, mantissa) |
	       no_os_field_prep(LT7170_IEEE754_EXPONENT_MSK, exponent);

	return 0;
}

/**
 * @brief Converts raw IEEE754 register data to its actual value
 * @param dev - Device structure
 * @param reg - IEEE754 data to convert
 * @param data - Address of value
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7170_reg2data_ieee754(struct lt7170_dev *dev, uint16_t reg,
				   int *data, int scale)
{
	int val;
	int exponent;
	bool sign;

	sign = no_os_field_get(LT7170_IEEE754_SIGN_BIT, reg);
	exponent = no_os_field_get(LT7170_IEEE754_EXPONENT_MSK, reg);
	val = no_os_field_get(LT7170_IEEE754_MANTISSA_MSK, reg);

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
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Value to convert
 * @param reg - Address of register data
 * @return 0 in case of success, negative error code otherwise
 */
static int lt7170_data2reg(struct lt7170_dev *dev, uint8_t cmd, int data,
			   uint16_t *reg)
{
	switch (cmd) {
	case LT7170_VOUT_COMMAND:
	case LT7170_VOUT_MAX:
	case LT7170_VOUT_MARGIN_HIGH:
	case LT7170_VOUT_MARGIN_LOW:
	case LT7170_FREQUENCY_SWITCH:
	case LT7170_VIN_ON:
	case LT7170_VIN_OFF:
	case LT7170_VOUT_OV_FAULT_LIMIT:
	case LT7170_VOUT_OV_WARN_LIMIT:
	case LT7170_VOUT_UV_WARN_LIMIT:
	case LT7170_VOUT_UV_FAULT_LIMIT:
	case LT7170_VIN_UV_WARN_LIMIT:
	case LT7170_READ_VIN:
	case LT7170_READ_VOUT:
	case LT7170_MFR_READ_EXTVCC:
	case LT7170_MFR_READ_ITH:
	case LT7170_MFR_VOUT_PEAK:
	case LT7170_MFR_VIN_PEAK:
	case LT7170_IOUT_CAL_OFFSET:
	case LT7170_IOUT_OC_WARN_LIMIT:
	case LT7170_OT_FAULT_LIMIT:
	case LT7170_OT_WARN_LIMIT:
	case LT7170_READ_IOUT:
	case LT7170_READ_TEMPERATURE_1:
	case LT7170_READ_FREQUENCY:
	case LT7170_MFR_IOUT_PEAK:
	case LT7170_MFR_RETRY_DELAY:
	case LT7170_MFR_TEMPERATURE_1_PEAK:
	case LT7170_MFR_READ_PWM_CFG:
	case LT7170_MFR_READ_VOUT_CFG:
	case LT7170_MFR_DISCHARGE_THRESHOLD:
	case LT7170_MFR_PGOOD_DELAY:
	case LT7170_MFR_NOT_PGOOD_DELAY:
		return lt7170_data2reg_ieee754(dev, data, reg, MILLI);
	default:
		return -EINVAL;
	}
}

static int lt7170_reg2data(struct lt7170_dev *dev, uint8_t cmd,
			   uint16_t reg, int *data)
{
	switch (cmd) {
	case LT7170_VOUT_COMMAND:
	case LT7170_VOUT_MAX:
	case LT7170_VOUT_MARGIN_HIGH:
	case LT7170_VOUT_MARGIN_LOW:
	case LT7170_FREQUENCY_SWITCH:
	case LT7170_VIN_ON:
	case LT7170_VIN_OFF:
	case LT7170_VOUT_OV_FAULT_LIMIT:
	case LT7170_VOUT_OV_WARN_LIMIT:
	case LT7170_VOUT_UV_WARN_LIMIT:
	case LT7170_VOUT_UV_FAULT_LIMIT:
	case LT7170_VIN_UV_WARN_LIMIT:
	case LT7170_READ_VIN:
	case LT7170_READ_VOUT:
	case LT7170_MFR_READ_EXTVCC:
	case LT7170_MFR_READ_ITH:
	case LT7170_MFR_VOUT_PEAK:
	case LT7170_MFR_VIN_PEAK:
	case LT7170_IOUT_CAL_OFFSET:
	case LT7170_IOUT_OC_WARN_LIMIT:
	case LT7170_OT_FAULT_LIMIT:
	case LT7170_OT_WARN_LIMIT:
	case LT7170_READ_IOUT:
	case LT7170_READ_TEMPERATURE_1:
	case LT7170_READ_FREQUENCY:
	case LT7170_MFR_READ_ASEL:
	case LT7170_MFR_IOUT_PEAK:
	case LT7170_MFR_RETRY_DELAY:
	case LT7170_MFR_TEMPERATURE_1_PEAK:
	case LT7170_MFR_READ_PWM_CFG:
	case LT7170_MFR_READ_VOUT_CFG:
	case LT7170_MFR_DISCHARGE_THRESHOLD:
	case LT7170_MFR_PGOOD_DELAY:
	case LT7170_MFR_NOT_PGOOD_DELAY:
		return lt7170_reg2data_ieee754(dev, reg, data, MILLI);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Perform packet-error checking via CRC
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param buf - Pointer to the first packet buffer
 * @param nbytes - Packet buffer size
 * @param op - Operation performed. 0 for write, 1 for read
 * @return PEC code of the PMBus packet
 */
static uint8_t lt7170_pec(struct lt7170_dev *dev, uint8_t cmd, uint8_t *buf,
			  size_t nbytes, uint8_t op)
{
	uint8_t crc_buf[nbytes + op + 2];

	crc_buf[0] = (dev->i2c_desc->slave_address << 1);
	crc_buf[1] = cmd;
	if (op)
		crc_buf[2] = (dev->i2c_desc->slave_address << 1) | 1;

	if (buf != NULL && nbytes > 0)
		memcpy(&crc_buf[2 + op], buf, nbytes);

	return no_os_crc8(lt7170_crc_table, crc_buf, nbytes + op + 2, 0);
}

/**
 * @brief Initialize the device structure
 * @param device - Device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_init(struct lt7170_dev **device,
		struct lt7170_init_param *init_param)
{
	struct lt7170_dev *dev;
	int ret;
	uint16_t word;
	uint8_t block[lt7170_info[init_param->chip_id].name_size];

	dev = (struct lt7170_dev *)no_os_calloc(1, sizeof(struct lt7170_dev));
	if (!dev) {
		ret = -ENOMEM;
	}

	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto i2c_err;

	ret = lt7170_read_block_data(dev, LT7170_IC_DEVICE_ID, block,
				     lt7170_info[init_param->chip_id].name_size);
	if (ret)
		goto dev_err;

	if (strncmp((char *)block, lt7170_info[init_param->chip_id].name,
		    lt7170_info[init_param->chip_id].name_size)) {
		ret = -EIO;
		goto dev_err;
	}

	/* Set PEC */
	word = no_os_field_prep(LT7170_CONFIG_ALL_PEC_BIT,
				(uint8_t)init_param->crc_en);
	ret = lt7170_write_word(dev, LT7170_MFR_CONFIG_ALL, word);
	if (ret)
		goto dev_err;

	dev->crc_en = init_param->crc_en;

	/* Populate CRC table for PEC */
	if (dev->crc_en)
		no_os_crc8_populate_msb(lt7170_crc_table,
					LT7170_CRC_POLYNOMIAL);

	/* Initialize GPIO for PGOOD */
	ret = no_os_gpio_get_optional(&dev->pg_desc, init_param->pg_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->pg_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for RUN */
	ret = no_os_gpio_get_optional(&dev->run_desc, init_param->run_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_output(dev->run_desc, NO_OS_GPIO_HIGH);
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

	/* Initialize GPIO for FAULT */
	ret = no_os_gpio_get_optional(&dev->fault_desc,
				      init_param->fault_param);
	if (ret)
		goto dev_err;

	if (init_param->fault_cfg == LT7170_FAULT_PIN_OUTPUT) {
		ret = no_os_gpio_direction_output(dev->fault_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto dev_err;
	} else {
		ret = no_os_gpio_direction_input(dev->fault_desc);
		if (ret)
			goto dev_err;
	}

	/* Initialize external clock for synchronization if used */
	if (init_param->external_clk_en) {
		ret = no_os_pwm_init(&dev->sync_desc, init_param->sync_param);
		if (ret)
			goto dev_err;

		ret = lt7170_sync_config(dev, false, true);
		if (ret)
			goto dev_err;
	}

	/* Set operation */
	ret = lt7170_set_operation(dev, LT7170_OPERATION_ON);
	if (ret)
		goto dev_err;

	/* Clear faults */
	ret = lt7170_clear_faults(dev);
	if (ret)
		goto dev_err;

	*device = dev;

	return 0;

dev_err:
	no_os_i2c_remove(dev->i2c_desc);
i2c_err:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free or remove device instance
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_remove(struct lt7170_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->pg_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->run_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->alert_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->fault_desc);
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
 * @brief Send a PMBus command to the device
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_send_byte(struct lt7170_dev *dev, uint8_t cmd)
{
	int ret;
	uint8_t tx_buf[2] = {0};

	tx_buf[0] = cmd;
	if (dev->crc_en)
		tx_buf[1] = lt7170_pec(dev, cmd, NULL, 0, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 1, 1);
}

/**
 * @brief Perform a raw PMBus read byte operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of the byte read
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_read_byte(struct lt7170_dev *dev, uint8_t cmd, uint8_t *data)
{
	int ret;
	uint8_t rx_buf[2];

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		if (lt7170_pec(dev, cmd, rx_buf, 1, 1) != rx_buf[1])
			return -EBADMSG;

		*data = rx_buf[0];
		return ret;
	} else {
		return no_os_i2c_read(dev->i2c_desc, data, 1, 1);
	}
}

/**
 * @brief Perform a raw PMBus write byte operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param value - Byte to be written
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_write_byte(struct lt7170_dev *dev, uint8_t cmd, uint8_t value)
{
	int ret;
	uint8_t tx_buf[3] = {0};

	tx_buf[0] = cmd;
	tx_buf[1] = value;

	if (dev->crc_en)
		tx_buf[2] = lt7170_pec(dev, cmd, &value, 1, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 2, 1);
}

/**
 * @brief Perform a raw PMBus read word operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param word - Address of the read word
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_read_word(struct lt7170_dev *dev, uint8_t cmd, uint16_t *word)
{
	int ret;
	uint8_t rx_buf[3] = {0};

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 3, 1);
		if (ret)
			return ret;

		if (lt7170_pec(dev, cmd, rx_buf, 2, 1) != rx_buf[2])
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
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param word - Word to write
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_write_word(struct lt7170_dev *dev, uint8_t cmd, uint16_t word)
{
	int ret;
	uint8_t tx_buf[4] = {0};

	tx_buf[0] = cmd;
	no_os_put_unaligned_le16(word, &tx_buf[1]);

	if (dev->crc_en)
		tx_buf[3] = lt7170_pec(dev, cmd, &tx_buf[1], 2, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 3, 1);
}

/**
 * @brief Perform a PMBus read word operation and converts to actual value
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of data read
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_read_word_data(struct lt7170_dev *dev, uint8_t cmd, int *data)
{
	int ret;
	uint16_t reg;

	ret = lt7170_read_word(dev, cmd, &reg);
	if (ret)
		return ret;

	return lt7170_reg2data(dev, cmd, reg, data);
}

/**
 * @brief Converts value to register data and do PMBus write word operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_write_word_data(struct lt7170_dev *dev, uint8_t cmd, int data)
{
	int ret;
	uint16_t reg;

	ret = lt7170_data2reg(dev, cmd, data, &reg);
	if (ret)
		return ret;

	return lt7170_write_word(dev, cmd, reg);
}

/**
 * @brief Perform a PMBus read block operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of the read block
 * @param nbytes - Size of the block in bytes
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_read_block_data(struct lt7170_dev *dev, uint8_t cmd, uint8_t *data,
			   size_t nbytes)
{
	int ret;
	uint8_t rxbuf[nbytes + 2];

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 2, 1);
		if (ret)
			return ret;

		if ((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;

		if (lt7170_pec(dev, cmd, rxbuf, nbytes + 1, 1) !=
		    rxbuf[nbytes + 1])
			return -EBADMSG;
	} else {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 1, 1);
		if (ret)
			return ret;

		if ((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;
	}

	memcpy(data, &rxbuf[1], nbytes);

	return 0;
}

/**
 * @brief Read a value
 * @param dev - Device structure
 * @param value_type - Value type.
 * 		       Example values: LT7170_VIN
 * 				       LT7170_VOUT
 * 				       LT7170_IOUT
 * 				       LT7170_TEMP
 * 				       LT7170_FREQUENCY
 * @param value - Address of the read value
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_read_value(struct lt7170_dev *dev, enum lt7170_value_type value_type,
		      int *value)
{
	return lt7170_read_word_data(dev, (uint8_t)value_type, value);
}

/**
 * @brief Read statuses
 * @param dev - Device structure
 * @param status_type - Status type.
 * 			Example values: LT7170_STATUS_BYTE_TYPE
 * 					LT7170_STATUS_VOUT_TYPE
 * 					LT7170_STATUS_IOUT_TYPE
 * 					LT7170_STATUS_INPUT_TYPE
 * 					LT7170_STATUS_CML_TYPE
 * @param status - Address of the status structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_read_status(struct lt7170_dev *dev,
		       enum lt7170_status_type status_type,
		       struct lt7170_status *status)
{
	int ret;

	if (status_type & LT7170_STATUS_WORD_TYPE) {
		ret = lt7170_read_word(dev, LT7170_STATUS_WORD,
				       &status->word);
		if (ret)
			return ret;
	}

	if (status_type & LT7170_STATUS_BYTE_TYPE) {
		ret = lt7170_read_byte(dev, LT7170_STATUS_BYTE,
				       &status->byte);
		if (ret)
			return ret;
	}

	if (status_type & LT7170_STATUS_VOUT_TYPE) {
		ret = lt7170_read_byte(dev, LT7170_STATUS_VOUT,
				       &status->vout);
		if (ret)
			return ret;
	}

	if (status_type & LT7170_STATUS_IOUT_TYPE) {
		ret = lt7170_read_byte(dev, LT7170_STATUS_IOUT,
				       &status->iout);
		if (ret)
			return ret;
	}

	if (status_type & LT7170_STATUS_INPUT_TYPE) {
		ret = lt7170_read_byte(dev, LT7170_STATUS_INPUT,
				       &status->input);
		if (ret)
			return ret;
	}

	if (status_type & LT7170_STATUS_TEMP_TYPE) {
		ret = lt7170_read_byte(dev,
				       LT7170_STATUS_TEMPERATURE,
				       &status->temp);
		if (ret)
			return ret;
	}

	if (status_type & LT7170_STATUS_CML_TYPE) {
		ret = lt7170_read_byte(dev,
				       LT7170_STATUS_CML, &status->cml);
		if (ret)
			return ret;
	}

	if (status_type & LT7170_STATUS_MFR_SPECIFIC_TYPE) {
		ret = lt7170_read_byte(dev,
				       LT7170_STATUS_MFR_SPECIFIC,
				       &status->mfr_specific);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set output voltage and its upper limit
 * @param dev - Device structure
 * @param vout_command - Output voltage in millivolts
 * @param vout_max - Output voltage upper limit in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_vout_value(struct lt7170_dev *dev, int vout_command, int vout_max)
{
	int ret;

	ret = lt7170_write_word_data(dev, LT7170_VOUT_COMMAND, vout_command);
	if (ret)
		return ret;

	return lt7170_write_word_data(dev, LT7170_VOUT_MAX, vout_max);
}

/**
 * @brief Set output voltage transition rate
 * @param dev - Device structure
 * @param tr - Transition rate in V/s or mV/ms
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_vout_tr(struct lt7170_dev *dev, int tr)
{
	return lt7170_write_word_data(dev, LT7170_VOUT_TRANSITION_RATE, tr);
}

/**
 * @brief Set output voltage margin
 * @param dev - Device structure
 * @param margin_high - Upper margin in millivolts
 * @param margin_low - Lower margin in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_vout_margin(struct lt7170_dev *dev, int margin_high, int margin_low)
{
	int ret;

	if (margin_high < margin_low)
		return -EINVAL;

	ret = lt7170_write_word_data(dev, LT7170_VOUT_MARGIN_HIGH, margin_high);
	if (ret)
		return ret;

	return lt7170_write_word_data(dev, LT7170_VOUT_MARGIN_LOW, margin_low);
}

/**
 * @brief Set input voltage window at which power conversion will proceed
 * @param dev - Device structure
 * @param vin_on - Input voltage in millivolts at which conversion will start
 * @param vin_off - Input voltage in millivolts at which conversion will stop
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_set_vin(struct lt7170_dev *dev, int vin_on, int vin_off)
{
	int ret;

	if (vin_on < vin_off)
		return -EINVAL;

	if (vin_on < LT7170_VIN_ON_MIN || vin_on > LT7170_VIN_ON_MAX)
		return -EINVAL;

	if (vin_off < LT7170_VIN_OFF_MIN || vin_off > LT7170_VIN_OFF_MAX)
		return -EINVAL;

	ret = lt7170_write_word_data(dev, LT7170_VIN_ON, vin_on);
	if (ret)
		return ret;

	return lt7170_write_word_data(dev, LT7170_VIN_OFF, vin_off);
}

/**
 * @brief Set timing values
 * @param dev - Device structure
 * @param timing_type - Timing value type.
 * 			Example: LT7170_TON_DELAY_TYPE
 * 				 LT7170_TON_RISE_TYPE
 * 				 LT7170_TOFF_DELAY_TYPE
 * 				 LT7170_RETRY_DELAY_TYPE
 * @param time - Time value in microseconds
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_set_timing(struct lt7170_dev *dev,
		      enum lt7170_timing_type timing_type, int time) // in us
{
	return lt7170_write_word_data(dev, timing_type, time);
}

/**
 * @brief Set switching frequency
 * @param dev - Device structure
 * @param freq - Frequency to set in Hz. Value should be between 400000 Hz and
 * 		 4000000 Hz.
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_switch_freq(struct lt7170_dev *dev, int freq)
{
	if (freq < LT7170_FREQ_MIN || freq > LT7170_FREQ_MAX)
		return -EINVAL;

	freq *= MILLI;
	return lt7170_write_word_data(dev, LT7170_FREQUENCY_SWITCH, freq);
}

/**
 * @brief Set output PWM mode
 * @param dev - Device structure
 * @param pwm_mode - PWM mode of the output channel.
 * 		     Example: LT7170_PWM_FORCED_CONTINUOUS_MODE
 * 			      LT7170_PWM_PULSE_SKIP_MODE
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_pwm_mode(struct lt7170_dev *dev, enum lt7170_pwm_mode pwm_mode)
{
	int ret;
	uint16_t temp_word;

	ret = lt7170_read_word(dev, LT7170_MFR_PWM_MODE, &temp_word);
	if (ret)
		return ret;

	temp_word &= ~(LT7170_PWM_OP_MODE_BIT);
	temp_word |= no_os_field_prep(LT7170_PWM_OP_MODE_BIT, pwm_mode);

	return lt7170_write_word(dev, LT7170_MFR_PWM_MODE, temp_word);
}

/**
 * @brief Set output PWM phase
 * @param dev - Device structure
 * @param phase - Phase in milli-degrees
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_pwm_phase(struct lt7170_dev *dev, int phase)
{
	phase %= LT7170_PHASE_FULL_MILLI;
	if (phase < 0)
		phase += LT7170_PHASE_FULL_MILLI;

	return lt7170_write_word_data(dev, LT7170_MFR_PWM_PHASE, phase);
}

/**
 * @brief Set overvalue and undervalue limits
 * @param dev - Device structure
 * @param limit - Limit value type.
 * 			Example: LT7170_VOUT_OV_FAULT_LIMIT_TYPE
 * 				 LT7170_OT_WARN_LIMIT_TYPE
 * 				 LT7170_TON_MAX_FAULT_LIMIT_TYPE
 * 				 LT7170_VIN_UV_WARN_LIMIT_TYPE
 * @param limit_val - Limit value in milli-units for voltage and current, and
 * 		      microseconds for timing.
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_set_limit(struct lt7170_dev *dev, enum lt7170_limit_type limit,
		     int limit_val)
{
	return lt7170_write_word_data(dev, (uint8_t)limit, limit_val);
}

/**
 * @brief Set channel operation
 * @param dev - Device structure
 * @param operation - Operation.
 * 		      Accepted values are: LT7170_OPERATION_OFF
 * 					   LT7170_OPERATION_ON
 * 					   LT7170_OPERATION_MARGIN_HIGH
 * 					   LT7170_OPERATION_MARGIN_LOW
 * 					   LT7170_OPERATION_SEQ_OFF
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_set_operation(struct lt7170_dev *dev,
			 enum lt7170_operation_type operation)
{
	return lt7170_write_byte(dev, LT7170_OPERATION, (uint8_t)operation);
}

/**
 * @brief Set channel state using the RUN pin.
 * @param dev - Device structure
 * @param state - true for ON, false for OFF
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_set_channel_state(struct lt7170_dev *dev, bool state)
{
	return no_os_gpio_direction_output(dev->run_desc, state);
}

/**
 * @brief Configure SYNC pin for clock synchronization
 * @param dev - Device structure
 * @param input_clk - Enable to use SYNC clock input
 * @param output_clk - Enable to use SYNC output clock
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_sync_config(struct lt7170_dev *dev, bool input_clk, bool output_clk)
{
	int ret;
	uint8_t temp;

	ret = lt7170_read_byte(dev, LT7170_MFR_SYNC_CONFIG, &temp);
	if (ret)
		return ret;

	temp &= ~(LT7170_SYNC_CLK_INPUT_BIT | LT7170_SYNC_CLK_OUTPUT_BIT);
	temp |= no_os_field_prep(LT7170_SYNC_CLK_INPUT_BIT, !input_clk) |
		no_os_field_prep(LT7170_SYNC_CLK_OUTPUT_BIT, output_clk);

	return lt7170_write_byte(dev,
				 LT7170_MFR_SYNC_CONFIG, temp);
}

/**
 * @brief Configure ADC control
 * @param dev - Device structure
 * @param low_freq_telemetry - Enable for low frequency telemetry (every 100ms)
 * @param debug_telemetry - Enable for debug telemetry with EXTVcc and Ith
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_adc_control(struct lt7170_dev *dev, bool low_freq_telemetry,
		       bool debug_telemetry)
{
	return lt7170_write_byte(dev, LT7170_MFR_ADC_CONTROL,
				 no_os_field_prep(LT7170_ADC_CTRL_DEBUG_BIT,
						 (uint8_t)debug_telemetry) |
				 no_os_field_prep(LT7170_ADC_CTRL_LOW_FREQ_BIT,
						 (uint8_t)low_freq_telemetry));
}

/**
 * @brief Perform commands for non-volatile memory (NVM)
 * @param dev - Device structure
 * @param cmd - NVM commands.
 * 		Example: LT7170_LOCK_USER
 * 			 LT7170_UNLOCK_USER
 * 			 LT7170_STORE_USER
 * 			 LT7170_COMPARE_USER
 * 			 LT7170_RESTORE_USER
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_nvm_cmd(struct lt7170_dev *dev, enum lt7170_nvm_cmd_type cmd)
{
	int ret;

	switch (cmd) {
	case LT7170_LOCK_USER:
		return lt7170_write_byte(dev, LT7170_MFR_NVM_USER_WP, 1);
	case LT7170_UNLOCK_USER:
		return lt7170_write_byte(dev, LT7170_MFR_NVM_USER_WP, 0);
	case LT7170_STORE_USER:
	case LT7170_COMPARE_USER:
		return lt7170_send_byte(dev, (uint8_t)cmd);
	case LT7170_RESTORE_USER:
		ret = lt7170_send_byte(dev, (uint8_t)cmd);
		if (ret)
			return ret;
		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}

/**
 * @brief Clear all asserted faults
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_clear_faults(struct lt7170_dev *dev)
{
	return lt7170_send_byte(dev, LT7170_CLEAR_FAULTS);
}

/**
 * @brief Perform a device software reset
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt7170_software_reset(struct lt7170_dev *dev)
{
	int ret;

	ret = lt7170_send_byte(dev, LT7170_MFR_RESET);
	if (ret)
		return ret;

	return 0;
}
