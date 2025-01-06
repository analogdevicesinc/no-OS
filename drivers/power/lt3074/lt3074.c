/*******************************************************************************
*   @file   lt3074.c
*   @brief  Source code of the LT3074 Driver
*   @authors Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
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

#include "lt3074.h"

/* LINEAR data format params */
#define LT3074_LIN11_MANTISSA_MAX	1023000L
#define LT3074_LIN11_MANTISSA_MIN	511000L
#define LT3074_LIN11_EXPONENT_MAX	15
#define LT3074_LIN11_EXPONENT_MIN	-15
#define LT3074_LIN11_MANTISSA_MSK	NO_OS_GENMASK(10,0)
#define LT3074_LIN11_EXPONENT_MSK	NO_OS_GENMASK(15,11)
#define LT3074_LIN11_EXPONENT(x)	((int16_t)(x) >> 11)
#define LT3074_LIN11_MANTISSA(x)	(((int16_t)((x & 0x7FF) << 5)) >> 5)
#define LT3074_LIN16_EXPONENT		-13

NO_OS_DECLARE_CRC8_TABLE(lt3074_crc_table);

/**
 * @brief Converts data to LINEAR16 register value.
 *
 * @param dev The LT3074 device structure.
 * @param data The data value to be converted.
 * @param reg Pointer to store the converted register value.
 * @return Returns 0 on success, or a negative error code on failure.
 */
static int lt3074_data2reg_linear16(struct lt3074_dev *dev, int data,
				    uint16_t *reg)
{
	if (data <= 0)
		return -EINVAL;
	data <<= -(dev->lin16_exp);

	data = NO_OS_DIV_ROUND_CLOSEST_ULL(data, MILLI);
	*reg = (uint16_t)no_os_clamp(data, 0, 0xFFFF);

	return 0;
}

/**
 * @brief Converts data to LINEAR11 register value
 *
 * @param dev The LT3074 device structure.
 * @param data The data value to be converted.
 * @param reg Pointer to store the resulting register value.
 * @return Returns 0 on success, or a negative error code on failure.
 */
static int lt3074_data2reg_linear11(struct lt3074_dev *dev, int data,
				    uint16_t *reg)
{
	int exp = 0, mant = 0;
	uint8_t negative = 0;

	if (data < 0) {
		negative = 1;
		data = -data;
	}

	/* If value too high, continuously do m/2 until m < 1023. */
	while (data >= LT3074_LIN11_MANTISSA_MAX &&
	       exp < LT3074_LIN11_EXPONENT_MAX) {
		exp++;
		data >>= 1;
	}

	/* If value too low, increase mantissa. */
	while (data < LT3074_LIN11_MANTISSA_MIN &&
	       exp > LT3074_LIN11_EXPONENT_MIN) {
		exp--;
		data <<= 1;
	}

	mant = no_os_clamp(NO_OS_DIV_ROUND_CLOSEST_ULL(data, MILLI),
			   0, NO_OS_GENMASK(9, 0));
	if (negative)
		mant = -mant;

	*reg = no_os_field_prep(LT3074_LIN11_MANTISSA_MSK, mant) |
	       no_os_field_prep(LT3074_LIN11_EXPONENT_MSK, exp);

	return 0;
}

/**
 * @brief Converts raw LINEAR16 register value to its actual data
 *
 * @param dev   Pointer to the lt3074_dev structure.
 * @param reg   16-bit register value to be converted.
 * @param data  Pointer to an integer to store the converted linear data.
 * @return      0 on success, negative error code on failure.
 */
static int lt3074_reg2data_linear16(struct lt3074_dev *dev, uint16_t reg,
				    int *data)
{
	int exp = dev->lin16_exp;
	if (exp < 0)
		exp = -exp;

	*data = ((int)(reg) * MILLI) >> exp;

	return 0;
}

/**
 * @brief Converts raw LINEAR11 register value to its actual data
 *
 * @param dev   Pointer to the LT3074 device structure.
 * @param reg   The register value to be converted.
 * @param data  Pointer to the variable where the converted data will be stored.
 * @return      Returns 0 on success, or a negative error code on failure.
 */
static int lt3074_reg2data_linear11(struct lt3074_dev *dev, uint16_t reg,
				    int *data)
{
	int val, exp, mant;

	exp = LT3074_LIN11_EXPONENT(reg);
	mant = LT3074_LIN11_MANTISSA(reg);

	val = mant * MILLI;
	if (exp >= 0)
		*data = val << exp;
	else
		*data = val >> -exp;

	return 0;
}

/**
 * @brief Converts the given data value to a register value.
 *
 * @param dev The LT3074 device structure.
 * @param cmd The command value.
 * @param data The data value to be converted.
 * @param reg Pointer to store the converted register value.
 * @return Returns 0 on success, or a negative error code on failure.
 */
static int lt3074_data2reg(struct lt3074_dev *dev, uint32_t cmd,
			   int data, uint16_t *reg)
{
	switch (cmd) {
	case LT3074_VOUT_OV_WARN_LIMIT:
	case LT3074_VOUT_UV_WARN_LIMIT:
	case LT3074_READ_VOUT:
		return lt3074_data2reg_linear16(dev, data, reg);
	case LT3074_IOUT_OC_FAULT_LIMIT:
	case LT3074_OT_WARN_LIMIT:
	case LT3074_VIN_OV_WARN_LIMIT:
	case LT3074_VIN_UV_WARN_LIMIT:
	case LT3074_READ_VIN:
	case LT3074_READ_IOUT:
	case LT3074_READ_TEMPERATURE_1:
	case LT3074_MFR_READ_VBIAS:
	case LT3074_MFR_BIAS_OV_WARN_LIMIT:
	case LT3074_MFR_BIAS_UV_WARN_LIMIT:
	case LT3074_MFR_IOUT_MIN_WARN_LIMIT:
		return lt3074_data2reg_linear11(dev, data, reg);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Converts the given register value to its actual data value.
 *
 * @param dev The LT3074 device structure.
 * @param cmd The command value.
 * @param reg The register value to be converted.
 * @param data Pointer to store the converted data value.
 * @return Returns 0 on success, or a negative error code on failure.
 */
static int lt3074_reg2data(struct lt3074_dev *dev, uint32_t cmd,
			   uint16_t reg, int *data)
{
	switch (cmd) {
	case LT3074_VOUT_OV_WARN_LIMIT:
	case LT3074_VOUT_UV_WARN_LIMIT:
	case LT3074_READ_VOUT:
		return lt3074_reg2data_linear16(dev, reg, data);
	case LT3074_IOUT_OC_FAULT_LIMIT:
	case LT3074_OT_WARN_LIMIT:
	case LT3074_VIN_OV_WARN_LIMIT:
	case LT3074_VIN_UV_WARN_LIMIT:
	case LT3074_READ_VIN:
	case LT3074_READ_IOUT:
	case LT3074_READ_TEMPERATURE_1:
	case LT3074_MFR_READ_VBIAS:
	case LT3074_MFR_BIAS_OV_WARN_LIMIT:
	case LT3074_MFR_BIAS_UV_WARN_LIMIT:
	case LT3074_MFR_IOUT_MIN_WARN_LIMIT:
		return lt3074_reg2data_linear11(dev, reg, data);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Perform packet-error checking via CRC
 *
 * @param dev The LT3074 device structure.
 * @param cmd The command byte.
 * @param buf Pointer to the data buffer.
 * @param nbytes Number of bytes in the data buffer.
 * @param op Operation performed. 0 for write, 1 for read
 * @return The calculated PEC value.
 */
static uint8_t lt3074_pec(struct lt3074_dev *dev, uint8_t cmd, uint8_t *buf,
			  size_t nbytes, uint8_t op)
{
	uint8_t crc_buf[nbytes + op + 2];

	crc_buf[0] = (dev->i2c_desc->slave_address << 1);
	crc_buf[1] = cmd;
	if (op)
		crc_buf[2] = (dev->i2c_desc->slave_address << 1) | 1;

	memcpy(&crc_buf[2 + op], buf, nbytes);

	return no_os_crc8(lt3074_crc_table, crc_buf, nbytes + op + 2, 0);
}

/**
 * @brief Initialize the device structure
 *
 * @param device - Device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_init(struct lt3074_dev **device,
		struct lt3074_init_param *init_param)
{
	struct lt3074_dev *dev;
	int ret;
	uint32_t value;
	uint8_t block[7];

	dev = (struct lt3074_dev *)no_os_calloc(1, sizeof(struct lt3074_dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto i2c_err;

	/* Identify device */
	ret = lt3074_reg_read(dev, LT3074_MFR_SPECIAL_ID, &value);
	if (ret)
		goto dev_err;
	if (value != LT3074_SPECIAL_ID_VALUE) {
		ret = -EIO;
		goto dev_err;
	}

	ret = lt3074_read_block_data(dev, LT3074_IC_DEVICE_ID,
				     block, 6);
	if (ret || (strncmp((char *)block, "LT3074", 6))) {
		ret = -EIO;
		goto dev_err;
	}

	/* Set PEC */
	dev->crc_en = init_param->crc_en;
	if (dev->crc_en)
		no_os_crc8_populate_msb(lt3074_crc_table,
					LT3074_CRC_POLYNOMIAL);

	dev->lin16_exp = LT3074_LIN16_EXPONENT;

	/* Initialize GPIO for PGOOD */
	ret = no_os_gpio_get_optional(&dev->pg_desc,
				      init_param->pg_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_input(dev->pg_desc);
	if (ret)
		goto dev_err;

	/* Initialize GPIO for EN */
	ret = no_os_gpio_get_optional(&dev->en_desc,
				      init_param->en_param);
	if (ret)
		goto dev_err;

	ret = no_os_gpio_direction_output(dev->en_desc, NO_OS_GPIO_HIGH);
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

	/* Set operation */
	ret = lt3074_set_operation(dev, LT3074_OPERATION_ON);
	if (ret)
		goto dev_err;

	/* Clear faults */
	ret = lt3074_clear_faults(dev);
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
 *
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_remove(struct lt3074_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->pg_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->en_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->alert_desc);
	if (ret)
		return ret;

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
int lt3074_send_byte(struct lt3074_dev *dev, uint32_t cmd)
{
	uint8_t command = LT3074_CMD(cmd);
	return no_os_i2c_write(dev->i2c_desc, &command, 1, 1);
}

/**
 * @brief Perform a raw PMBus read byte operation
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of the byte read
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_read_byte(struct lt3074_dev *dev, uint32_t cmd, uint8_t *data)
{
	int ret;
	uint8_t rx_buf[2];
	uint8_t command = LT3074_CMD(cmd);

	ret = no_os_i2c_write(dev->i2c_desc, &command, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		if (lt3074_pec(dev, command, rx_buf, 1, 1) != rx_buf[1])
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
int lt3074_write_byte(struct lt3074_dev *dev, uint32_t cmd, uint8_t value)
{
	uint8_t tx_buf[3] = {0};

	tx_buf[0] = LT3074_CMD(cmd);
	tx_buf[1] = value;

	if (dev->crc_en)
		tx_buf[2] = lt3074_pec(dev, tx_buf[0], &value, 1, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 2, 1);
}

/**
 * @brief Perform a raw PMBus read word operation
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param word - Address of the read word
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_read_word(struct lt3074_dev *dev, uint32_t cmd, uint16_t *word)
{
	int ret;
	uint8_t rx_buf[3] = {0};
	uint8_t command = LT3074_CMD(cmd);

	ret = no_os_i2c_write(dev->i2c_desc, &command, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 3, 1);
		if (ret)
			return ret;

		if (lt3074_pec(dev, command, rx_buf, 2, 1) != rx_buf[2])
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
 * @param cmd - PMBus command
 * @param word - Word to write
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_write_word(struct lt3074_dev *dev, uint32_t cmd, uint16_t word)
{
	uint8_t tx_buf[4] = {0};

	tx_buf[0] = LT3074_CMD(cmd);
	no_os_put_unaligned_le16(word, &tx_buf[1]);

	if (dev->crc_en)
		tx_buf[3] = lt3074_pec(dev, tx_buf[0], &tx_buf[1], 2, 0);

	return no_os_i2c_write(dev->i2c_desc, tx_buf, dev->crc_en + 3, 1);
}

/**
 * @brief Perform a PMBus read word operation and converts to actual value
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of data read
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_read_word_data(struct lt3074_dev *dev, uint32_t cmd, int *data)
{
	int ret;
	uint16_t reg;

	ret = lt3074_read_word(dev, cmd, &reg);
	if (ret)
		return ret;

	return lt3074_reg2data(dev, cmd, reg, data);
}

/**
 * @brief Converts value to register data and do PMBus write word operation
 *
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_write_word_data(struct lt3074_dev *dev, uint32_t cmd, int data)
{
	int ret;
	uint16_t reg;

	ret = lt3074_data2reg(dev, cmd, data, &reg);
	if (ret)
		return ret;

	return lt3074_write_word(dev, cmd, reg);
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
int lt3074_read_block_data(struct lt3074_dev *dev, uint32_t cmd,
			   uint8_t *data, size_t nbytes)
{
	int ret;
	uint8_t rxbuf[nbytes + 2];
	uint8_t command = LT3074_CMD(cmd);

	ret = no_os_i2c_write(dev->i2c_desc, &command, 1, 0);
	if (ret)
		return ret;

	if (dev->crc_en) {
		ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 2, 1);
		if (ret)
			return ret;

		if ((size_t)rxbuf[0] > nbytes)
			return -EMSGSIZE;

		if (lt3074_pec(dev, command, rxbuf, nbytes + 1, 1) !=
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
 * @brief Generic register read wrapper
 *
 * @param dev - Device structure
 * @param reg - PMBus regiser
 * @param data - Address of the read data
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_reg_read(struct lt3074_dev *dev, uint32_t reg, uint32_t *data)
{
	int ret;
	int size = LT3074_ADDR_SIZE(reg);
	uint16_t word;
	uint8_t byte;
	uint8_t block[size];

	switch (size) {
	case LT3074_BYTE:
		ret = lt3074_read_byte(dev, reg, &byte);
		if (ret)
			return ret;

		*data = byte;

		return 0;
	case LT3074_WORD:
		ret = lt3074_read_word(dev, reg, &word);
		if (ret)
			return ret;

		*data = word;

		return 0;
	default:
		ret = lt3074_read_block_data(dev, reg, block, size);
		if (ret)
			return ret;

		*data = no_os_get_unaligned_be32(block);

		return 0;
	}
}

/**
 * @brief Generic register write wrapper
 *
 * @param dev - Device structure
 * @param reg - PMBus register
 * @param val - Value to write
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_reg_write(struct lt3074_dev *dev, uint32_t reg, uint32_t val)
{
	switch (LT3074_ADDR_SIZE(reg)) {
	case LT3074_BYTE:
		return lt3074_write_byte(dev, LT3074_CMD(reg), (uint8_t)val);
	case LT3074_WORD:
		return lt3074_write_word(dev, LT3074_CMD(reg), (uint16_t)val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read a value
 *
 * @param dev - Device structure
 * @param value_type - Value type.
 * 		       Example values: LT3074_VIN
 * 				       LT3074_VOUT
 * 				       LT3074_IOUT
 * 				       LT3074_TEMP
 * 				       LT3074_VBIAS
 * @param value - Address of the read value
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_read_value(struct lt3074_dev *dev,
		      enum lt3074_value_type value_type,
		      int *value)
{
	return lt3074_read_word_data(dev, value_type, value);
}

/**
 * @brief Read statuses
 *
 * @param dev - Device structure
 * @param status_type - Status type.
 * 			Example values: LT3074_STATUS_BYTE_TYPE
 * 					LT3074_STATUS_VOUT_TYPE
 * 					LT3074_STATUS_IOUT_TYPE
 * 					LT3074_STATUS_INPUT_TYPE
 * 					LT3074_STATUS_CML_TYPE
 * @param status - Address of the status structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_read_status(struct lt3074_dev *dev,
		       enum lt3074_status_type status_type,
		       struct lt3074_status *status)
{
	int ret;

	if (status_type & LT3074_STATUS_WORD_TYPE) {
		ret = lt3074_read_word(dev, LT3074_STATUS_WORD, &status->word);
		if (ret)
			return ret;
	}

	if (status_type & LT3074_STATUS_BYTE_TYPE) {
		ret = lt3074_read_byte(dev, LT3074_STATUS_BYTE, &status->byte);
		if (ret)
			return ret;
	}

	if (status_type & LT3074_STATUS_VOUT_TYPE) {
		ret = lt3074_read_byte(dev, LT3074_STATUS_VOUT, &status->vout);
		if (ret)
			return ret;
	}

	if (status_type & LT3074_STATUS_IOUT_TYPE) {
		ret = lt3074_read_byte(dev, LT3074_STATUS_IOUT, &status->iout);
		if (ret)
			return ret;
	}

	if (status_type & LT3074_STATUS_INPUT_TYPE) {
		ret = lt3074_read_byte(dev, LT3074_STATUS_INPUT, &status->input);
		if (ret)
			return ret;
	}

	if (status_type & LT3074_STATUS_TEMP_TYPE) {
		ret = lt3074_read_byte(dev,
				       LT3074_STATUS_TEMPERATURE,
				       &status->temp);
		if (ret)
			return ret;
	}

	if (status_type & LT3074_STATUS_CML_TYPE) {
		ret = lt3074_read_byte(dev, LT3074_STATUS_CML, &status->cml);
		if (ret)
			return ret;
	}

	if (status_type & LT3074_STATUS_MFR_SPECIFIC_TYPE) {
		ret = lt3074_read_byte(dev,
				       LT3074_STATUS_MFR_SPECIFIC,
				       &status->mfr_specific);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set output voltage margin
 *
 * @param dev - Device structure
 * @param margin_high - Upper margin
 * @param margin_low - Lower margin
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_vout_margin(struct lt3074_dev *dev, enum lt3074_margin margin_high,
		       enum lt3074_margin margin_low)
{
	uint32_t raw_data;

	raw_data = no_os_field_prep(LT3074_MARGIN_LOW_MSK, margin_high) |
		   no_os_field_prep(LT3074_MARGIN_HIGH_MSK, margin_low);

	return lt3074_reg_write(dev, LT3074_MFR_MARGIN, raw_data);
}

/**
 * @brief Sets the limit value for a specific fault/warning limits.
 *
 * @param dev - Device structure
 * @param limit - The type of limit to set.
 * @param limit_val - The value to set for the specified limit type.
 * @return Returns 0 on success, or a negative error code on failure.
 */
int lt3074_set_limit(struct lt3074_dev *dev, enum lt3074_limit_type limit,
		     int limit_val)
{
	return lt3074_write_word_data(dev, limit, limit_val);
}

/**
 * @brief Set operation.
 *
 * @param dev - Device structure
 * @param operation - Operation.
 * 		      Accepted values are: LT3074_OPERATION_OFF
 * 					   LT3074_OPERATION_ON
 * 					   LT3074_OPERATION_MARGIN_HIGH
 * 					   LT3074_OPERATION_MARGIN_LOW
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_set_operation(struct lt3074_dev *dev,
			 enum lt3074_operation_type operation)
{
	int ret;
	uint32_t temp;

	ret = lt3074_reg_read(dev, LT3074_OPERATION, &temp);
	if (ret)
		return ret;

	temp &= ~LT3074_OPERATION_ACCESS_MSK;
	temp |= operation;

	return lt3074_reg_write(dev, LT3074_OPERATION, temp);
}

/**
 * @brief Clear status registers.
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_clear_faults(struct lt3074_dev *dev)
{
	return lt3074_send_byte(dev, LT3074_CLEAR_FAULTS);
}

/**
 * @brief Enable/Disable the device using the EN pin.
 *
 * @param dev - Device structure
 * @param state - Enable/Disable state. 0 for disable, 1 for enable.
 *
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_set_enable_pin(struct lt3074_dev *dev, bool state)
{
	return no_os_gpio_direction_output(dev->en_desc, state);
}

/**
 * @brief Perform a device software reset
 *
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int lt3074_software_reset(struct lt3074_dev *dev)
{
	return lt3074_send_byte(dev, LT3074_MFR_RESET);
}
