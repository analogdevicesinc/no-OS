/*******************************************************************************
*   @file   ltp8800.c
*   @brief  Source code of the LTP8800 Driver
*   @authors Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
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

#include "ltp8800.h"

NO_OS_DECLARE_CRC8_TABLE(ltp8800_crc_table);

/**
 * @brief Converts value to LINEAR16 register data
 *
 * @param dev - Device structure
 * @param data - Value to convert
 * @param reg - Address of converted register data
 * @param scale - Value scaling factor
 * @return 0 in case of success, negative error code otherwise
 */
static int ltp8800_data2reg_linear16(struct ltp8800_dev *dev, int data,
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
static int ltp8800_data2reg_linear11(struct ltp8800_dev *dev, int data,
				     uint16_t *reg, int scale)
{
	int exp = 0, mant = 0;
	uint8_t negative = 0;

	if (data < 0) {
		negative = 1;
		data = -data;
	}

	/* If value too high, continuously do m/2 until m < 1023. */
	while (data >= LTP8800_LIN11_MANTISSA_MAX * scale &&
	       exp < LTP8800_LIN11_EXPONENT_MAX) {
		exp++;
		data >>= 1;
	}

	/* If value too low, increase mantissa. */
	while (data < LTP8800_LIN11_MANTISSA_MIN * scale &&
	       exp > LTP8800_LIN11_EXPONENT_MIN) {
		exp--;
		data <<= 1;
	}

	mant = no_os_clamp(NO_OS_DIV_ROUND_CLOSEST_ULL(data, scale),
			   0, 0x3FF);
	if (negative)
		mant = -mant;

	*reg = no_os_field_prep(LTP8800_LIN11_MANTISSA_MSK, mant) |
	       no_os_field_prep(LTP8800_LIN11_EXPONENT_MSK, exp);

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
static int ltp8800_reg2data_linear16(struct ltp8800_dev *dev, uint16_t reg,
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
static int ltp8800_reg2data_linear11(struct ltp8800_dev *dev, uint16_t reg,
				     int *data, int scale)
{
	int val, exp, mant;

	exp = LTP8800_LIN11_EXPONENT(reg);
	mant = LTP8800_LIN11_MANTISSA(reg);

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
static int ltp8800_data2reg(struct ltp8800_dev *dev, uint16_t cmd,
			    int data, uint16_t *reg)
{
	switch (cmd) {
	case LTP8800_VOUT_COMMAND:
	case LTP8800_READ_VOUT:
		return ltp8800_data2reg_linear16(dev, data, reg,
						 MILLIVOLT_PER_VOLT);
	case LTP8800_VOUT_SCALE_LOOP:
	case LTP8800_VOUT_SCALE_MONITOR:
	case LTP8800_FREQUENCY_SWITCH:
	case LTP8800_VIN_ON:
	case LTP8800_VIN_OFF:
	case LTP8800_VIN_OV_FAULT_LIMIT:
	case LTP8800_VIN_UV_FAULT_LIMIT:
	case LTP8800_IIN_OC_FAULT_LIMIT:
	case LTP8800_READ_VIN:
	case LTP8800_READ_IIN:
	case LTP8800_READ_IOUT:
	case LTP8800_READ_TEMPERATURE_2:
	case LTP8800_READ_TEMPERATURE_3:
	case LTP8800_READ_DUTY_CYCLE:
	case LTP8800_READ_FREQUENCY:
		return ltp8800_data2reg_linear11(dev, data, reg,
						 MILLI);
	case LTP8800_POUT_OP_FAULT_LIMIT:
	case LTP8800_READ_POUT:
		return ltp8800_data2reg_linear11(dev, data, reg,
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
static int ltp8800_reg2data(struct ltp8800_dev *dev, uint16_t cmd,
			    uint16_t reg, int *data)
{
	switch (cmd) {
	case LTP8800_VOUT_COMMAND:
	case LTP8800_READ_VOUT:
		return ltp8800_reg2data_linear16(dev, reg, data,
						 MILLIVOLT_PER_VOLT);
	case LTP8800_VOUT_SCALE_LOOP:
	case LTP8800_VOUT_SCALE_MONITOR:
	case LTP8800_FREQUENCY_SWITCH:
	case LTP8800_VIN_ON:
	case LTP8800_VIN_OFF:
	case LTP8800_VIN_OV_FAULT_LIMIT:
	case LTP8800_VIN_UV_FAULT_LIMIT:
	case LTP8800_IIN_OC_FAULT_LIMIT:
	case LTP8800_READ_VIN:
	case LTP8800_READ_IIN:
	case LTP8800_READ_IOUT:
	case LTP8800_READ_TEMPERATURE_2:
	case LTP8800_READ_TEMPERATURE_3:
	case LTP8800_READ_DUTY_CYCLE:
	case LTP8800_READ_FREQUENCY:
		return ltp8800_reg2data_linear11(dev, reg, data,
						 MILLI);
	case LTP8800_POUT_OP_FAULT_LIMIT:
	case LTP8800_READ_POUT:
		return ltp8800_reg2data_linear11(dev, reg, data,
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
static uint8_t ltp8800_pec(struct ltp8800_dev *dev, uint16_t cmd, uint8_t *buf,
			   size_t nbytes, uint8_t op)
{
	uint8_t crc_buf[nbytes + op + 3];
	uint8_t i = 0;

	crc_buf[i++] = (dev->i2c_desc->slave_address << 1);

	if (cmd >= LTP8800_EXTENDED_COMMAND_BEGIN)
		crc_buf[i++] = no_os_field_get(LTP8800_COMMAND_MSB_MSK, cmd);

	crc_buf[i++] = no_os_field_get(LTP8800_COMMAND_LSB_MSK, cmd);
	if (op)
		crc_buf[i++] = (dev->i2c_desc->slave_address << 1) | 1;

	if (buf != NULL && nbytes > 0)
		memcpy(&crc_buf[i], buf, nbytes);

	return no_os_crc8(ltp8800_crc_table, crc_buf, nbytes + i, 0);
}

/**
 * @brief Initialize the device structure
 *
 * @param device - Device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_init(struct ltp8800_dev **device,
		 struct ltp8800_init_param *init_param)
{
	struct ltp8800_dev *dev;
	int ret;
	uint8_t block[2];
	uint8_t ic_device_id[2] = LTP8800_IC_DEVICE_ID_VALUE;
	uint8_t val;

	dev = (struct ltp8800_dev *)no_os_calloc(1, sizeof(struct ltp8800_dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto i2c_err;

	/* Identify device */
	ret = ltp8800_read_block_data(dev, LTP8800_IC_DEVICE_ID, block, 2);
	if (ret)
		goto dev_err;
	if (strncmp((char *)block, (char *)ic_device_id, 2)) {
		ret = -EIO;
		goto dev_err;
	}

	dev->crc_en = init_param->crc_en;

	/* Populate CRC table for PEC */
	if (dev->crc_en)
		no_os_crc8_populate_msb(ltp8800_crc_table,
					LTP8800_CRC_POLYNOMIAL);

	ret = ltp8800_read_byte(dev, LTP8800_VOUT_MODE, &val);
	if (ret)
		goto dev_err;

	dev->lin16_exp = (int)no_os_sign_extend32(val, 4);

	/* Configure WRITE PROTECT */
	ret = ltp8800_read_byte(dev, LTP8800_WRITE_PROTECT, &val);
	if (ret)
		goto dev_err;

	val &= ~(LTP8800_WRITE_PROTECT_1_BIT);
	val |= no_os_field_prep(LTP8800_WRITE_PROTECT_1_BIT,
				init_param->write_protect_en);

	ret = ltp8800_write_byte(dev, LTP8800_WRITE_PROTECT, val);
	if (ret)
		goto dev_err;

	dev->write_protect_en = init_param->write_protect_en;

	if (!dev->write_protect_en) {
		/* Configure synchronization */
		ret = ltp8800_sync_config(dev, init_param->sync_en);
		if (ret)
			goto dev_err;

		/* Set PolyPhase order */
		ret = ltp8800_interleave_order(dev,
					       init_param->polyphase_order);
		if (ret)
			goto dev_err;
	}

	/* Initialize GPIO for ALERT */
	ret = no_os_gpio_get_optional(&dev->smbalert_desc,
				      init_param->smbalert_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->smbalert_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for CTRL */
	ret = no_os_gpio_get_optional(&dev->ctrl_desc,
				      init_param->ctrl_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_output(dev->ctrl_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto dev_err;

	/* Initialize external clock for synchronization if used */
	if (init_param->external_clk_en) {
		ret = no_os_pwm_init(&dev->ext_clk_desc,
				     init_param->ext_clk_param);
		if (ret)
			goto dev_err;
	}

	*device = dev;

	return 0;

dev_err:
	no_os_gpio_remove(dev->ctrl_desc);
	no_os_gpio_remove(dev->smbalert_desc);
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
int ltp8800_remove(struct ltp8800_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->smbalert_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->ctrl_desc);
	if (ret)
		return ret;

	if (dev->ext_clk_desc) {
		ret = no_os_pwm_remove(dev->ext_clk_desc);
		if (ret)
			return ret;
	}

	no_os_free(dev);

	return ret;
}

/**
 * @brief Send a PMBus command to the device
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @return 0 in case of success, negative error code otherwise
 */

int ltp8800_send_byte(struct ltp8800_dev *dev, uint16_t cmd)
{
	uint8_t tx_buf[3];
	uint8_t i = 0;

	if (!dev)
		return -EINVAL;

	tx_buf[i++] = no_os_field_get(LTP8800_COMMAND_LSB_MSK, cmd);

	if (cmd >= LTP8800_EXTENDED_COMMAND_BEGIN)
		tx_buf[i++] = LTP8800_EXTENDED_COMMAND_PREFIX;

	if (dev->crc_en)
		tx_buf[i++] = ltp8800_pec(dev, cmd, NULL, 0, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, i, 1);
}

/**
 * @brief Perform a raw PMBus read byte operation
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of the byte read
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_read_byte(struct ltp8800_dev *dev, uint16_t cmd, uint8_t *data)
{
	int ret;
	uint8_t cmd_buf[2];
	uint8_t rx_buf[2];

	if (!dev)
		return -EINVAL;

	if (cmd >= LTP8800_EXTENDED_COMMAND_BEGIN) {
		no_os_put_unaligned_be16(cmd, cmd_buf);
		ret = no_os_i2c_write(dev->i2c_desc, cmd_buf, 2, 0);
	} else {
		cmd_buf[0] = no_os_field_get(LTP8800_COMMAND_LSB_MSK, cmd);
		ret = no_os_i2c_write(dev->i2c_desc, cmd_buf, 1, 0);
	}
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		if (ltp8800_pec(dev, cmd, rx_buf, 1, 1) != rx_buf[1])
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
 * @param cmd - PMBus command
 * @param value - Byte to be written
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_write_byte(struct ltp8800_dev *dev, uint16_t cmd, uint8_t value)
{
	uint8_t tx_buf[4];
	uint8_t i = 0;

	if (!dev)
		return -EINVAL;

	if (cmd >= LTP8800_EXTENDED_COMMAND_BEGIN)
		tx_buf[i++] = LTP8800_EXTENDED_COMMAND_PREFIX;

	tx_buf[i++] = no_os_field_get(LTP8800_COMMAND_LSB_MSK, cmd);

	tx_buf[i++] = value;

	if (dev->crc_en)
		tx_buf[i++] = ltp8800_pec(dev, cmd, &value, 1, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, i, 1);
}

/**
 * @brief Perform a raw PMBus read word operation
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param word - Address of the read word
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_read_word(struct ltp8800_dev *dev, uint16_t cmd, uint16_t *word)
{
	int ret;
	uint8_t rx_buf[3];
	uint8_t cmd_buf[2];

	if (!dev)
		return -EINVAL;

	if (cmd >= LTP8800_EXTENDED_COMMAND_BEGIN) {
		no_os_put_unaligned_be16(cmd, cmd_buf);
		ret = no_os_i2c_write(dev->i2c_desc, cmd_buf, 2, 0);
	} else {
		cmd_buf[0] = no_os_field_get(LTP8800_COMMAND_LSB_MSK, cmd);
		ret = no_os_i2c_write(dev->i2c_desc, cmd_buf, 1, 0);
	}
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 3, 1);
		if (ret)
			return ret;

		if (ltp8800_pec(dev, cmd, rx_buf, 2, 1) != rx_buf[2])
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
 * @param cmd - PMBus command
 * @param word - Word to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_write_word(struct ltp8800_dev *dev, uint16_t cmd, uint16_t word)
{
	uint8_t tx_buf[5];
	uint8_t i = 0;

	if (!dev)
		return -EINVAL;

	if (cmd >= LTP8800_EXTENDED_COMMAND_BEGIN)
		tx_buf[i++] = LTP8800_EXTENDED_COMMAND_PREFIX;

	tx_buf[i++] = no_os_field_get(LTP8800_COMMAND_LSB_MSK, cmd);

	no_os_put_unaligned_le16(word, &tx_buf[i]);
	i += 2;

	if (dev->crc_en)
		tx_buf[i++] = ltp8800_pec(dev, cmd, &tx_buf[1], 2, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, i, 1);
}

/**
 * @brief Perform a PMBus read word operation and converts to actual value
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of data read
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_read_word_data(struct ltp8800_dev *dev, uint16_t cmd, int *data)
{
	int ret;
	uint16_t reg;

	ret = ltp8800_read_word(dev, cmd, &reg);
	if (ret)
		return ret;

	return ltp8800_reg2data(dev, cmd, reg, data);
}

/**
 * @brief Converts value to register data and do PMBus write word operation
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_write_word_data(struct ltp8800_dev *dev, uint16_t cmd, int data)
{
	int ret;
	uint16_t reg;

	ret = ltp8800_data2reg(dev, cmd, data, &reg);
	if (ret)
		return ret;

	return ltp8800_write_word(dev, cmd, reg);
}

/**
 * @brief Perform a PMBus read block operation
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of the read block
 * @param nbytes - Size of the block in bytes
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_read_block_data(struct ltp8800_dev *dev, uint16_t cmd,
			    uint8_t *data, size_t nbytes)
{
	int ret;
	uint8_t rxbuf[nbytes + 2];
	uint8_t cmd_buf[2];

	if (!dev)
		return -EINVAL;

	if (cmd >= LTP8800_EXTENDED_COMMAND_BEGIN) {
		no_os_put_unaligned_be16(cmd, cmd_buf);
		ret = no_os_i2c_write(dev->i2c_desc, cmd_buf, 2, 0);
	} else {
		cmd_buf[0] = no_os_field_get(LTP8800_COMMAND_LSB_MSK, cmd);
		ret = no_os_i2c_write(dev->i2c_desc, cmd_buf, 1, 0);
	}
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 2, 1);
		if (ret)
			return ret;

		if ((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;

		if (ltp8800_pec(dev, cmd, rxbuf, nbytes + 1, 1) !=
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
 * @param value_type - Value type.
 * 		       Example values: LTP8800_VIN
 * 				       LTP8800_VOUT
 * 				       LTP8800_IIN
 * 				       LTP8800_IOUT
 * 				       LTP8800_TEMP
 * 				       LTP8800_FREQUENCY
 * @param value - Address of the read value
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_read_value(struct ltp8800_dev *dev,
		       enum ltp8800_value_type value_type,
		       int *value)
{
	return ltp8800_read_word_data(dev, (uint8_t)value_type, value);
}

/**
 * @brief Read statuses
 *
 * @param dev - Device structure
 * @param status_type - Status type.
 * 			Example values: LTP8800_STATUS_BYTE_TYPE
 * 					LTP8800_STATUS_VOUT_TYPE
 * 					LTP8800_STATUS_IOUT_TYPE
 * 					LTP8800_STATUS_INPUT_TYPE
 * 					LTP8800_STATUS_CML_TYPE
 * @param status - Address of the status structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_read_status(struct ltp8800_dev *dev,
			enum ltp8800_status_type status_type,
			struct ltp8800_status *status)
{
	int ret;

	if (status_type & LTP8800_STATUS_WORD_TYPE) {
		ret = ltp8800_read_word(dev, LTP8800_STATUS_WORD,
					&status->word);
		if (ret)
			return ret;
	}

	if (status_type & LTP8800_STATUS_BYTE_TYPE) {
		ret = ltp8800_read_byte(dev, LTP8800_STATUS_BYTE,
					&status->byte);
		if (ret)
			return ret;
	}

	if (status_type & LTP8800_STATUS_VOUT_TYPE) {
		ret = ltp8800_read_byte(dev, LTP8800_STATUS_VOUT,
					&status->vout);
		if (ret)
			return ret;
	}

	if (status_type & LTP8800_STATUS_IOUT_TYPE) {
		ret = ltp8800_read_byte(dev, LTP8800_STATUS_IOUT,
					&status->iout);
		if (ret)
			return ret;
	}

	if (status_type & LTP8800_STATUS_INPUT_TYPE) {
		ret = ltp8800_read_byte(dev, LTP8800_STATUS_INPUT,
					&status->input);
		if (ret)
			return ret;
	}

	if (status_type & LTP8800_STATUS_TEMP_TYPE) {
		ret = ltp8800_read_byte(dev, LTP8800_STATUS_TEMPERATURE,
					&status->temp);
		if (ret)
			return ret;
	}

	if (status_type & LTP8800_STATUS_CML_TYPE) {
		ret = ltp8800_read_byte(dev, LTP8800_STATUS_CML,
					&status->cml);
		if (ret)
			return ret;
	}

	if (status_type & LTP8800_STATUS_MFR_SPECIFIC_TYPE) {
		ret = ltp8800_read_byte(dev, LTP8800_STATUS_MFR_SPECIFIC,
					&status->mfr_specific);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set output voltage command
 *
 * @param dev - Device structure
 * @param vout_command - Output voltage in millivolts
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_vout_value(struct ltp8800_dev *dev, int vout_command)
{
	if (vout_command >= LTP8800_VOUT_COMMAND_MIN &&
	    vout_command <= LTP8800_VOUT_COMMAND_MAX)
		return ltp8800_write_word_data(dev, LTP8800_VOUT_COMMAND,
					       vout_command);
	else
		return -EINVAL;
}

/**
 * @brief Set output voltage and its upper limit
 *
 * @param dev - Device structure
 * @param settings - Gain settings
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_vout_settings(struct ltp8800_dev *dev,
			  enum ltp8800_vout_settings settings)
{
	int ret;

	ret = ltp8800_write_word(dev, LTP8800_VOUT_COMMAND,
				 LTP8800_VOUT_COMMAND_DEFAULT);
	if (ret)
		return ret;

	ret = ltp8800_write_word(dev, LTP8800_VOUT_SCALE_LOOP,
				 (uint16_t)settings);
	if (ret)
		return ret;

	return ltp8800_write_word(dev, LTP8800_VOUT_SCALE_MONITOR,
				  (uint16_t)settings);
}

/**
 * @brief Set input voltage window at which power conversion will proceed
 *
 * @param dev - Device structure
 * @param vin_on - Input voltage in millivolts at which conversion will start
 * @param vin_off - Input voltage in millivolts at which conversion will stop
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_set_vin(struct ltp8800_dev *dev, int vin_on, int vin_off)
{
	int ret;

	if (vin_on < vin_off)
		return -EINVAL;

	ret = ltp8800_write_word_data(dev, LTP8800_VIN_ON, vin_on);
	if (ret)
		return ret;

	return ltp8800_write_word_data(dev, LTP8800_VIN_OFF, vin_off);
}

/**
 * @brief Set overvalue and undervalue limits
 *
 * @param dev - Device structure
 * @param limit - Limit value type.
 * 		  Example: LTP8800_VIN_OV_FAULT_LIMIT_TYPE
 * 			   LTP8800_VIN_UV_FAULT_LIMIT_TYPE
 * 			   LTP8800_IIN_OC_FAULT_LIMIT_TYPE
 * 			   LTP8800_POUT_OP_FAULT_LIMIT_TYPE
 * @param limit_val - Limit value in milli-units for voltage and current, and
 * 		      microseconds for timing.
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_set_fault_limit(struct ltp8800_dev *dev,
			    enum ltp8800_limit_type limit,
			    int limit_val)
{
	return ltp8800_write_word_data(dev, (uint16_t)limit, limit_val);
}

/**
 * @brief Enable or disable sync pin
 *
 * @param dev - Device structure
 * @param enable - Set to true to enable, or false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_sync_config(struct ltp8800_dev *dev, bool enable)
{
	int ret;
	uint8_t val;

	ret = ltp8800_read_byte(dev, LTP8800_SYNC, &val);
	if (ret)
		return ret;

	val &= ~(LTP8800_SYNC_ENABLE_BIT);
	val |= no_os_field_prep(LTP8800_SYNC_ENABLE_BIT, !enable);

	ret = ltp8800_write_byte(dev, LTP8800_SYNC, val);
	if (ret)
		return ret;

	return ltp8800_write_byte(dev, LTP8800_GO_CMD, LTP8800_SYNC_LATCH_BIT);
}

/**
 * @brief Set phase order for polyphase application
 *
 * @param dev - Device structure
 * @param order - Order number. The resulting phase offset of the device is
 * 		  equivalent to this value multiplied by 22.5 degrees.
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_interleave_order(struct ltp8800_dev *dev, uint8_t order)
{
	int ret;
	uint16_t val;

	if (order > LTP8800_MAX_INTERLEAVE_ORDER)
		return -EINVAL;

	ret = ltp8800_read_word(dev, LTP8800_INTERLEAVE, &val);
	if (ret)
		return ret;

	val &= ~(LTP8800_INTERLEAVE_ORDER_MSK);
	val |= no_os_field_prep(LTP8800_INTERLEAVE_ORDER_MSK, order);

	ret = ltp8800_write_word(dev, LTP8800_INTERLEAVE, val);
	if (ret)
		return ret;

	dev->polyphase_order = order;

	return 0;
}

/**
 * @brief Program loop compensation for regulator transient response
 *
 * @param dev - Device structure
 * @param pole - Pole setting
 * @param zero - Zero setting
 * @param hf_gain - High frequency gain setting
 * @param lf_gain - Low frequence gain setting
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_loop_compensation(struct ltp8800_dev *dev,
			      uint8_t pole,
			      uint8_t zero,
			      uint8_t hf_gain,
			      uint8_t lf_gain)
{
	int ret;

	ret = ltp8800_write_byte(dev, LTP8800_NM_DIGFILT_POLE, pole);
	if (ret)
		return ret;

	ret = ltp8800_write_byte(dev, LTP8800_NM_DIGFILT_ZERO, zero);
	if (ret)
		return ret;

	ret = ltp8800_write_byte(dev, LTP8800_NM_DIGFILT_HF_GAIN, hf_gain);
	if (ret)
		return ret;

	return ltp8800_write_byte(dev, LTP8800_NM_DIGFILT_LF_GAIN, lf_gain);
}

/**
 * @brief Set device state
 *
 * @param dev - Device structure
 * @param state - Set to true to enable device, false to disable
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_set_device_state(struct ltp8800_dev *dev, bool state)
{
	return no_os_gpio_direction_output(dev->ctrl_desc, (uint8_t)state);
}

/**
 * @brief Store user settings to EEPROM.
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_store_user_settings(struct ltp8800_dev *dev)
{
	int ret, i;

	/* Unlock EEPROM */
	for (i = 0; i < 2; i++) {
		ret = ltp8800_write_byte(dev, LTP8800_EEPROM_PASSWORD,
					 LTP8800_EEPROM_PASSWORD_VALUE);
		if (ret)
			return ret;
	}

	ret = ltp8800_send_byte(dev, LTP8800_STORE_USER_ALL);
	if (ret)
		return ret;

	return ltp8800_write_byte(dev, LTP8800_EEPROM_PASSWORD,
				  LTP8800_EEPROM_LOCK_VALUE);
}

/**
 * @brief Restore user settings.
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int ltp8800_restore_user_settings(struct ltp8800_dev *dev)
{
	return ltp8800_send_byte(dev, LTP8800_RESTORE_USER_ALL);
}
