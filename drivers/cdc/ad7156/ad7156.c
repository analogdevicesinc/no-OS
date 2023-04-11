/***************************************************************************//**
 *   @file   AD7156.c
 *   @brief  Implementation of AD7156 Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include "ad7156.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Performs a burst read of a specified number of registers.
 *
 * @param dev               - The device structure.
 * @param p_read_data       - The read values are stored in this buffer.
 * @param register_address  - The start address of the burst read.
 * @param bytes_number      - Number of bytes to read.
 *
 * @return None.
*******************************************************************************/
void ad7156_get_register_value(struct ad7156_dev *dev,
			       uint8_t* p_read_data,
			       uint8_t register_address,
			       uint8_t bytes_number)
{
	no_os_i2c_write(dev->i2c_desc, &register_address, 1, 0);
	no_os_i2c_read(dev->i2c_desc, p_read_data, bytes_number, 1);
}

/***************************************************************************//**
 * @brief Writes data into one or two registers.
 *
 * @param dev              - The device structure.
 * @param register_value   - Data value to write.
 * @param register_address - Address of the register.
 * @param bytes_number     - Number of bytes. Accepted values: 0 - 1.
 *
 * @return None.
*******************************************************************************/
void ad7156_set_register_value(struct ad7156_dev *dev,
			       uint16_t register_value,
			       uint8_t  register_address,
			       uint8_t  bytes_number)
{
	uint8_t data_buffer[3] = {0, 0, 0};

	data_buffer[0] = register_address;
	data_buffer[1] = (register_value >> 8);
	data_buffer[bytes_number] = (register_value & 0x00FF);
	no_os_i2c_write(dev->i2c_desc, data_buffer, bytes_number + 1, 1);
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD7156 Board.
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
int8_t ad7156_init(struct ad7156_dev **device,
		   struct ad7156_init_param init_param)
{
	struct ad7156_dev *dev;
	int8_t status = -1;
	uint8_t test = 0;

	dev = (struct ad7156_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->ad7156_channel1_range = init_param.ad7156_channel1_range;
	dev->ad7156_channel2_range = init_param.ad7156_channel2_range;

	status = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
	ad7156_get_register_value(dev,
				  &test,
				  AD7156_REG_CHIP_ID,
				  1);
	if(test != AD7156_DEFAULT_ID) {
		status = -1;
	}

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7156_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad7156_remove(struct ad7156_dev *dev)
{
	int32_t status;

	status = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return status;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad7156_reset(struct ad7156_dev *dev)
{
	uint8_t address_pointer = 0;

	address_pointer = AD7156_RESET_CMD;
	no_os_i2c_write(dev->i2c_desc, &address_pointer, 1, 1);
}

/***************************************************************************//**
 * @brief Sets the converter mode of operation.
 *
 * @param dev      - The device structure.
 * @param pwr_mode - Mode of operation option.
 *		    Example: AD7156_CONV_MODE_IDLE - Idle
 *                           AD7156_CONV_MODE_CONT_CONV  - Continuous conversion
 *                           AD7156_CONV_MODE_SINGLE_CONV - Single conversion
 *                           AD7156_CONV_MODE_PWR_DWN - Power-down
 *
 * @return None.
*******************************************************************************/
void ad7156_set_power_mode(struct ad7156_dev *dev,
			   uint8_t pwr_mode)
{
	uint8_t old_config_reg = 0;
	uint8_t new_config_reg = 0;

	ad7156_get_register_value(dev,
				  &old_config_reg,
				  AD7156_REG_CONFIG,
				  1);
	old_config_reg &= ~AD7156_CONFIG_MD(0x3);
	new_config_reg = old_config_reg| AD7156_CONFIG_MD(pwr_mode);
	ad7156_set_register_value(dev,
				  new_config_reg,
				  AD7156_REG_CONFIG,
				  1);
}

/***************************************************************************//**
 * @brief Enables or disables conversion on the selected channel.
 *
 * @param dev         - The device structure.
 * @param channel     - Channel option.
 *                      Example: AD7156_CHANNEL1
 *                               AD7156_CHANNEL2
 * @param enable_conv - The state of channel activity.
 *                      Example: 0 - disable conversion on selected channel.
 *                               1 - enable conversion on selected channel.
 *
 * @return None.
*******************************************************************************/
void ad7156_channel_state(struct ad7156_dev *dev,
			  uint8_t channel,
			  uint8_t enable_conv)
{
	uint8_t old_config_reg = 0;
	uint8_t new_config_reg = 0;
	uint8_t channel_mask = 0;

	channel_mask = (channel == 1) ? AD7156_CONFIG_EN_CH1 : AD7156_CONFIG_EN_CH2;

	ad7156_get_register_value(dev,
				  &old_config_reg,
				  AD7156_REG_CONFIG,
				  1);
	old_config_reg &= ~channel_mask;
	new_config_reg = old_config_reg | (channel_mask * enable_conv);
	ad7156_set_register_value(dev,
				  new_config_reg,
				  AD7156_REG_CONFIG,
				  1);
}

/***************************************************************************//**
 * @brief Sets the input range of the specified channel.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @param range   - Input range option.
 *                  Example: AD7156_CDC_RANGE_2_PF   - 2pF input range.
 *                           AD7156_CDC_RANGE_0_5_PF - 0.5pF input range.
 *                           AD7156_CDC_RANGE_1_PF   - 1pF input range.
 *                           AD7156_CDC_RANGE_4_PF   - 4pF input range.
 *
 * @return None.
*******************************************************************************/
void ad7156_set_range(struct ad7156_dev *dev,
		      uint32_t channel,
		      uint8_t range)
{
	uint8_t old_setup_reg = 0;
	uint8_t new_setup_reg = 0;
	uint8_t reg_address = 0;

	reg_address = (channel == 1) ? AD7156_REG_CH1_SETUP : AD7156_REG_CH2_SETUP;
	ad7156_get_register_value(dev,
				  &old_setup_reg,
				  reg_address,
				  1);
	old_setup_reg &= ~AD7156_CH1_SETUP_RANGE(0x3);
	new_setup_reg = old_setup_reg | AD7156_CH1_SETUP_RANGE(range);
	ad7156_set_register_value(dev,
				  new_setup_reg,
				  reg_address,
				  1);
	/* Update global variables that hold range information. */
	if(channel == 1) {
		dev->ad7156_channel1_range = ad7156_get_range(dev,
					     channel);
	} else {
		dev->ad7156_channel2_range = ad7156_get_range(dev,
					     channel);
	}
}

/***************************************************************************//**
 * @brief Reads the range bits from the device and returns the range in pF.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 *
 * @return The capacitive input range(pF).
*******************************************************************************/
float ad7156_get_range(struct ad7156_dev *dev,
		       uint32_t channel)
{
	uint8_t setup_reg = 0;
	uint8_t reg_address = 0;
	float range = 0;

	reg_address = (channel == 1) ? AD7156_REG_CH1_SETUP : AD7156_REG_CH2_SETUP;
	ad7156_get_register_value(dev,
				  &setup_reg,
				  reg_address,
				  1);
	setup_reg = (setup_reg & AD7156_CH1_SETUP_RANGE(0x3)) >> 6;
	switch(setup_reg) {
	case AD7156_CDC_RANGE_2_PF:
		range =  2.0;
		break;
	case AD7156_CDC_RANGE_0_5_PF:
		range = 0.5;
		break;
	case AD7156_CDC_RANGE_1_PF:
		range =  1.0;
		break;
	case AD7156_CDC_RANGE_4_PF:
		range =  4.0;
		break;
	}
	/* Update global variables that hold range information. */
	if(channel == 1) {
		dev->ad7156_channel1_range = range;
	} else {
		dev->ad7156_channel2_range = range;
	}

	return range;
}

/***************************************************************************//**
 * @brief Selects the threshold mode of operation.
 *
 * @param dev       - The device structure.
 * @param thr_mode  - Output comparator mode.
 *                   Example: AD7156_THR_MODE_NEGATIVE
 *                            AD7156_THR_MODE_POSITIVE
 *                            AD7156_THR_MODE_IN_WINDOW
 *                            AD7156_THR_MODE_OU_WINDOW
 * @param thr_fixed - Selects the threshold mode.
 *                   Example: AD7156_ADAPTIVE_THRESHOLD
 *                            AD7156_FIXED_THRESHOLD
 *
 * @return None.
*******************************************************************************/
void ad7156_set_threshold_mode(struct ad7156_dev *dev,
			       uint8_t thr_mode,
			       uint8_t thr_fixed)
{
	uint8_t old_config_reg = 0;
	uint8_t new_config_reg = 0;

	ad7156_get_register_value(dev,
				  &old_config_reg,
				  AD7156_REG_CONFIG,
				  1);
	old_config_reg &= ~(AD7156_CONFIG_THR_FIXED | AD7156_CONFIG_THR_MD(0x3));
	new_config_reg = old_config_reg |
			 (AD7156_CONFIG_THR_FIXED * thr_fixed) |
			 (AD7156_CONFIG_THR_MD(thr_mode));
	ad7156_set_register_value(dev,
				  new_config_reg,
				  AD7156_REG_CONFIG,
				  1);
}

/***************************************************************************//**
 * @brief Writes to the threshold register when threshold fixed mode is enabled.
 *
 * @param dev      - The device structure.
 * @param channel  - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @param p_fthr   - The threshold value in picofarads(pF). The value must not be
 *                  out of the selected input range.
 *
 * @return None.
*******************************************************************************/
void ad7156_set_threshold(struct ad7156_dev *dev,
			  uint8_t channel,
			  float p_fthr)
{
	uint8_t thr_reg_address = 0;
	uint16_t raw_thr = 0;
	float range = 0;

	thr_reg_address = (channel == 1) ? AD7156_REG_CH1_SENS_THRSH_H :
			  AD7156_REG_CH2_SENS_THRSH_H;
	range = ad7156_get_range(dev,
				 channel);
	raw_thr = (uint16_t)((p_fthr * 0xA000 / range) + 0x3000);
	if(raw_thr > 0xD000) {
		raw_thr = 0xD000;
	} else if(raw_thr < 0x3000) {
		raw_thr = 0x3000;
	}
	ad7156_set_register_value(dev,
				  raw_thr,
				  thr_reg_address,
				  2);
}

/***************************************************************************//**
 * @brief Writes a value(pF) to the sensitivity register. This functions
 * should be used when adaptive threshold mode is selected.
 *
 * @param dev            - The device structure.
 * @param channel        - Channel option.
 *                        Example: AD7156_CHANNEL1
 *                                 AD7156_CHANNEL2
 * @param p_fsensitivity - The sensitivity value in picofarads(pF).
 *
 * @return None.
*******************************************************************************/
void ad7156_set_sensitivity(struct ad7156_dev *dev,
			    uint8_t channel,
			    float p_fsensitivity)
{
	uint8_t sensitivity_reg_addr = 0;
	uint16_t raw_sensitivity = 0;
	float range = 0;

	sensitivity_reg_addr = (channel == 1) ? AD7156_REG_CH1_SENS_THRSH_H :
			       AD7156_REG_CH2_SENS_THRSH_H;
	range = (channel == 1) ? dev->ad7156_channel1_range :
		dev->ad7156_channel2_range;
	raw_sensitivity = (uint16_t)(p_fsensitivity * 0xA00 / range);
	raw_sensitivity = (raw_sensitivity << 4) & 0x0FF0;
	ad7156_set_register_value(dev,
				  raw_sensitivity,
				  sensitivity_reg_addr,
				  2);
}

/***************************************************************************//**
 * @brief Reads a 12-bit sample from the selected channel.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel.
*******************************************************************************/
uint16_t ad7156_read_channel_data(struct ad7156_dev *dev,
				  uint8_t channel)
{
	uint16_t ch_result = 0;
	uint8_t reg_data[2] = {0, 0};
	uint8_t ch_address = 0;

	if(channel == 1) {
		ch_address = AD7156_REG_CH1_DATA_H;
	} else {
		ch_address = AD7156_REG_CH2_DATA_H;
	}
	ad7156_get_register_value(dev,
				  reg_data,
				  ch_address,
				  2);
	ch_result = (reg_data[0] << 8) + reg_data[1];

	return ch_result;
}

/***************************************************************************//**
 * @brief Waits for a finished CDC conversion and reads a 12-bit sample from
 *        the selected channel.
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel.
*******************************************************************************/
uint16_t ad7156_wait_read_channel_data(struct ad7156_dev *dev,
				       uint8_t channel)
{
	uint16_t ch_result = 0;
	uint8_t reg_data[2] = {0, 0};
	uint8_t status = 0;
	uint8_t ch_rdy_mask = 0;
	uint8_t ch_address = 0;

	if(channel == 1) {
		ch_rdy_mask = AD7156_STATUS_RDY1;
		ch_address = AD7156_REG_CH1_DATA_H;
	} else {
		ch_rdy_mask = AD7156_STATUS_RDY2;
		ch_address = AD7156_REG_CH2_DATA_H;
	}
	do {
		ad7156_get_register_value(dev,
					  &status,
					  AD7156_REG_STATUS,
					  1);
	} while((status & ch_rdy_mask) != 0);
	ad7156_get_register_value(dev,
				  reg_data,
				  ch_address,
				  2);
	ch_result = (reg_data[0] << 8) + reg_data[1];

	return ch_result;
}

/***************************************************************************//**
 * @brief Reads a sample the selected channel and converts the data to
 *        picofarads(pF).
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel as picofarads(pF).
*******************************************************************************/
float ad7156_read_channel_capacitance(struct ad7156_dev *dev,
				      uint8_t channel)
{
	uint16_t raw_ch = 0;
	float ch_range = 0;
	float p_fdata = 0;

	ch_range = (channel == 1) ? dev->ad7156_channel1_range :
		   dev->ad7156_channel2_range;
	raw_ch = ad7156_read_channel_data(dev,
					  channel);
	if(raw_ch < 0x3000) {
		raw_ch= 0x3000;
	} else if(raw_ch > 0xD000) {
		raw_ch = 0xD000;
	}
	p_fdata = (((raw_ch) - 0x3000) * ch_range) / 0xA000;

	return p_fdata;
}

/***************************************************************************//**
 * @brief Waits for a finished CDC conversion the selected channel, reads a
 *        sample and converts the data to picofarads(pF).
 *
 * @param dev     - The device structure.
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel as picofarads(pF).
*******************************************************************************/
float ad7156_wait_read_channel_capacitance(struct ad7156_dev *dev,
		uint8_t channel)
{
	uint16_t raw_ch = 0;
	float ch_range = 0;
	float p_fdata = 0;

	ch_range = (channel == 1) ? dev->ad7156_channel1_range :
		   dev->ad7156_channel2_range;
	raw_ch = ad7156_wait_read_channel_data(dev,
					       channel);
	if(raw_ch < 0x3000) {
		raw_ch= 0x3000;
	} else if(raw_ch > 0xD000) {
		raw_ch = 0xD000;
	}
	p_fdata = (((raw_ch) - 0x3000) * ch_range) / 0xA000;

	return p_fdata;
}
