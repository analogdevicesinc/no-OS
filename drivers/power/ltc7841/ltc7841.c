/***************************************************************************/ /**
																			   *   @file   ltc7841.c
																			   *   @brief  Source file of LTC7841 Driver.
																			   *   @author Marvin Cabuenas (marvvinneil.cabuenas@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ltc7841.h"

static uint8_t g_i2c0_read_buffer[I2C_DEFAULT_ARRAY_SIZE];
static int I2C0_FLAG = 0;

// I2C callback function, this function is called after every write/ write/read transaction for I2C port 0
static void I2C0_Callback(mxc_i2c_req_t *req, int error)
{
	I2C0_FLAG = error;
	return;
}

const uint8_t ltc7841_write_only_registers[WRITE_ONLY_REGISTERS_NUMBER] =
	{
		LTC7841_MFR_CLEAR_PEAKS,
		LTC7841_MFR_RESET};

/** @brief read data from slave
	@param slave_type, there are multiple slaves for the BBU, such as BMS MCU, EE Prom, LTC2971, etc
		   read_length, number of bytes to receive
		   register_address, address of the register to read from
	@return error_result, 0 for no error
**/
static int read_data_from_slave(uint8_t read_length, uint8_t register_address)
{
	int error_result;
	mxc_i2c_req_t request;
	request.addr = LTC7841_ADDR;
	request.restart = STOP_MODE;
	request.tx_len = I2C_WRITE_LENGTH_FOR_READ;
	request.rx_len = read_length;
	request.tx_buf = &register_address;
	request.i2c = MXC_I2C0;
	request.rx_buf = g_i2c0_read_buffer;
	request.callback = I2C0_Callback;
	request.callback = I2C0_Callback;

	error_result = MXC_I2C_MasterTransaction(&request);
}

/** @brief write data to slave
	@param write_array, array of bytes to send to the slave
		   data_length, number of bytes to send
	@return error_result, 0 for no error
**/
int write_data_to_slave(uint8_t *write_array, uint8_t data_length)
{
	int error_result;
	mxc_i2c_req_t request;
	request.addr = LTC7841_ADDR;
	request.rx_len = 0;
	request.tx_buf = write_array;
	request.tx_len = data_length;
	request.restart = STOP_MODE;
	I2C0_FLAG = 0;
	request.i2c = MXC_I2C0;
	request.callback = I2C0_Callback;

	error_result = MXC_I2C_MasterTransaction(&request);
}
/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int ltc7841_remove(struct ltc7841_desc *desc)
{
	int ret;
	if (!desc)
	{
		return -EINVAL;
	}

	ret = no_os_i2c_remove(desc->comm_desc);
	if (ret)
	{
		return ret;
	}

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
	switch (addr)
	{
	case LTC7841_MFR_CLEAR_PEAKS:
	{
		byte_size = 0;
		break;
	}
	case LTC7841_OPERATION:
	case LTC7841_VOUT_MODE:
	case LTC7841_PMBUS_REVISION:
	case LTC7841_MFR_CONFIG:
	case LTC7841_MFR_RAIL_ADDRESS:
	case LTC7841_MFR_RESET:
	{
		byte_size = 1;
		break;
	}
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
	{
		byte_size = 2;
		break;
	}
	default:
	{
		byte_size = 0;
		break;
	}
	}
	return byte_size;
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc7841_init(struct ltc7841_desc **device, struct ltc7841_init_param *init_param)
{
	struct ltc7841_desc *dev;
	int ret;
	/* ltc7841 only needs sda, scl and ground connection to the microcontroller*/
	dev = (struct ltc7841_desc *)calloc(1, sizeof(struct ltc7841_desc));
	if (!dev)
	{
		return -ENOMEM;
	}
	/* Initialize I2C */
	ret = no_os_i2c_init(&dev->comm_desc, init_param->comm_param);
	if (ret)
	{
		goto i2c_err;
	}
	// ltc7841_registers_init(dev);
	/*initialize mfr vout command to half upon initializing*/
	ret = ltc7841_change_vout_command(dev, MFR_VOUT_INIT);
	if (ret)
	{
		goto i2c_err;
	}
	ret = ltc7841_turn_on(dev);
	if (ret)
	{
		goto i2c_err;
	}
	*device = dev;
	return 0;
i2c_err:
	// no_os_free(dev);
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
	uint8_t write_length = 1;
	uint8_t size = get_register_size(cmd);
	uint8_t write_buffer[write_length];

	/* I2C write target address */
	read_length = size;
	write_buffer[0] = cmd;
	// write part of write_read, 0 at the end for no stop bit
	ret = no_os_i2c_write(desc->comm_desc, write_buffer, write_length, 0);
	if (ret)
	{
		return ret;
	}
	// read part of write_read, 1 at the end for stop bit sending
	ret = no_os_i2c_read(desc->comm_desc, g_i2c0_read_buffer, read_length, 1);
	if (ret)
	{
		return ret;
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
	if (WORD_LENGTH == size)
	{
		i2c_data[1] = value & 0x00FF;
		i2c_data[2] = (value & 0xFF00) >> 8;
	}
	else if (BYTE_LENGTH == size)
	{
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
	int ret;
	/* Write register */
	ret = ltc7841_reg_write(desc, LTC7841_MFR_RESET, 0);
	return ret;
}
/**
 * @brief Perform change of rail address of LTC7841
 * @param desc - The device structure.
 * @param addr - 7 bit address
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_rail_address(struct ltc7841_desc *desc, uint8_t addr)
{
	// apply a 7 bit mask since bit 7 needs to be held low to enable communication
	addr &= LTC7841_7_BIT_ADDRESS_MASK;
	int ret;
	/* Write register */
	ret = ltc7841_reg_write(desc, LTC7841_MFR_RAIL_ADDRESS, addr);
	if (ret)
	{
		return ret;
	}
}
/**
 * @brief Turn off LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_turn_off(struct ltc7841_desc *desc)
{
	int ret;
	/* Write register */
	ret = ltc7841_reg_write(desc, LTC7841_OPERATION, LTC7841_OPERATION_OFF);
	if (ret)
	{
		return ret;
	}
}
/**
 * @brief Turn on LTC7841
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_turn_on(struct ltc7841_desc *desc)
{
	int ret;
	/* Write register */
	ret = ltc7841_reg_write(desc, LTC7841_OPERATION, LTC7841_OPERATION_TURN_ON);
	if (ret)
	{
		return ret;
	}
}

/**
 * @brief Margin Low for LTC7841 Operation
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_operation_margin_low(struct ltc7841_desc *desc)
{
	int ret;
	/* Write register */
	ret = ltc7841_reg_write(desc, LTC7841_OPERATION, LTC7841_OPERATION_MARGIN_LOW);
	if (ret)
	{
		return ret;
	}
}

/**
 * @brief Margin High for LTC7841 Operation
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_operation_margin_high(struct ltc7841_desc *desc)
{
	int ret;
	/* Write register */
	ret = ltc7841_reg_write(desc, LTC7841_OPERATION, LTC7841_OPERATION_MARGIN_HIGH);
	if (ret)
	{
		return ret;
	}
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

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, g_i2c0_read_buffer);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	config = g_i2c0_read_buffer[0];
	config &= LTC7841_NOMINAL_SLEW_RATE;

	ret = ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
	if (ret)
	{
		return ret;
	}
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

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, g_i2c0_read_buffer);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	config = g_i2c0_read_buffer[0];
	config |= LTC7841_SLOW_SLEW_RATE;

	ret = ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
	if (ret)
	{
		return ret;
	}
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

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, g_i2c0_read_buffer);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	config = g_i2c0_read_buffer[0];
	config |= LTC7841_FAST_SLEW_RATE;

	ret = ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
	if (ret)
	{
		return ret;
	}
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

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, g_i2c0_read_buffer);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	config = g_i2c0_read_buffer[0];
	config &= ((LTC7841_INTERNAL_DIE_TEMPERATURE) << READ_TEMPERATURE_1_BIT);
	ret = ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
	if (ret)
	{
		return ret;
	}
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

	ret = ltc7841_reg_read(desc, LTC7841_MFR_CONFIG, g_i2c0_read_buffer);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	config = g_i2c0_read_buffer[0];
	config |= ((LTC7841_TSNS_PIN_VOLTAGE) << READ_TEMPERATURE_1_BIT);
	ret = ltc7841_reg_write(desc, LTC7841_MFR_CONFIG, config);
	if (ret)
	{
		return ret;
	}
}
/**
 * @brief Change Vout command
 * @param desc - The device structure.
 * @param vout_level - value is from 0 to 0x1FF
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_vout_command(struct ltc7841_desc *desc, uint16_t vout_level)
{
	int ret;

	if (vout_level <= MFR_MAX_VALUE)
	{
		// okay to proceed
		ret = ltc7841_reg_write(desc, LTC7841_MFR_VOUT_COMMAND, vout_level);
		if (ret)
		{
			return ret;
		}
		else
		{
			ret = -1;
		}
		return ret;
	}
}

/**
 * @brief Change margin low command
 * @param desc - The device structure.
 * @param vout_level - value is from 0 to 0x1FF
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_margin_low_command(struct ltc7841_desc *desc, uint16_t vout_level)
{
	int ret;

	if (vout_level <= MFR_MAX_VALUE)
	{
		// okay to proceed
		ret = ltc7841_reg_write(desc, LTC7841_MFR_VOUT_MARGIN_LOW, vout_level);
		if (ret)
		{
			return ret;
		}
	}
	else
	{
		ret = -1;
	}
	return ret;
}
/**
 * @brief Change margin high command
 * @param desc - The device structure.
 * @param vout_level - value is from 0 to 0x1FF
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_change_margin_high_command(struct ltc7841_desc *desc, uint16_t vout_level)
{
	int ret;

	if (vout_level <= MFR_MAX_VALUE)
	{
		// okay to proceed
		ret = ltc7841_reg_write(desc, LTC7841_MFR_VOUT_MARGIN_HIGH, vout_level);
		if (ret)
		{
			return ret;
		}
	}
	else
	{
		ret = -1;
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
	int ret;

	ret = ltc7841_reg_write(desc, LTC7841_MFR_CLEAR_PEAKS, 0);
	return ret;
}
/**
 * @brief This clears the STATUS_WORD BITS that are clearable
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_clear_status_word_bits(struct ltc7841_desc *desc)
{
	int ret;
	uint16_t clear_bits;

	clear_bits = (1 << LTC7841_COMMUNICATION_FAILURE) |
				 (1 << LTC7841_TEMPERATURE_FAULT) |
				 (1 << LTC7841_OUTPUT_OVERVOLTAGE_FAULT) |
				 (1 << LTC7841_OUTPUT_VOLTAGE_FAULT);

	ret = ltc7841_reg_write(desc, LTC7841_STATUS_WORD, clear_bits);
	return ret;
}