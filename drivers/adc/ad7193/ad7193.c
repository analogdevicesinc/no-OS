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
 * @return ret - The result of the initialization procedure.
 *               Example: -1 - SPI peripheral was not initialized or the
 *                             device is not present.
 *                         0 - SPI peripheral was initialized and the
 *                             device is present.
*******************************************************************************/
int8_t ad7193_init(struct ad7193_dev **device,
		   struct ad7193_init_param init_param)
{
	struct ad7193_dev *dev;
	int8_t status = 0;
	uint8_t reg_val = 0;

	dev = (struct ad7193_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->current_polarity = init_param.current_polarity;
	dev->current_gain = init_param.current_gain;

	/* SPI */
	status = spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	status |= gpio_get(&dev->gpio_cs, init_param.gpio_cs);
	status |= gpio_get(&dev->gpio_miso, init_param.gpio_miso);

	if (dev->gpio_cs)
		status |= gpio_direction_output(dev->gpio_cs,
						GPIO_HIGH);
	if (dev->gpio_miso)
		status |= gpio_direction_input(dev->gpio_miso);

	reg_val = ad7193_get_register_value(dev,
					    AD7193_REG_ID,
					    1,
					    1);
	if((reg_val & AD7193_ID_MASK) != ID_AD7193) {
		status = -1;
	}

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7193_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad7193_remove(struct ad7193_dev *dev)
{
	int32_t status;

	status = spi_remove(dev->spi_desc);

	status |= gpio_remove(dev->gpio_cs);
	status |= gpio_remove(dev->gpio_miso);

	free(dev);

	return status;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param register_value   - Data value to write.
 * @param bytes_number     - Number of bytes to be written.
 * @param modify_cs        - Allows Chip Select to be modified.
 *
 * @return none.
*******************************************************************************/
void ad7193_set_register_value(struct ad7193_dev *dev,
			       uint8_t register_address,
			       uint32_t register_value,
			       uint8_t bytes_number,
			       uint8_t modify_cs)
{
	uint8_t write_command[5] = {0, 0, 0, 0, 0};
	uint8_t* data_pointer    = (uint8_t*)&register_value;
	uint8_t bytes_nr         = bytes_number;

	write_command[0] = AD7193_COMM_WRITE |
			   AD7193_COMM_ADDR(register_address);
	while(bytes_nr > 0) {
		write_command[bytes_nr] = *data_pointer;
		data_pointer ++;
		bytes_nr --;
	}
	if (modify_cs)
		AD7193_CS_LOW;
	spi_write_and_read(dev->spi_desc, write_command, bytes_number + 1);
	if (modify_cs)
		AD7193_CS_HIGH;
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param bytes_number     - Number of bytes that will be read.
 * @param modify_cs        - Allows Chip Select to be modified.
 *
 * @return buffer          - Value of the register.
*******************************************************************************/
uint32_t ad7193_get_register_value(struct ad7193_dev *dev,
				   uint8_t register_address,
				   uint8_t bytes_number,
				   uint8_t modify_cs)
{
	uint8_t register_word[5] = {0, 0, 0, 0, 0};
	uint32_t buffer = 0x0;
	uint8_t i = 0;

	register_word[0] = AD7193_COMM_READ |
			   AD7193_COMM_ADDR(register_address);
	if (modify_cs)
		AD7193_CS_LOW;
	spi_write_and_read(dev->spi_desc, register_word, bytes_number + 1);
	if (modify_cs)
		AD7193_CS_HIGH;
	for(i = 1; i < bytes_number + 1; i++) {
		buffer = (buffer << 8) + register_word[i];
	}

	return buffer;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev - The device structure.
 *
 * @return none.
*******************************************************************************/
void ad7193_reset(struct ad7193_dev *dev)
{
	uint8_t register_word[6] = {0, 0, 0, 0, 0, 0};

	register_word[0] = 0xFF;
	register_word[1] = 0xFF;
	register_word[2] = 0xFF;
	register_word[3] = 0xFF;
	register_word[4] = 0xFF;
	register_word[5] = 0xFF;
	spi_write_and_read(dev->spi_desc, register_word, 6);
}

/***************************************************************************//**
 * @brief Set device to idle or power-down.
 *
 * @param dev      - The device structure.
 * @param pwr_mode - Selects idle mode or power-down mode.
 *                  Example: 0 - power-down
 *                           1 - idle
 *
 * @return none.
*******************************************************************************/
void ad7193_set_power(struct ad7193_dev *dev,
		      uint8_t pwr_mode)
{
	uint32_t old_pwr_mode = 0x0;
	uint32_t new_pwr_mode = 0x0;

	old_pwr_mode  = ad7193_get_register_value(dev,
			AD7193_REG_MODE,
			3,
			1);
	old_pwr_mode &= ~(AD7193_MODE_SEL(0x7));
	new_pwr_mode  = old_pwr_mode |
			AD7193_MODE_SEL((pwr_mode * (AD7193_MODE_IDLE)) |
					(!pwr_mode * (AD7193_MODE_PWRDN)));
	ad7193_set_register_value(dev,
				  AD7193_REG_MODE,
				  new_pwr_mode,
				  3,
				  1);
}

/***************************************************************************//**
 * @brief Waits for RDY pin to go low.
 *
 * @return none.
*******************************************************************************/
void ad7193_wait_rdy_go_low(struct ad7193_dev *dev)
{
	uint8_t wait = 1;

	while (wait)
		AD7193_RDY_STATE(wait);
}

/***************************************************************************//**
 * @brief Selects the channel to be enabled.
 *
 * @param dev     - The device structure.
 * @param channel - Selects a channel.
 *                  Example: AD7193_CH_0 - AIN1(+) - AIN2(-);  (Pseudo = 0)
 *                           AD7193_CH_1 - AIN3(+) - AIN4(-);  (Pseudo = 0)
 *                           AD7193_TEMP - Temperature sensor
 *                           AD7193_SHORT - AIN2(+) - AIN2(-); (Pseudo = 0)
 *
 * @return none.
*******************************************************************************/
void ad7193_channel_select(struct ad7193_dev *dev,
			   uint16_t channel)
{
	uint32_t old_reg_value = 0x0;
	uint32_t new_reg_value = 0x0;

	old_reg_value  = ad7193_get_register_value(dev,
			 AD7193_REG_CONF,
			 3,
			 1);
	old_reg_value &= ~(AD7193_CONF_CHAN(0x3FF));
	new_reg_value  = old_reg_value | AD7193_CONF_CHAN(1 << channel);
	ad7193_set_register_value(dev,
				  AD7193_REG_CONF,
				  new_reg_value,
				  3,
				  1);
}

/***************************************************************************//**
 * @brief Performs the given calibration to the specified channel.
 *
 * @param dev     - The device structure.
 * @param mode    - Calibration type.
 * @param channel - Channel to be calibrated.
 *
 * @return none.
*******************************************************************************/
void ad7193_calibrate(struct ad7193_dev *dev,
		      uint8_t mode,
		      uint8_t channel)
{
	uint32_t old_reg_value = 0x0;
	uint32_t new_reg_value = 0x0;

	ad7193_channel_select(dev,
			      channel);
	old_reg_value  = ad7193_get_register_value(dev,
			 AD7193_REG_MODE,
			 3,
			 1);
	old_reg_value &= ~AD7193_MODE_SEL(0x7);
	new_reg_value  = old_reg_value | AD7193_MODE_SEL(mode);
	AD7193_CS_LOW;
	ad7193_set_register_value(dev,
				  AD7193_REG_MODE,
				  new_reg_value,
				  3,
				  0); // CS is not modified.
	ad7193_wait_rdy_go_low(dev);
	AD7193_CS_HIGH;
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
 * @return none.
*******************************************************************************/
void ad7193_range_setup(struct ad7193_dev *dev,
			uint8_t polarity,
			uint8_t range)
{
	uint32_t old_reg_value = 0x0;
	uint32_t new_reg_value = 0x0;

	old_reg_value  = ad7193_get_register_value(dev,
			 AD7193_REG_CONF,
			 3,
			 1);
	old_reg_value &= ~(AD7193_CONF_UNIPOLAR |
			   AD7193_CONF_GAIN(0x7));
	new_reg_value  = old_reg_value |
			 (polarity * AD7193_CONF_UNIPOLAR) |
			 AD7193_CONF_GAIN(range);
	ad7193_set_register_value(dev,
				  AD7193_REG_CONF,
				  new_reg_value,
				  3,
				  1);
	/* Store the last settings regarding polarity and gain. */
	dev->current_polarity = polarity;
	dev->current_gain     = 1 << range;
}

/***************************************************************************//**
 * @brief Returns the result of a single conversion.
 *
 * @param dev - The device structure.
 *
 * @return regData - Result of a single analog-to-digital conversion.
*******************************************************************************/
uint32_t ad7193_single_conversion(struct ad7193_dev *dev)
{
	uint32_t command = 0x0;
	uint32_t reg_data = 0x0;

	command = AD7193_MODE_SEL(AD7193_MODE_SINGLE) |
		  AD7193_MODE_CLKSRC(AD7193_CLK_INT) |
		  AD7193_MODE_RATE(0x060);
	AD7193_CS_LOW;
	ad7193_set_register_value(dev,
				  AD7193_REG_MODE,
				  command,
				  3,
				  0); // CS is not modified.
	ad7193_wait_rdy_go_low(dev);
	reg_data = ad7193_get_register_value(dev,
					     AD7193_REG_DATA,
					     3,
					     0); // CS is not modified.
	AD7193_CS_HIGH;

	return reg_data;
}

/***************************************************************************//**
 * @brief Returns the average of several conversion results.
 *
 * @param dev - The device structure.
 *
 * @return samplesAverage - The average of the conversion results.
*******************************************************************************/
uint32_t ad7193_continuous_read_avg(struct ad7193_dev *dev,
				    uint8_t sample_number)
{
	uint32_t samples_average = 0;
	uint32_t command = 0;
	uint8_t count = 0;

	command = AD7193_MODE_SEL(AD7193_MODE_CONT) |
		  AD7193_MODE_CLKSRC(AD7193_CLK_INT) |
		  AD7193_MODE_RATE(0x060);
	AD7193_CS_LOW;
	ad7193_set_register_value(dev,
				  AD7193_REG_MODE,
				  command,
				  3,
				  0); // CS is not modified.
	for(count = 0; count < sample_number; count++) {
		ad7193_wait_rdy_go_low(dev);
		samples_average += ad7193_get_register_value(dev,
				   AD7193_REG_DATA,
				   3,
				   0); // CS is not modified.
	}
	AD7193_CS_HIGH;
	samples_average = samples_average / sample_number;

	return samples_average;
}

/***************************************************************************//**
 * @brief Read data from temperature sensor and converts it to Celsius degrees.
 *
 * @param dev - The device structure.
 *
 * @return temperature - Celsius degrees.
*******************************************************************************/
float ad7193_temperature_read(struct ad7193_dev *dev)
{
	uint32_t data_reg = 0;
	float temperature = 0;

	ad7193_range_setup(dev,
			   0,
			   AD7193_CONF_GAIN_1); // Bipolar operation, 0 Gain.
	ad7193_channel_select(dev,
			      AD7193_CH_TEMP);
	data_reg      = ad7193_single_conversion(dev);
	data_reg     -= 0x800000;
	temperature  = (float) data_reg / 2815;   // Kelvin Temperature
	temperature -= 273;                      // Celsius Temperature

	return temperature;
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
		voltage = 1000 * (((float)raw_data / (1ul << 23)) - 1) * v_ref / dev->current_gain;
	} else {                    // Unipolar mode
		voltage = 1000 * ((float)raw_data * v_ref) / (1ul << 24) / dev->current_gain;
	}

	return voltage;
}
