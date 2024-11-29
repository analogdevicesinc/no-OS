/***************************************************************************//**
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

//I2C callback function, this function is called after every write/ write/read transaction for I2C port 0
static void I2C0_Callback(mxc_i2c_req_t *req, int error)
{
    I2C0_FLAG = error;
    return;
}

const uint8_t ltc7841_write_only_registers[WRITE_ONLY_REGISTERS_NUMBER] =
{
	LTC7841_MFR_CLEAR_PEAKS,
	LTC7841_MFR_RESET
};

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
int write_data_to_slave(uint8_t * write_array, uint8_t data_length)
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

	if (!desc)
	{
		return -EINVAL;
	}

	/*ret = no_os_i2c_remove(desc->comm_desc);
	if (ret)
	{
		return ret;
	}

	no_os_free(desc);
	*/

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
 * @brief Returns the array order of a register, given its address 
 * @param addr - Register address
 * @return register_array_order
 */
uint8_t get_ltc7841_register_order(uint8_t addr)
{
	uint8_t order;
	switch (addr)
	{
		case LTC7841_OPERATION:
		{
			order = LTC7841_OPERATION_ORDER;
			break;
		}
		case LTC7841_VOUT_MODE:
		{
			order = LTC7841_VOUT_MODE_ORDER;
			break;
		}
		case LTC7841_STATUS_WORD:
		{
			order = LTC7841_STATUS_WORD_ORDER;
			break;
		}
		case LTC7841_READ_VIN:
		{
			order = LTC7841_READ_VIN_ORDER;
			break;
		}
		case LTC7841_READ_IIN:
		{
			order = LTC7841_READ_IIN_ORDER;
			break;
		}
		case LTC7841_READ_VOUT:
		{
			order = LTC7841_READ_VOUT_ORDER;
			break;
		}
		case LTC7841_READ_IOUT:
		{
			order = LTC7841_READ_IOUT_ORDER;
			break;
		}
		case LTC7841_READ_TEMPERATURE_1:
		{
			order = LTC7841_READ_TEMPERATURE_1_ORDER;
			break;
		}
		case LTC7841_PMBUS_REVISION:
		{
			order = LTC7841_PMBUS_REVISION_ORDER;
			break;
		}
		case LTC7841_MFR_IOUT_PEAK:
		{
			order = LTC7841_MFR_IOUT_PEAK_ORDER;
			break;
		}
		case LTC7841_MFR_VOUT_PEAK:
		{
			order = LTC7841_MFR_VOUT_PEAK_ORDER;
			break;
		}
		case LTC7841_MFR_VIN_PEAK:
		{
			order = LTC7841_MFR_VIN_PEAK_ORDER;
			break;
		}
		case LTC7841_MFR_TEMEPRATURE1_PEAK:
		{
			order = LTC7841_MFR_TEMEPRATURE1_PEAK_ORDER;
			break;
		}
		case LTC7841_MFR_IIN_PEAK:
		{
			order = LTC7841_MFR_IIN_PEAK_ORDER;
			break;
		}
		case LTC7841_MFR_CLEAR_PEAKS:
		{
			order = LTC7841_MFR_CLEAR_PEAKS_ORDER;
			break;
		}
		case LTC7841_MFR_VOUT_MARGIN_HIGH:
		{
			order = LTC7841_MFR_VOUT_MARGIN_HIGH_ORDER;
			break;
		}
		case LTC7841_MFR_SPECIAL_ID:
		{
			order = LTC7841_MFR_SPECIAL_ID_ORDER;
			break;
		}
		case LTC7841_MFR_VOUT_COMMAND:
		{
			order = LTC7841_MFR_VOUT_COMMAND_ORDER;
			break;
		}
		case LTC7841_MFR_CONFIG:
		{
			order = LTC7841_MFR_CONFIG_ORDER;
			break;
		}
		case LTC7841_MFR_VOUT_MARGIN_LOW:
		{
			order = LTC7841_MFR_VOUT_MARGIN_LOW_ORDER;
			break;
		}
		case LTC7841_MFR_RAIL_ADDRESS:
		{
			order = LTC7841_MFR_RAIL_ADDRESS_ORDER;
			break;
		}
		case LTC7841_MFR_RESET:
		{
			order = LTC7841_MFR_RESET_ORDER;
			break;
		}
		default:
	    {
			order = 0xFF;
			break;
		}
	}
	return order;
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc7841_init(struct ltc7841_desc ** device, struct ltc7841_init_param * init_param)
{
	struct ltc7841_desc *dev;
	int ret;
    /* ltc7841 only needs sda, scl and ground connection to the microcontroller*/
	dev = (struct ltc7841_desc *) calloc(1, sizeof(struct ltc7841_desc));
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
    //ltc7841_registers_init(dev);
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
	//no_os_free(dev);
	return ret;
}

/**
 * @brief Read a raw value from a register.
 * @param desc - The device structure.
 * @param reg - Register structure holding info about the register to read from.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_reg_read(struct ltc7841_desc *desc, struct ltc7841_reg_st *reg)
{
    int ret;
	uint8_t read_length;
	uint8_t write_length = 1;
    uint8_t size = get_register_size(reg->addr);
	uint8_t write_buffer[write_length];

    /* I2C write target address */
	read_length = size;
	write_buffer[0] = reg->addr;
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

    /* Update register with new read value */
    if (WORD_LENGTH == size)
	{
		// shift High data byte by 8 to store data
		reg->value = g_i2c0_read_buffer[0] | (g_i2c0_read_buffer[1] << 8);
	}
	else
	{
		// byte length
		reg->value = g_i2c0_read_buffer[0];
	}
	return 0;
}

/**
 * @brief Write a raw value to a register.
 * @param desc - The device structure.
 * @param reg - Register structure holding info about the register to be written.
 * @return 0 in case of success, error code otherwise
 */
int ltc7841_reg_write(struct ltc7841_desc *desc, struct ltc7841_reg_st reg)
{
	uint8_t i2c_data[LTC7841_I2C_WR_FRAME_SIZE] = {0};
	uint8_t bytes_number;
	uint8_t size = get_register_size(reg.addr);

	// +1 for register address
    bytes_number = size + 1;
	i2c_data[0] = reg.addr;
	if (WORD_LENGTH == size)
	{
		i2c_data[1] = reg.value & 0x00FF;
		i2c_data[2] = (reg.value & 0xFF00) >> 8;
	}
	else if (BYTE_LENGTH == size)
	{
        i2c_data[1] = (uint8_t) reg.value;
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
	 struct ltc7841_reg_st reg = desc->regs[LTC7841_MFR_RESET_ORDER];
	 /* Write register */
	 ret = ltc7841_reg_write(desc, reg);
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
	struct ltc7841_reg_st reg = desc->regs[LTC7841_MFR_RAIL_ADDRESS_ORDER];
	reg.value = addr;
	/* Write register */
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_MFR_RAIL_ADDRESS_ORDER].value = reg.value;
		return 0;
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
	 struct ltc7841_reg_st reg = desc->regs[LTC7841_OPERATION_ORDER];
	 reg.value = LTC7841_OPERATION_OFF;
	 /* Write register */
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_OPERATION_ORDER].value = reg.value;
		return 0;
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
	 struct ltc7841_reg_st reg = desc->regs[LTC7841_OPERATION_ORDER];
	 reg.addr = LTC7841_OPERATION;
	 reg.value = LTC7841_OPERATION_TURN_ON;
	 /* Write register */
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_OPERATION_ORDER].value = reg.value;
		return 0;
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
	struct ltc7841_reg_st reg = desc->regs[LTC7841_OPERATION_ORDER];
	reg.value = LTC7841_OPERATION_MARGIN_LOW;
	/* Write register */
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_OPERATION_ORDER].value = reg.value;
		return 0;
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
	 struct ltc7841_reg_st reg = desc->regs[LTC7841_OPERATION_ORDER];
	 reg.value = LTC7841_OPERATION_MARGIN_HIGH;
	 /* Write register */
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_OPERATION_ORDER].value = reg.value;
		return 0;
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
	 struct ltc7841_reg_st reg;
	 
	ret = ltc7841_reg_read(desc, &desc->regs[LTC7841_MFR_CONFIG_ORDER]);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	reg = desc->regs[LTC7841_MFR_CONFIG_ORDER];
	reg.value &= LTC7841_NOMINAL_SLEW_RATE;
	 
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_MFR_CONFIG_ORDER].value = reg.value;
		return 0;
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
	struct ltc7841_reg_st reg;
	 
	ret = ltc7841_reg_read(desc, &desc->regs[LTC7841_MFR_CONFIG_ORDER]);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	reg = desc->regs[LTC7841_MFR_CONFIG_ORDER];
	reg.value |= LTC7841_SLOW_SLEW_RATE;
	 
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_MFR_CONFIG_ORDER].value = reg.value;
		return 0;
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
	struct ltc7841_reg_st reg;
	 
	ret = ltc7841_reg_read(desc, &desc->regs[LTC7841_MFR_CONFIG_ORDER]);
	if (ret)
	{
		return ret;
	}
	 // get latest data for MFR_CONFIG
	reg = desc->regs[LTC7841_MFR_CONFIG_ORDER];
	reg.value |= LTC7841_FAST_SLEW_RATE;
	 
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_MFR_CONFIG_ORDER].value = reg.value;
		return 0;
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
	struct ltc7841_reg_st reg;
	 
	ret = ltc7841_reg_read(desc, &desc->regs[LTC7841_MFR_CONFIG_ORDER]);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	reg = desc->regs[LTC7841_MFR_CONFIG_ORDER];
	reg.value &= ((LTC7841_INTERNAL_DIE_TEMPERATURE) << READ_TEMPERATURE_1_BIT);
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
		desc->regs[LTC7841_MFR_CONFIG_ORDER].value = reg.value;
		return 0;
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
	struct ltc7841_reg_st reg;
	 
	ret = ltc7841_reg_read(desc, &desc->regs[LTC7841_MFR_CONFIG_ORDER]);
	if (ret)
	{
		return ret;
	}
	// get latest data for MFR_CONFIG
	reg = desc->regs[LTC7841_MFR_CONFIG_ORDER];
	reg.value |= ((LTC7841_TSNS_PIN_VOLTAGE) << READ_TEMPERATURE_1_BIT);
	ret = ltc7841_reg_write(desc, reg);
	if (ret)
	{
		return ret;
	}
	else
	{
		/*Update register value*/
	    desc->regs[LTC7841_MFR_CONFIG_ORDER].value = reg.value;
	    return 0;
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
	struct ltc7841_reg_st reg;
	
	if (vout_level <= MFR_MAX_VALUE)
	{
		// okay to proceed
		reg.addr = LTC7841_MFR_VOUT_COMMAND;
		reg.value = vout_level;
		ret = ltc7841_reg_write(desc, reg);
		if (ret)
		{
			return ret;
		}
		else
		{
			/*Update register value*/
			desc->regs[LTC7841_MFR_VOUT_COMMAND_ORDER].value = reg.value;
			ret = 0;
		}
	}
	else
	{
		ret = -1;
	}
	return ret;
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
	struct ltc7841_reg_st reg;
	
	if (vout_level <= MFR_MAX_VALUE)
	{
		// okay to proceed
		reg.addr = LTC7841_MFR_VOUT_MARGIN_LOW;
		reg.value = vout_level;
		ret = ltc7841_reg_write(desc, reg);
		if (ret)
		{
			return ret;
		}
		else
		{
			/*Update register value*/
			desc->regs[LTC7841_MFR_VOUT_MARGIN_LOW_ORDER].value = reg.value;
			ret = 0;
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
	struct ltc7841_reg_st reg;
	
	if (vout_level <= MFR_MAX_VALUE)
	{
		// okay to proceed
		reg.addr = LTC7841_MFR_VOUT_MARGIN_HIGH;
		reg.value = vout_level;
		ret = ltc7841_reg_write(desc, reg);
		if (ret)
		{
			return ret;
		}
		else
		{
			/*Update register value*/
			desc->regs[LTC7841_MFR_VOUT_MARGIN_HIGH_ORDER].value = reg.value;
			ret = 0;
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
	 struct ltc7841_reg_st reg;

	 reg.addr = LTC7841_MFR_CLEAR_PEAKS;
	 
	 ret = ltc7841_reg_write(desc, reg);
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
	struct ltc7841_reg_st reg;

	reg.addr = LTC7841_STATUS_WORD;

    reg.value = (1 << LTC7841_COMMUNICATION_FAILURE) |
                (1 << LTC7841_TEMPERATURE_FAULT) |
                (1 << LTC7841_OUTPUT_OVERVOLTAGE_FAULT) |
				(1 << LTC7841_OUTPUT_VOLTAGE_FAULT);
	
	ret = ltc7841_reg_write(desc, reg);
	return ret;     				
 }