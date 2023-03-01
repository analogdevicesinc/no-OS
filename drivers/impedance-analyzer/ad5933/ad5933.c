/***************************************************************************//**
 *   @file   AD5933.c
 *   @brief  Implementation of AD5933 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdlib.h>
#include "ad5933.h"
#include <math.h>
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Constants Definitions *****************************/
/******************************************************************************/
const int32_t pow_2_27 = 134217728ul;      // 2 to the power of 27

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD5933 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret - The result of the initialization procedure.
 *               Example: -1 - I2C peripheral was not initialized or the
 *                             device is not present.
 *                         0 - I2C peripheral was initialized and the
 *                             device is present.
*******************************************************************************/
int32_t ad5933_init(struct ad5933_dev **device,
		    struct ad5933_init_param init_param)
{
	struct ad5933_dev *dev;
	int32_t status;

	dev = (struct ad5933_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->current_sys_clk = init_param.current_sys_clk;
	dev->current_clock_source = init_param.current_clock_source;
	dev->current_gain = init_param.current_gain;
	dev->current_range = init_param.current_range;

	status = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5933_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad5933_remove(struct ad5933_dev *dev)
{
	int32_t status;

	status = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return status;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param register_value   - Data value to write.
 * @param bytes_number     - Number of bytes.
 *
 * @return None.
*******************************************************************************/
void ad5933_set_register_value(struct ad5933_dev *dev,
			       uint8_t register_address,
			       uint32_t register_value,
			       uint8_t bytes_number)
{
	uint8_t byte = 0;
	uint8_t write_data[2] = {0, 0};

	for(byte = 0; byte < bytes_number; byte++) {
		write_data[0] = register_address + bytes_number - byte - 1;
		write_data[1] = (uint8_t)((register_value >> (byte * 8)) & 0xFF);
		no_os_i2c_write(dev->i2c_desc, write_data, 2, 1);
	}
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param bytes_number     - Number of bytes.
 *
 * @return registerValue  - Value of the register.
*******************************************************************************/
uint32_t ad5933_get_register_value(struct ad5933_dev *dev,
				   uint8_t register_address,
				   uint8_t bytes_number)
{
	uint32_t register_value = 0;
	uint8_t byte = 0;
	uint8_t write_data[2]   = {0, 0};
	uint8_t read_data[2]    = {0, 0};

	for(byte = 0; byte < bytes_number; byte ++) {
		/* Set the register pointer. */
		write_data[0] = AD5933_ADDR_POINTER;
		write_data[1] = register_address + byte;
		no_os_i2c_write(dev->i2c_desc, write_data, 2, 1);
		/* Read Register Data. */
		read_data[0] = 0xFF;
		no_os_i2c_read(dev->i2c_desc, read_data, 1, 1);
		register_value = register_value << 8;
		register_value += read_data[0];
	}

	return register_value;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev             - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad5933_reset(struct ad5933_dev *dev)
{
	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_LB,
				  AD5933_CONTROL_RESET | dev->current_clock_source,
				  1);
}

/***************************************************************************//**
 * @brief Selects the source of the system clock.
 *
 * @param dev          - The device structure.
 * @param clk_source   - Selects the source of the system clock.
 *                     Example: AD5933_CONTROL_INT_SYSCLK
 *                              AD5933_CONTROL_EXT_SYSCLK
 * @param ext_clk_freq - Frequency value of the external clock, if used.
 *
 * @return None.
*******************************************************************************/
void ad5933_set_system_clk(struct ad5933_dev *dev,
			   int8_t clk_source,
			   uint32_t ext_clk_freq)
{
	dev->current_clock_source = clk_source;
	if(clk_source == AD5933_CONTROL_EXT_SYSCLK) {
		dev->current_sys_clk = ext_clk_freq;                 // External clock frequency
	} else {
		dev->current_sys_clk = AD5933_INTERNAL_SYS_CLK;    // 16 MHz
	}
	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_LB,
				  dev->current_clock_source,
				  1);
}


/***************************************************************************//**
 * @brief Selects the range and gain of the device.
 *
 * @param dev   - The device structure.
 * @param range - Range option.
 *                Example: AD5933_RANGE_2000mVpp
 *                         AD5933_RANGE_200mVpp
 *                         AD5933_RANGE_400mVpp

 *                         AD5933_RANGE_1000mVpp
 * @param gain  - Gain option.
 *                Example: AD5933_GAIN_X5
 *                         AD5933_GAIN_X1
 *
 * @return None.
*******************************************************************************/
void ad5933_set_range_and_gain(struct ad5933_dev *dev,
			       int8_t range,
			       int8_t gain)
{
	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_HB,
				  AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_NOP) |
				  AD5933_CONTROL_RANGE(range) |
				  AD5933_CONTROL_PGA_GAIN(gain),
				  1);
	/* Store the last settings made to range and gain. */
	dev->current_range = range;
	dev->current_gain = gain;
}

/***************************************************************************//**
 * @brief Reads the temperature from the part and returns the data in
 *        degrees Celsius.
 *
 * @param dev             - The device structure.
 *
 * @return temperature - Temperature.
*******************************************************************************/
float ad5933_get_temperature(struct ad5933_dev *dev)
{
	float temperature = 0;
	uint8_t status = 0;

	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_HB,
				  AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_MEASURE_TEMP) |
				  AD5933_CONTROL_RANGE(dev->current_range) |
				  AD5933_CONTROL_PGA_GAIN(dev->current_gain),
				  1);
	while((status & AD5933_STAT_TEMP_VALID) == 0) {
		status = ad5933_get_register_value(dev,
						   AD5933_REG_STATUS,
						   1);
	}

	temperature = ad5933_get_register_value(dev,
						AD5933_REG_TEMP_DATA,
						2);
	if(temperature < 8192) {
		temperature /= 32;
	} else {
		temperature -= 16384;
		temperature /= 32;
	}

	return temperature;
}

/***************************************************************************//**
 * @brief Configures the sweep parameters: Start frequency, Frequency increment
 *        and Number of increments.
 *
 * @param dev        - The device structure.
 * @param start_freq - Start frequency in Hz;
 * @param inc_freq   - Frequency increment in Hz;
 * @param inc_num    - Number of increments. Maximum value is 511(0x1FF).
 *
 * @return None.
*******************************************************************************/
void ad5933_config_sweep(struct ad5933_dev *dev,
			 uint32_t  start_freq,
			 uint32_t  inc_freq,
			 uint16_t inc_num)
{
	uint32_t start_freq_reg = 0;
	uint32_t inc_freq_reg = 0;
	uint16_t inc_num_reg = 0;

	/* Ensure that incNum is a valid data. */
	if(inc_num > AD5933_MAX_INC_NUM) {
		inc_num_reg = AD5933_MAX_INC_NUM;
	} else {
		inc_num_reg = inc_num;
	}

	/* Convert users start frequency to binary code. */
	start_freq_reg = (uint32_t)((double)start_freq * 4 / dev->current_sys_clk *
				    pow_2_27);

	/* Convert users increment frequency to binary code. */
	inc_freq_reg = (uint32_t)((double)inc_freq * 4 / dev->current_sys_clk *
				  pow_2_27);

	/* Configure the device with the sweep parameters. */
	ad5933_set_register_value(dev,
				  AD5933_REG_FREQ_START,
				  start_freq_reg,
				  3);
	ad5933_set_register_value(dev,
				  AD5933_REG_FREQ_INC,
				  inc_freq_reg,
				  3);
	ad5933_set_register_value(dev,
				  AD5933_REG_INC_NUM,
				  inc_num_reg,
				  2);
}

/***************************************************************************//**
 * @brief Starts the sweep operation.
 *
 * @param dev             - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad5933_start_sweep(struct ad5933_dev *dev)
{
	uint8_t status = 0;

	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_HB,
				  AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_STANDBY) |
				  AD5933_CONTROL_RANGE(dev->current_range) |
				  AD5933_CONTROL_PGA_GAIN(dev->current_gain),
				  1);
	ad5933_reset(dev);
	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_HB,
				  AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_INIT_START_FREQ)|
				  AD5933_CONTROL_RANGE(dev->current_range) |
				  AD5933_CONTROL_PGA_GAIN(dev->current_gain),
				  1);
	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_HB,
				  AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_START_SWEEP) |
				  AD5933_CONTROL_RANGE(dev->current_range) |
				  AD5933_CONTROL_PGA_GAIN(dev->current_gain),
				  1);
	status = 0;
	while((status & AD5933_STAT_DATA_VALID) == 0) {
		status = ad5933_get_register_value(dev,
						   AD5933_REG_STATUS,
						   1);
	};
}

/***************************************************************************//**
 * @brief Reads the real and imaginary value from register
 *
 * @param dev                   - The device structure.
 * @param freq_function         - Frequency function.
 * @param imag_data         	- Pointer to imaginary data
 * @param real_data         	- Pointer to real data
 *
 * @return None.
*******************************************************************************/
void ad5933_get_data(struct ad5933_dev *dev,
		     uint8_t freq_function,
		     short *imag_data,
		     short *real_data)
{
	uint8_t status = 0;

	if (!dev)
		return;

	ad5933_set_register_value(dev,
				  AD5933_REG_CONTROL_HB,
				  AD5933_CONTROL_FUNCTION(freq_function) |
				  AD5933_CONTROL_RANGE(dev->current_range) |
				  AD5933_CONTROL_PGA_GAIN(dev->current_gain),
				  1);

	while((status & AD5933_STAT_DATA_VALID) == 0) {
		status = ad5933_get_register_value(dev,
						   AD5933_REG_STATUS,
						   1);
	}
	*real_data = ad5933_get_register_value(dev,
					       AD5933_REG_REAL_DATA,
					       2);
	*imag_data = ad5933_get_register_value(dev,
					       AD5933_REG_IMAG_DATA,
					       2);
}

/***************************************************************************//**
 * @brief Reads the real and the imaginary data and calculates the Gain Factor.
 *
 * @param dev                   - The device structure.
 * @param calibration_impedance - The calibration impedance value.
 * @param freq_function         - Frequency function.
 *                               Example: AD5933_FUNCTION_INC_FREQ - Increment
                                          freq.;
 *                                        AD5933_FUNCTION_REPEAT_FREQ - Repeat
                                          freq..
 *
 * @return gainFactor          - Calculated gain factor.
*******************************************************************************/
double ad5933_calculate_gain_factor(struct ad5933_dev *dev,
				    uint32_t calibration_impedance,
				    uint8_t freq_function)
{
	double gain_factor = 0;
	double magnitude = 0;
	signed short real_data = 0;
	signed short imag_data = 0;

	ad5933_get_data(dev, freq_function, &imag_data, &real_data);

	magnitude = sqrt((real_data * real_data) + (imag_data * imag_data));
	gain_factor = 1 / (magnitude * calibration_impedance);

	return gain_factor;
}

/***************************************************************************//**
 * @brief Reads the real and the imaginary data and calculates the Impedance.
 *
 * @param dev           - The device structure.
 * @param gain_factor   - The gain factor.
 * @param freq_function - Frequency function.
 *                       Example: AD5933_FUNCTION_INC_FREQ - Increment freq.;
 *                                AD5933_FUNCTION_REPEAT_FREQ - Repeat freq..
 *
 * @return impedance   - Calculated impedance.
*******************************************************************************/
double ad5933_calculate_impedance(struct ad5933_dev *dev,
				  double gain_factor,
				  uint8_t freq_function)
{
	signed short real_data = 0;
	signed short imag_data = 0;
	double magnitude = 0;
	double impedance = 0;

	ad5933_get_data(dev, freq_function, &imag_data, &real_data);

	magnitude = sqrt((real_data * real_data) + (imag_data * imag_data));

	impedance =  1 / (magnitude * gain_factor);

	return impedance;
}

/***************************************************************************//**
 * @brief Selects the number of settling cycles of the device.
 *
 * @param dev		 - The device structure.
 * @param number_cycles	 - 9-bit number of cycles to wait before triggering ADC
 *
 * @param multiplier - Multiply number of cycles by X1, X2 or X4
 *                Example: AD5933_SETTLING_X1
 *                         AD5933_SETTLING_X2
 *                         AD5933_SETTLING_X4
 *
 * @return None.
*******************************************************************************/
void ad5933_set_settling_time(struct ad5933_dev *dev,
			      uint8_t multiplier,
			      uint16_t number_cycles)
{
	if ((multiplier != AD5933_SETTLING_X2) && (multiplier != AD5933_SETTLING_X4))
		multiplier = AD5933_SETTLING_X1;

	ad5933_set_register_value(dev,
				  AD5933_REG_SETTLING_CYCLES,
				  number_cycles | (multiplier << 9),
				  2);
}
