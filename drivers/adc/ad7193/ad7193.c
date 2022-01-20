/***************************************************************************//**
 *   @file   AD7193.c
 *   @brief  Implementation of AD7193 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad7193.h"    // AD7193 definitions.
#include "no-os/error.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD7193 Board and resets the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_init(struct ad7193_dev **device,
		struct ad7193_init_param init_param)
{
	struct ad7193_dev *dev;
	uint32_t reg_val;
	int ret;

	dev = (struct ad7193_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI */
	ret = spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret != SUCCESS)
		goto error_dev;

	/* GPIO */
	ret = gpio_get(&dev->gpio_miso, &init_param.gpio_miso);
	if (ret != SUCCESS)
		goto error_spi;

	ret = gpio_direction_input(dev->gpio_miso);
	if (ret != SUCCESS)
		goto error_miso;

	ret = ad7193_get_register_value(dev, AD7193_REG_ID, 1, &reg_val);
	if (ret != SUCCESS)
		goto error_miso;

	if((reg_val & AD7193_ID_MASK) != ID_AD7193) {
		goto error_miso;
	}

	/* Reset */
	ret = ad7193_reset(dev);
	if (ret != SUCCESS)
		goto error_miso;

	/* Initialization */
	ret = ad7193_range_setup(dev, init_param.current_polarity,
				 init_param.current_gain_code);
	if (ret != SUCCESS)
		goto error_miso;

	ret = ad7193_output_rate_select(dev, init_param.data_rate_code);
	if (ret != SUCCESS)
		goto error_miso;

	ret = ad7193_buffer_select(dev, init_param.buffer);
	if (ret != SUCCESS)
		goto error_miso;

	ret = ad7193_config_input_mode(dev, init_param.input_mode);
	if (ret != SUCCESS)
		goto error_miso;

	ret = ad7193_clock_select(dev, init_param.clock_source);
	if (ret != SUCCESS)
		goto error_miso;

	ret = ad7193_set_bridge_switch(dev, init_param.bpdsw_mode);
	if (ret != SUCCESS)
		goto error_miso;

	ret = ad7193_set_operating_mode(dev, init_param.operating_mode);
	if (ret != SUCCESS)
		goto error_miso;

	*device = dev;

	return ret;

error_miso:
	gpio_remove(dev->gpio_miso);
error_spi:
	spi_remove(dev->spi_desc);
error_dev:
	free(dev);

	return FAILURE;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7193_init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_remove(struct ad7193_dev *dev)
{
	int ret;

	ret = spi_remove(dev->spi_desc);
	if (ret != SUCCESS)
		return ret;

	ret = gpio_remove(dev->gpio_miso);
	if (ret != SUCCESS)
		return ret;

	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param reg_addr         - Address of the register.
 * @param reg_val          - Data value to write.
 * @param bytes_number     - Number of bytes to be written.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_set_register_value(struct ad7193_dev *dev,
			      uint8_t reg_addr,
			      uint32_t reg_val,
			      uint8_t bytes_number)
{
	uint8_t write_command[5] = {0, 0, 0, 0, 0};
	uint8_t* data_pointer    = (uint8_t*)&reg_val;
	uint8_t bytes_nr         = bytes_number;
	int ret;

	write_command[0] = AD7193_COMM_WRITE | AD7193_COMM_ADDR(reg_addr);
	while (bytes_nr > 0) {
		write_command[bytes_nr] = *data_pointer;
		data_pointer++;
		bytes_nr--;
	}

	ret = spi_write_and_read(dev->spi_desc, write_command, bytes_number + 1);
	if (ret != SUCCESS)
		return ret;

	return ret;
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev              - The device structure.
 * @param reg_addr         - Address of the register.
 * @param bytes_number     - Number of bytes that will be read.
 * @param reg_data         - Data read from the register.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_get_register_value(struct ad7193_dev *dev, uint8_t reg_addr,
			      uint8_t bytes_number, uint32_t *reg_data)
{
	uint8_t reg_word[5] = {0, 0, 0, 0, 0};
	uint8_t i = 0;
	int ret;

	if (!reg_data)
		return FAILURE;

	reg_word[0] = AD7193_COMM_READ | AD7193_COMM_ADDR(reg_addr);

	ret = spi_write_and_read(dev->spi_desc, reg_word, bytes_number + 1);
	if (ret != SUCCESS)
		return ret;

	*reg_data = 0; // Clearing the buffer
	for(i = 1; i < bytes_number + 1; i++) {
		*reg_data = (*reg_data << 8) + reg_word[i];
	}

	return ret;
}

/***************************************************************************//**
 * @brief Write masked data into device register.
 *
 * @param dev              - The device structure.
 * @param reg_addr         - Address of the register.
 * @param mask             - The mask.
 * @param reg_data         - Register data to be written.
 * @param bytes            - Number of bytes that will be read.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_set_masked_register_value(struct ad7193_dev *dev,
				     uint8_t reg_addr, uint32_t mask, uint32_t reg_data,
				     uint8_t bytes)
{
	uint32_t old_reg_data = 0x00;
	uint32_t new_reg_data = 0x00;
	int ret;

	ret = ad7193_get_register_value(dev, reg_addr, bytes, &old_reg_data);
	if (ret != SUCCESS)
		return ret;

	old_reg_data &= ~mask;
	new_reg_data = old_reg_data | reg_data;

	return ad7193_set_register_value(dev, reg_addr, new_reg_data, bytes);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_reset(struct ad7193_dev *dev)
{
	uint8_t register_word[6] = {0, 0, 0, 0, 0, 0};

	register_word[0] = 0xFF;
	register_word[1] = 0xFF;
	register_word[2] = 0xFF;
	register_word[3] = 0xFF;
	register_word[4] = 0xFF;
	register_word[5] = 0xFF;
	return spi_write_and_read(dev->spi_desc, register_word, 6);
}

/***************************************************************************//**
 * @brief Sets device into the specified operating mode.
 *
 * @param dev        - The device structure.
 * @param opt_mode	 - Operating mode to be set.
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_set_operating_mode(struct ad7193_dev *dev,
			      uint8_t opt_mode)
{
	int ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_MODE,
					       AD7193_MODE_SEL(0x7), AD7193_MODE_SEL(opt_mode),
					       3);

	if (ret == SUCCESS) {
		/* Store the last settings regarding operating mode. */
		dev->operating_mode = opt_mode;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Waits for RDY pin to go low.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_wait_rdy_go_low(struct ad7193_dev *dev)
{
	uint8_t wait = 1;
	uint16_t timeout = 0xFFFF;
	int ret;

	while (wait && (timeout > 0)) {
		ret = gpio_get_value(dev->gpio_miso, &wait);
		if (ret != SUCCESS)
			break;
		timeout--;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects the channels to be enabled.
 *
 * @param dev      - The device structure.
 * @param chn_mask - Channel mask.
 *                  Example: AD7193_CH_0 - AIN1(+) - AIN2(-);  (Pseudo = 0)
 *                           AD7193_CH_1 - AIN3(+) - AIN4(-);  (Pseudo = 0)
 *                           AD7193_TEMP - Temperature sensor
 *                           AD7193_SHORT - AIN2(+) - AIN2(-); (Pseudo = 0)
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_channels_select(struct ad7193_dev *dev,
			   uint16_t chn_mask)
{
	if (chn_mask > 0x3FF) {
		return -EINVAL;
	}

	return ad7193_set_masked_register_value(dev, AD7193_REG_CONF,
						AD7193_CONF_CHAN(0x3FF), AD7193_CONF_CHAN(chn_mask),
						3);
}

/***************************************************************************//**
 * @brief Performs the given calibration to the specified channel.
 *
 * @param dev     - The device structure.
 * @param mode    - Calibration type.
 * @param channel - Channel to be calibrated.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_calibrate(struct ad7193_dev *dev,
		     uint8_t mode, uint8_t channel)
{
	int ret;

	ret = ad7193_channels_select(dev, channel);
	if (ret != SUCCESS)
		return ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_MODE,
					       AD7193_MODE_SEL(0x7), AD7193_MODE_SEL(mode),
					       3);
	if (ret != SUCCESS)
		return ret;

	return ad7193_wait_rdy_go_low(dev);
}

/***************************************************************************//**
 * @brief Configures the input mode of the ADC
 *
 * @param dev   - The device structure.
 * @param mode  - Mode selection.
 *				  Example: 0 - Differential Analog Inputs
 *						   1 - Pseudo Differential Analog Inputs
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_config_input_mode(struct ad7193_dev *dev, uint8_t mode)
{
	int ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_CONF,
					       AD7193_CONF_PSEUDO, (AD7193_CONF_PSEUDO * mode),
					       3);

	if (ret == SUCCESS) {
		/* Store the last settings regarding input mode. */
		dev->input_mode = mode;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Enables or disables the buffer on the ADC input channels
 *
 * @param dev      - The device structure.
 * @param buff_en  - Enable/disable option.
 *                   Example: 0 - Buffer disabled
 *                            1 - Buffer Enabled
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_buffer_select(struct ad7193_dev *dev, uint8_t buff_en)
{
	int ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_CONF,
					       AD7193_CONF_BUF, (AD7193_CONF_BUF * buff_en),
					       3);

	if (ret == SUCCESS) {
		/* Store the last settings regarding buffer selection. */
		dev->buffer = buff_en;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects the filter output data rate of the ADC.
 *
 * @param dev			 - The device structure.
 * @param out_rate_code  - Filter output data rate code.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_output_rate_select(struct ad7193_dev *dev,
			      uint16_t out_rate_code)
{
	int ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_MODE,
					       AD7193_MODE_RATE(0x3FF), AD7193_MODE_RATE(out_rate_code),
					       3);

	if (ret == SUCCESS) {
		/* Store the last settings regarding filter output data rate. */
		dev->data_rate_code = out_rate_code;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects the clock source of the ADC.
 *
 * @param dev         - The device structure.
 * @param clk_select  - Clock source to be selected.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_clock_select(struct ad7193_dev *dev, uint16_t clk_select)
{
	int ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_MODE,
					       AD7193_MODE_CLKSRC(0x3), AD7193_MODE_CLKSRC(clk_select),
					       3);

	if (ret == SUCCESS) {
		/* Store the last settings regarding clock source. */
		dev->clock_source = clk_select;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Opens or closes the bridge power-down switch of the ADC.
 *
 * @param dev          - The device structure.
 * @param bpdsw_select - Option to open or close bridge power-down switch.
 *						 0 - Switch Opened
 *						 1 - Switch Closed
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_set_bridge_switch(struct ad7193_dev *dev, uint8_t bpdsw_select)
{
	int ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_GPOCON,
					       AD7193_GPOCON_BPDSW, (AD7193_GPOCON_BPDSW * bpdsw_select),
					       1);

	if (ret == SUCCESS) {
		/* Store the last settings regarding bridge power-down switch */
		dev->bpdsw_mode = bpdsw_select;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Selects the polarity of the conversion and the ADC input range.
 *
 * @param dev      - The device structure.
 * @param polarity - Polarity select bit.
 *                   Example: 0 - bipolar operation is selected.
 *                            1 - unipolar operation is selected.
 *@param range     - Gain select bits. These bits are written by the user to select
 *                   the ADC input range.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_range_setup(struct ad7193_dev *dev,
		       uint8_t polarity, uint8_t range)
{
	int ret;

	ret = ad7193_set_masked_register_value(dev, AD7193_REG_CONF,
					       (AD7193_CONF_UNIPOLAR | AD7193_CONF_GAIN(0x7)),
					       (polarity * AD7193_CONF_UNIPOLAR) | AD7193_CONF_GAIN(range),
					       3);

	if (ret == SUCCESS) {
		/* Store the last settings regarding polarity and gain. */
		dev->current_polarity = polarity;
		dev->current_gain     = 1 << range;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Returns the result of a single conversion.
 *
 * @param dev - The device structure.
 * @param reg_data - Buffer to store sampled register data
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_single_conversion(struct ad7193_dev *dev, uint32_t *reg_data)
{
	uint32_t command = 0x0;
	int ret;

	if (!reg_data)
		return FAILURE;

	command = AD7193_MODE_SEL(AD7193_MODE_SINGLE) | AD7193_MODE_CLKSRC(
			  AD7193_CLK_INT) | AD7193_MODE_RATE(dev->data_rate_code);

	ret = ad7193_set_register_value(dev, AD7193_REG_MODE, command, 3);
	if (ret != SUCCESS)
		return ret;

	ret = ad7193_wait_rdy_go_low(dev);
	if (ret != SUCCESS)
		return ret;

	return ad7193_get_register_value(dev, AD7193_REG_DATA, 3, reg_data);
}

/***************************************************************************//**
 * @brief Returns the average of several conversion results.
 *
 * @param dev - The device structure.
 * @param sample_number - the number of samples
 * @param samples_avg - Average of the samples read
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_continuous_read_avg(struct ad7193_dev *dev,
			       uint8_t sample_number, uint32_t *samples_avg)
{
	uint32_t samples = 0;
	uint32_t command = 0;
	uint8_t count = 0;
	int ret;

	command = AD7193_MODE_SEL(AD7193_MODE_CONT) |
		  AD7193_MODE_CLKSRC(AD7193_CLK_INT) |
		  AD7193_MODE_RATE(dev->data_rate_code);

	ret = ad7193_set_register_value(dev, AD7193_REG_MODE, command, 3);
	if (ret != SUCCESS)
		return ret;

	for (count = 0; count < sample_number; count++) {
		ret = ad7193_wait_rdy_go_low(dev);
		if (ret != SUCCESS)
			return ret;

		ret = ad7193_get_register_value(dev, AD7193_REG_DATA, 3, &samples);
		if(ret != SUCCESS)
			return ret;

		*samples_avg += samples;
	}

	*samples_avg = *samples_avg / sample_number;

	return SUCCESS;
}

/***************************************************************************//**
 * @brief Read data from temperature sensor and converts it to Celsius degrees.
 *
 * @param dev - The device structure.
 * @param temp - Stores the temperature result.
 *
 * @return SUCCESS in case of success or negative error code.
*******************************************************************************/
int ad7193_temperature_read(struct ad7193_dev *dev, float *temp)
{
	uint32_t data_reg = 0;
	int ret;

	// Bipolar operation, 0 Gain.
	ret = ad7193_range_setup(dev, 0, AD7193_CONF_GAIN_1);
	if (ret != SUCCESS)
		return ret;

	ret = ad7193_channels_select(dev, AD7193_CH_MASK(AD7193_CH_TEMP));
	if (ret != SUCCESS)
		return ret;

	ret = ad7193_single_conversion(dev, &data_reg);
	if (ret != SUCCESS)
		return ret;

	data_reg -= 0x800000;
	*temp = (float) data_reg / 2815;   // Kelvin Temperature
	*temp -= 273;                      // Celsius Temperature

	return ret;
}

/***************************************************************************//**
 * @brief Converts 24-bit raw data to milivolts.
 *
 * @param dev       - The device structure.
 * @param raw_data  - 24-bit data sample.
 * @param v_ref     - The value of the voltage reference used by the device.
 *
 * @return voltage - The result of the conversion expressed as volts.
*******************************************************************************/
float ad7193_convert_to_volts(struct ad7193_dev *dev,
			      uint32_t raw_data,
			      float v_ref)
{
	float voltage = 0;

	if(dev->current_polarity == 0 ) { // Bipolar mode
		voltage = 1000 * (((float)raw_data / (1ul << 23)) - 1) * v_ref /
			  dev->current_gain;
	} else {                    // Unipolar mode
		voltage = 1000 * ((float)raw_data * v_ref) / (1ul << 24) / dev->current_gain;
	}

	return voltage;
}
