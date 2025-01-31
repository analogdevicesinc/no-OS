/***************************************************************************//**
 *   @file   ltc7841.c
 *   @brief  Source file for the LTC7841 Driver
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
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
#include "ltc7841.h"

const uint8_t ltc7841_write_only_registers[WRITE_ONLY_REGISTERS_NUMBER] = {
	LTC7841_MFR_CLEAR_PEAKS,
	LTC7841_MFR_RESET
};

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int ltc7841_remove(struct ltc7841_desc *desc)
{
	int ret;
	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_remove(desc->comm_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief Returns the size of the register, whether one byte or two bytes
 * @param addr - Register address
 * @return size - 1 or 2 for legitimate register address, 0 otherwise
 */
static uint8_t get_register_size(uint8_t addr)
{
	uint8_t byte_size;
	switch (addr) {
	case LTC7841_MFR_CLEAR_PEAKS:
		byte_size = 0;
		break;
	case LTC7841_OPERATION:
	case LTC7841_VOUT_MODE:
	case LTC7841_PMBUS_REVISION:
	case LTC7841_MFR_CONFIG:
	case LTC7841_MFR_RAIL_ADDRESS:
	case LTC7841_MFR_RESET:
		byte_size = 1;
		break;
	case LTC7841_STATUS_WORD:
	case LTC7841_READ_VIN:
	case LTC7841_READ_VOUT:
	case LTC7841_READ_IOUT:
	case LTC7841_READ_TEMPERATURE_1:
	case LTC7841_MFR_IOUT_PEAK:
	case LTC7841_MFR_VOUT_PEAK:
	case LTC7841_MFR_VIN_PEAK:
	case LTC7841_MFR_TEMEPRATURE1_PEAK:
	case LTC7841_MFR_IIN_PEAK:
	case LTC7841_MFR_VOUT_MARGIN_HIGH:
	case LTC7841_MFR_SPECIAL_ID:
	case LTC7841_MFR_VOUT_COMMAND:
	case LTC7841_MFR_VOUT_MARGIN_LOW:
		byte_size = 2;
		break;
	default:
		byte_size = 0;
		break;
	}

	return byte_size;
}

/**
 * @brief Initialize the device structure.
 * @param device - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc7841_init(struct ltc7841_desc **device,
		 struct ltc7841_init_param *init_param)
{
	struct ltc7841_desc *dev;
	int ret;
	dev = (struct ltc7841_desc *)calloc(1, sizeof(struct ltc7841_desc));
	if (!dev)
		return -ENOMEM;
	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->comm_desc, init_param->comm_param);
	if (ret)
		goto i2c_err;
	/* Initialize mfr vout command to half upon initializing*/
	ret = ltc7841_change_vout_command(dev, MFR_VOUT_INIT);
	if (ret)
		goto i2c_err;
	ret = ltc7841_turn_on(dev);
	if (ret)
		goto i2c_err;
	*device = dev;

	return 0;
i2c_err:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Read a raw value from a register.
 * @param desc - The device structure.
 * @param cmd - PMBus command
 * @param data - Address of the byte read
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_reg_read(struct ltc7841_desc *desc, uint8_t cmd, uint8_t *data)
{
	int ret;
	uint8_t read_length;
	uint8_t size = get_register_size(cmd);
	uint8_t write_buffer;

	if (0 != size) {
		/* I2C write target address */
		read_length = size;
		write_buffer = cmd;
		ret = no_os_i2c_write(desc->comm_desc, &write_buffer, 1, 0);
		if (ret)
			return ret;
		ret = no_os_i2c_read(desc->comm_desc, data, read_length, 1);
		if (ret)
			return ret;
	} else {
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Write a raw value to a register.
 * @param desc - The device structure.
 * @param cmd - PMBus command
 * @param value - Byte to be written
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_reg_write(struct ltc7841_desc *desc, uint8_t cmd, uint16_t value)
{
	uint8_t i2c_data[LTC7841_I2C_WR_FRAME_SIZE] = {0};
	uint8_t bytes_number;
	uint8_t size = get_register_size(cmd);

	// +1 for register address
	bytes_number = size + 1;
	i2c_data[0] = cmd;
	if (LTC7841_WORD_LENGTH == size) {
		i2c_data[1] = value & 0x00FF;
		i2c_data[2] = (value & 0xFF00) >> 8;
	} else if (LTC7841_BYTE_LENGTH == size) {
		i2c_data[1] = (uint8_t)value;
	}

	return no_os_i2c_write(desc->comm_desc, i2c_data, bytes_number, 1);
}

/**
 * @brief Perform a reset to the LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_reset(struct ltc7841_desc *desc)
{
	return ltc7841_reg_write(desc, LTC7841_MFR_RESET, 0);
}

/**
 * @brief Perform change of rail address of LTC7841
 * @param desc - The device structure.
 * @param addr - 7 bit address
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_rail_address(struct ltc7841_desc *desc, uint8_t addr)
{
	addr &= LTC7841_7_BIT_ADDRESS_MASK;

	return ltc7841_reg_write(desc, LTC7841_MFR_RAIL_ADDRESS, addr);
}

/**
 * @brief Turn off LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_turn_off(struct ltc7841_desc *desc)
{
	return ltc7841_reg_write(desc, LTC7841_OPERATION, LTC7841_OPERATION_OFF);
}

/**
 * @brief Turn on LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_turn_on(struct ltc7841_desc *desc)
{
	return ltc7841_reg_write(desc, LTC7841_OPERATION, LTC7841_OPERATION_TURN_ON);
}

/**
 * @brief Margin Low for LTC7841 Operation
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_operation_margin_low(struct ltc7841_desc *desc)
{

	return ltc7841_reg_write(desc, LTC7841_OPERATION, LTC7841_OPERATION_MARGIN_LOW);
}

/**
 * @brief Margin High for LTC7841 Operation
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_operation_margin_high(struct ltc7841_desc *desc)
{

	return ltc7841_reg_write(desc, LTC7841_OPERATION,
				 LTC7841_OPERATION_MARGIN_HIGH);
}

/**
 * @brief Change to Nominal Slew Rate for LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_to_nominal_slew_rate(struct ltc7841_desc *desc)
{
	int ret;
	uint8_t config;
	uint8_t register_data[LTC7841_BYTE_LENGTH];

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, register_data);
	if (ret)
		return ret;
	/* get latest data for MFR_CONFIG */
	config = register_data[0];
	config &= LTC7841_NOMINAL_SLEW_RATE;

	return ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
}

/**
 * @brief Change to Slow Slew Rate for LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_to_slow_slew_rate(struct ltc7841_desc *desc)
{
	int ret;
	uint8_t config;
	uint8_t register_data[LTC7841_BYTE_LENGTH];

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, register_data);
	if (ret)
		return ret;
	/* Get latest data for MFR_CONFIG */
	config = register_data[0];
	config |= LTC7841_SLOW_SLEW_RATE;

	return ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
}

/**
 * @brief Change to Fast Slew Rate for LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_to_fast_slew_rate(struct ltc7841_desc *desc)
{
	int ret;
	uint8_t config;
	uint8_t register_data[LTC7841_BYTE_LENGTH];

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, register_data);
	if (ret)
		return ret;
	/* Get latest data for MFR_CONFIG */
	config = register_data[0];
	config |= LTC7841_FAST_SLEW_RATE;

	return ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
}

/**
 * @brief Change to Internal Die Temeperature Reading for LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_to_internal_die_temperature(struct ltc7841_desc *desc)
{
	int ret;
	uint8_t config;
	uint8_t register_data[LTC7841_BYTE_LENGTH];

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, register_data);
	if (ret)
		return ret;
	/* Get latest data for MFR_CONFIG */
	config = register_data[0];
	config &= ((LTC7841_INTERNAL_DIE_TEMPERATURE) << READ_TEMPERATURE_1_BIT);

	return ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
}

/**
 * @brief Change to TSNS PIN VOLTAGE for LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_to_tsns_pin_voltage(struct ltc7841_desc *desc)
{
	int ret;
	uint8_t config;
	uint8_t register_data[LTC7841_BYTE_LENGTH];

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, register_data);
	if (ret)
		return ret;
	/* Get latest data for MFR_CONFIG */
	config = register_data[0];
	config |= ((LTC7841_TSNS_PIN_VOLTAGE) << READ_TEMPERATURE_1_BIT);

	return ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
}

/**
 * @brief Change Vout command
 * @param desc - The device structure.
 * @param vout_level - value is from 0 to 0x1FF
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_vout_command(struct ltc7841_desc *desc, uint16_t vout_level)
{
	int ret = -EINVAL;

	if (vout_level <= MFR_MAX_VALUE) {
		/* Okay to proceed */
		ret = ltc7841_reg_write(desc, LTC7841_MFR_VOUT_COMMAND, vout_level);
	}

	return ret;
}

/**
 * @brief Change margin low command
 * @param desc - The device structure.
 * @param vout_level - value is from 0 to 0x1FF
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_margin_low_command(struct ltc7841_desc *desc,
				      uint16_t vout_level)
{
	int ret = -EINVAL;

	if (vout_level <= MFR_MAX_VALUE) {
		/* Okay to proceed */
		ret = ltc7841_reg_write(desc, LTC7841_MFR_VOUT_MARGIN_LOW, vout_level);
	}

	return ret;
}

/**
 * @brief Change margin high command
 * @param desc - The device structure.
 * @param vout_level - value is from 0 to 0x1FF
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_margin_high_command(struct ltc7841_desc *desc,
				       uint16_t vout_level)
{
	int ret = -EINVAL;

	if (vout_level <= MFR_MAX_VALUE) {
		/* Okay to proceed */
		ret = ltc7841_reg_write(desc, LTC7841_MFR_VOUT_MARGIN_HIGH, vout_level);
	}

	return ret;
}

/**
 * @brief This clears the MFR peak data values
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_mfr_clear_peaks(struct ltc7841_desc *desc)
{
	return ltc7841_reg_write(desc, LTC7841_MFR_CLEAR_PEAKS, 0);
}

/**
 * @brief This clears the STATUS_WORD BITS that are clearable
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_clear_status_word_bits(struct ltc7841_desc *desc)
{
	uint16_t clear_bits;

	clear_bits = (1 << LTC7841_COMMUNICATION_FAILURE) |
		     (1 << LTC7841_TEMPERATURE_FAULT) |
		     (1 << LTC7841_OUTPUT_OVERVOLTAGE_FAULT) |
		     (1 << LTC7841_OUTPUT_VOLTAGE_FAULT);

	return ltc7841_reg_write(desc, LTC7841_STATUS_WORD, clear_bits);
}
