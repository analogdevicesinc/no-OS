/***************************************************************************//**
 *   @file   AD719X.c
 *   @brief  Implementation of AD7190/2/3/4/5 Driver.
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
#include "ad719x.h"    // AD719X definitions.
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include <string.h>

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD719X Board and resets the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_init(struct ad719x_dev **device,
		struct ad719x_init_param init_param)
{
	struct ad719x_dev *dev;
	uint32_t reg_val;
	int ret;

	dev = (struct ad719x_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->chip_id = init_param.chip_id;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret != 0)
		goto error_dev;

	/* GPIO */
	ret = no_os_gpio_get(&dev->gpio_miso, init_param.gpio_miso);
	if (ret != 0)
		goto error_spi;

	ret = no_os_gpio_direction_input(dev->gpio_miso);
	if (ret != 0)
		goto error_miso;

	ret = no_os_gpio_get_optional(&dev->sync_pin, init_param.sync_pin);
	if (ret != 0)
		goto error_miso;

	ret = no_os_gpio_direction_output(dev->sync_pin, NO_OS_GPIO_HIGH);
	if (ret != 0)
		goto error_sync;

	/* Reset */
	ret = ad719x_reset(dev);
	if (ret != 0)
		goto error_sync;

	ret = ad719x_get_register_value(dev, AD719X_REG_ID, 1, &reg_val);
	if (ret != 0)
		goto error_sync;

	switch (dev->chip_id) {
	case AD7190:
		if((reg_val & AD7190_4_ID_MASK) != AD7190) {
			goto error_sync;
		}
		break;
	case AD7192:
		if((reg_val & AD7190_4_ID_MASK) != AD7192) {
			goto error_sync;
		}
		break;
	case AD7193:
		if((reg_val & AD7190_4_ID_MASK) != AD7193) {
			goto error_sync;
		}
		break;
	case AD7194:
		if((reg_val & AD7190_4_ID_MASK) != AD7194) {
			goto error_sync;
		}
		break;
	case AD7195:
		if((reg_val & AD7195_ID_MASK) != AD7195) {
			goto error_sync;
		}
		break;
	default:
		return -1;
	}

	/* Initialization */
	ret = ad719x_range_setup(dev, init_param.current_polarity,
				 init_param.current_gain);
	if (ret != 0)
		goto error_sync;

	ret = ad719x_output_rate_select(dev, init_param.data_rate_code);
	if (ret != 0)
		goto error_sync;

	ret = ad719x_buffer_select(dev, init_param.buffer);
	if (ret != 0)
		goto error_sync;

	if(dev->chip_id == AD7193 || dev->chip_id == AD7194) {
		ret = ad719x_config_input_mode(dev, init_param.input_mode);
		if (ret != 0)
			goto error_sync;
	}

	ret = ad719x_clock_select(dev, init_param.clock_source);
	if (ret != 0)
		goto error_sync;

	if (dev->chip_id != AD7194) {
		ret = ad719x_set_bridge_switch(dev, init_param.bpdsw_mode);
		if (ret != 0)
			goto error_sync;
	}

	ret = ad719x_set_operating_mode(dev, init_param.operating_mode);
	if (ret != 0)
		goto error_sync;

	*device = dev;

	return ret;

error_sync:
	no_os_gpio_remove(dev->sync_pin);
error_miso:
	no_os_gpio_remove(dev->gpio_miso);
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return -1;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad719x_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_remove(struct ad719x_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_miso);
	if (ret != 0)
		return ret;

	no_os_free(dev);

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
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_set_register_value(struct ad719x_dev *dev,
			      uint8_t reg_addr,
			      uint32_t reg_val,
			      uint8_t bytes_number)
{
	uint8_t write_command[5] = {0, 0, 0, 0, 0};
	uint8_t* data_pointer    = (uint8_t*)&reg_val;
	uint8_t bytes_nr         = bytes_number;
	int ret;

	write_command[0] = AD719X_COMM_WRITE | AD719X_COMM_ADDR(reg_addr);
	while (bytes_nr > 0) {
		write_command[bytes_nr] = *data_pointer;
		data_pointer++;
		bytes_nr--;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, write_command, bytes_number + 1);
	if (ret != 0)
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
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_get_register_value(struct ad719x_dev *dev, uint8_t reg_addr,
			      uint8_t bytes_number, uint32_t *reg_data)
{
	uint8_t reg_word[5] = {0, 0, 0, 0, 0};
	uint8_t i = 0;
	int ret;

	if (!reg_data)
		return -1;

	reg_word[0] = AD719X_COMM_READ | AD719X_COMM_ADDR(reg_addr);

	ret = no_os_spi_write_and_read(dev->spi_desc, reg_word, bytes_number + 1);
	if (ret != 0)
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
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_set_masked_register_value(struct ad719x_dev *dev,
				     uint8_t reg_addr, uint32_t mask, uint32_t reg_data,
				     uint8_t bytes)
{
	uint32_t old_reg_data = 0x00;
	uint32_t new_reg_data = 0x00;
	int ret;

	ret = ad719x_get_register_value(dev, reg_addr, bytes, &old_reg_data);
	if (ret != 0)
		return ret;

	old_reg_data &= ~mask;
	new_reg_data = old_reg_data | reg_data;

	return ad719x_set_register_value(dev, reg_addr, new_reg_data, bytes);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_reset(struct ad719x_dev *dev)
{
	int ret;
	uint8_t register_word[5];

	memset(register_word, 0xFF, 5);

	ret = no_os_spi_write_and_read(dev->spi_desc, register_word, 5);
	if (ret != 0)
		return ret;

	// user must allow a period of 500 us
	no_os_udelay(500);

	return ret;
}

/***************************************************************************//**
 * @brief Sets device into the specified operating mode.
 *
 * @param dev        - The device structure.
 * @param opt_mode	 - Operating mode to be set.
 *		     Accepted values: AD719X_MODE_CONT
 *				      AD719X_MODE_SINGLE
 *				      AD719X_MODE_IDLE
 *				      AD719X_MODE_PWRDN
 *				      AD719X_MODE_CAL_INT_ZERO
 *				      AD719X_MODE_CAL_INT_FULL
 *				      AD719X_MODE_CAL_SYS_ZERO
 *				      AD719X_MODE_CAL_SYS_FULL
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_set_operating_mode(struct ad719x_dev *dev,
			      enum ad719x_adc_modes opt_mode)
{
	int ret;

	ret = ad719x_set_masked_register_value(dev, AD719X_REG_MODE,
					       AD719X_MODE_SEL(0x7), AD719X_MODE_SEL(opt_mode),
					       3);

	if (ret == 0) {
		/* Store the last settings regarding operating mode. */
		dev->operating_mode = opt_mode;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Waits for RDY pin to go low.
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_wait_rdy_go_low(struct ad719x_dev *dev)
{
	uint8_t wait = 1;
	uint16_t timeout = 0xFFFF;
	int ret;

	while (wait && (timeout > 0)) {
		ret = no_os_gpio_get_value(dev->gpio_miso, &wait);
		if (ret != 0)
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
 *                  Example: AD719X_CH_0 - AIN1(+) - AIN2(-);  (Pseudo = 0)
 *                           AD719X_CH_1 - AIN3(+) - AIN4(-);  (Pseudo = 0)
 *                           AD719X_TEMP - Temperature sensor
 *                           AD719X_SHORT - AIN2(+) - AIN2(-); (Pseudo = 0)
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_channels_select(struct ad719x_dev *dev,
			   uint16_t chn_mask)
{
	switch (dev->chip_id) {
	case AD7193:
		if (chn_mask > 0x3FF) {
			return -EINVAL;
		}
		break;
	case AD7194:
		if (chn_mask > 0x1FF) {
			return -EINVAL;
		}
		break;
	default:
		if (chn_mask > 0xFF) {
			return -EINVAL;
		}
	}

	if (dev->chip_id == AD7194)
		return ad719x_set_masked_register_value(dev, AD719X_REG_CONF,
							AD719X_CONF_CHAN(0x1FF), AD719X_CONF_CHAN(chn_mask),
							3);

	return ad719x_set_masked_register_value(dev, AD719X_REG_CONF,
						AD719X_CONF_CHAN(0x3FF), AD719X_CONF_CHAN(chn_mask),
						3);
}

/***************************************************************************//**
 * @brief Performs the given calibration to the specified channel.
 *
 * @param dev     - The device structure.
 * @param mode    - Calibration type.
 * @param channel - Channel to be calibrated.
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_calibrate(struct ad719x_dev *dev,
		     uint8_t mode, uint8_t channel)
{
	int ret;

	ret = ad719x_channels_select(dev, channel);
	if (ret != 0)
		return ret;

	ret = ad719x_set_masked_register_value(dev, AD719X_REG_MODE,
					       AD719X_MODE_SEL(0x7), AD719X_MODE_SEL(mode),
					       3);
	if (ret != 0)
		return ret;

	return ad719x_wait_rdy_go_low(dev);
}

/***************************************************************************//**
 * @brief Configures the input mode of the ADC
 *
 * @param dev   - The device structure.
 * @param mode  - Mode selection.
 *				  Example: 0 - Differential Analog Inputs
 *						   1 - Pseudo Differential Analog Inputs
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_config_input_mode(struct ad719x_dev *dev, uint8_t mode)
{
	int ret;

	if(dev->chip_id == AD7193 || dev->chip_id == AD7194) {
		ret = ad719x_set_masked_register_value(dev, AD719X_REG_CONF,
						       AD719X_CONF_PSEUDO, (AD719X_CONF_PSEUDO * mode),
						       3);
		if (ret == 0) {
			/* Store the last settings regarding input mode. */
			dev->input_mode = mode;
		}
		return ret;
	} else
		return -ENOTSUP;
}

/***************************************************************************//**
 * @brief Enables or disables the buffer on the ADC input channels
 *
 * @param dev      - The device structure.
 * @param buff_en  - Enable/disable option.
 *                   Example: 0 - Buffer disabled
 *                            1 - Buffer Enabled
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_buffer_select(struct ad719x_dev *dev, uint8_t buff_en)
{
	int ret;

	ret = ad719x_set_masked_register_value(dev, AD719X_REG_CONF,
					       AD719X_CONF_BUF, (AD719X_CONF_BUF * buff_en),
					       3);

	if (ret == 0) {
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
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_output_rate_select(struct ad719x_dev *dev,
			      uint16_t out_rate_code)
{
	int ret;

	ret = ad719x_set_masked_register_value(dev, AD719X_REG_MODE,
					       AD719X_MODE_RATE(0x3FF), AD719X_MODE_RATE(out_rate_code),
					       3);

	if (ret == 0) {
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
 * 		     Accepted values: AD719X_EXT_CRYSTAL_MCLK1_MCLK2
 *				      AD719X_EXT_CRYSTAL_MCLK2
 *				      AD719X_INT_CLK_4_92_MHZ_TRIST
 *				      AD719X_INT_CLK_4_92_MHZ
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_clock_select(struct ad719x_dev *dev,
			enum ad719x_adc_clock clk_select)
{
	int ret;

	ret = ad719x_set_masked_register_value(dev, AD719X_REG_MODE,
					       AD719X_MODE_CLKSRC(0x3), AD719X_MODE_CLKSRC(clk_select),
					       3);

	if (ret == 0) {
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
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_set_bridge_switch(struct ad719x_dev *dev, uint8_t bpdsw_select)
{
	int ret;

	if(dev->chip_id == AD7194) {
		return -ENOTSUP;
	}

	ret = ad719x_set_masked_register_value(dev, AD719X_REG_GPOCON,
					       AD719X_GPOCON_BPDSW, (AD719X_GPOCON_BPDSW * bpdsw_select),
					       1);

	if (ret == 0) {
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
 *@param gain      - Gain select bits. These bits are written by the user to select
 *                   the ADC input range.
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_range_setup(struct ad719x_dev *dev,
		       uint8_t polarity, enum ad719x_adc_gain gain)
{
	int ret;

	ret = ad719x_set_masked_register_value(dev, AD719X_REG_CONF,
					       (AD719X_CONF_UNIPOLAR | AD719X_CONF_GAIN(0x7)),
					       (polarity * AD719X_CONF_UNIPOLAR) | AD719X_CONF_GAIN(gain),
					       3);

	if (ret == 0) {
		/* Store the last settings regarding polarity and gain. */
		dev->current_polarity = polarity;
		dev->current_gain     = 1 << gain;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Returns the result of a single conversion.
 *
 * @param dev - The device structure.
 * @param reg_data - Buffer to store sampled register data
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_single_conversion(struct ad719x_dev *dev, uint32_t *reg_data)
{
	uint32_t command = 0x0;
	int ret;

	if (!reg_data)
		return -1;

	command = AD719X_MODE_SEL(AD719X_MODE_SINGLE) | AD719X_MODE_CLKSRC(
			  AD719X_INT_CLK_4_92_MHZ_TRIST) | AD719X_MODE_RATE(dev->data_rate_code);

	ret = ad719x_set_register_value(dev, AD719X_REG_MODE, command, 3);
	if (ret != 0)
		return ret;

	ret = ad719x_wait_rdy_go_low(dev);
	if (ret != 0)
		return ret;

	return ad719x_get_register_value(dev, AD719X_REG_DATA, 3, reg_data);
}

/***************************************************************************//**
 * @brief Returns the average of several conversion results.
 *
 * @param dev - The device structure.
 * @param sample_number - the number of samples
 * @param samples_avg - Average of the samples read
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_continuous_read_avg(struct ad719x_dev *dev,
			       uint8_t sample_number, uint32_t *samples_avg)
{
	uint32_t samples = 0;
	uint32_t command = 0;
	uint8_t count = 0;
	int ret;

	command = AD719X_MODE_SEL(AD719X_MODE_CONT) |
		  AD719X_MODE_CLKSRC(AD719X_INT_CLK_4_92_MHZ_TRIST) |
		  AD719X_MODE_RATE(dev->data_rate_code);

	ret = ad719x_set_register_value(dev, AD719X_REG_MODE, command, 3);
	if (ret != 0)
		return ret;

	for (count = 0; count < sample_number; count++) {
		ret = ad719x_wait_rdy_go_low(dev);
		if (ret != 0)
			return ret;

		ret = ad719x_get_register_value(dev, AD719X_REG_DATA, 3, &samples);
		if(ret != 0)
			return ret;

		*samples_avg += samples;
	}

	*samples_avg = *samples_avg / sample_number;

	return 0;
}

/***************************************************************************//**
 * @brief Read data from temperature sensor and converts it to Celsius degrees.
 *
 * @param dev - The device structure.
 * @param temp - Stores the temperature result.
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_temperature_read(struct ad719x_dev *dev, float *temp)
{
	uint32_t data_reg = 0;
	int ret;

	// Bipolar operation, 1 Gain.
	ret = ad719x_range_setup(dev, 0, AD719X_ADC_GAIN_1);
	if (ret != 0)
		return ret;

	if(dev->chip_id == AD7190 || dev->chip_id == AD7192 || dev->chip_id == AD7195) {
		ret = ad719x_channels_select(dev, AD719X_CH_MASK(AD719X_CH_2));
		if (ret != 0)
			return ret;
	} else {
		ret = ad719x_channels_select(dev, AD719X_CH_MASK(AD719X_CH_TEMP));
		if (ret != 0)
			return ret;
	}

	ret = ad719x_single_conversion(dev, &data_reg);
	if (ret != 0)
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
float ad719x_convert_to_volts(struct ad719x_dev *dev,
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

/***************************************************************************//**
 * @brief Control SYNC pin for synchronization of multiple devices.
 *
 * @param dev       - The device structure.
 * @param value     - Pin level to be written.
 * 						0 - LOW
 *						1 - HIGH
 *
 * @return 0 in case of success or negative error code.
*******************************************************************************/
int ad719x_sync_control(struct ad719x_dev *dev, uint8_t value)
{
	if(dev->sync_pin) {
		return no_os_gpio_set_value(dev->sync_pin, value);
	} else {
		return -ENOTSUP;
	}
}
