/***************************************************************************//**
*   @file   ad5686.c
*   @brief  Implementation of AD5686 Driver.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
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
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad5686.h"

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
static const ad5686_chip_info chip_info[] = {
	[ID_AD5684R] = {
		.resolution = 12,
		.communication = SPI,
	},
	[ID_AD5685R] = {
		.resolution = 14,
		.communication = SPI,
	},
	[ID_AD5686R] = {
		.resolution = 16,
		.communication = SPI,
	},
	[ID_AD5694R] = {
		.resolution = 12,
		.communication = I2C,
	},
	[ID_AD5695R] = {
		.resolution = 14,
		.communication = I2C,
	},
	[ID_AD5696R] = {
		.resolution = 16,
		.communication = I2C,
	}
};

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD5686 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - The result of the initialization procedure.
 *                  Example: -1 - I2C peripheral was not initialized or the
 *                                device is not present.
 *                            0 - I2C peripheral was initialized and the
 *                                device is present.
*******************************************************************************/
int32_t ad5686_init(ad5686_dev **device,
		    ad5686_init_param init_param)
{
	ad5686_dev *dev;
	int32_t     ret;

	dev = (ad5686_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->act_device = init_param.act_device;

	if (chip_info[dev->act_device].communication == SPI) {
		dev->spi_dev.type = init_param.spi_type;
		dev->spi_dev.id = init_param.spi_id;
		dev->spi_dev.max_speed_hz = init_param.spi_max_speed_hz;
		dev->spi_dev.mode = init_param.spi_mode;
		dev->spi_dev.chip_select = init_param.spi_chip_select;
		ret = spi_init(&dev->spi_dev);
	} else {
		dev->i2c_dev.type = init_param.i2c_type;
		dev->i2c_dev.id = init_param.i2c_id;
		dev->i2c_dev.max_speed_hz = init_param.i2c_max_speed_hz;
		dev->i2c_dev.slave_address = init_param.i2c_slave_address;
		ret = i2c_init(&dev->i2c_dev);
	}


	/* GPIO */
	dev->gpio_dev.id = init_param.gpio_id;
	dev->gpio_dev.type = init_param.gpio_type;
	ret |= gpio_init(&dev->gpio_dev);

	dev->gpio_reset = init_param.gpio_reset;
	dev->gpio_ldac = init_param.gpio_ldac;

	if (dev->gpio_ldac >= 0) {
		ret |= gpio_set_direction(&dev->gpio_dev,
					  dev->gpio_ldac,
					  GPIO_OUT);
		ret |= gpio_set_value(&dev->gpio_dev,
				      dev->gpio_ldac,
				      GPIO_LOW);
	}

	if (dev->gpio_reset >= 0) {
		ret |= gpio_set_direction(&dev->gpio_dev,
					  dev->gpio_reset,
					  GPIO_OUT);
		ret |= gpio_set_value(&dev->gpio_dev,
				      dev->gpio_reset,
				      GPIO_HIGH);
	}

	*device = dev;

	return ret;
}

/**************************************************************************//**
 * @brief Write to input shift register.
 *
 * @param dev     - The device structure.
 * @param command - Command control bits.
 * @param data    - Data to be written in input register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
uint16_t ad5686_set_shift_reg(ad5686_dev *dev,
			      uint8_t command,
			      uint8_t address,
			      uint16_t data)
{
	uint8_t data_buff[PKT_LENGTH] = {0, 0, 0};
	uint16_t read_back_data = 0;

	data_buff[0] = ((command & CMD_MASK) << CMD_OFFSET) | \
		       (address & ADDR_MASK);
	data_buff[1] = (data & MSB_MASK) >> MSB_OFFSET;
	data_buff[2] = (data & LSB_MASK);

	if(chip_info[dev->act_device].communication == SPI) {
		spi_write_and_read(&dev->spi_dev, data_buff, PKT_LENGTH);
		read_back_data = (data_buff[1] << MSB_OFFSET) | data_buff[2];
	} else
		i2c_write(&dev->i2c_dev, data_buff, PKT_LENGTH, 1);

	return read_back_data;
}

/**************************************************************************//**
 * @brief Write to Input Register n (dependent on LDAC)
 *
 * @param dev      - The device structure.
 * @param address  - The chosen channel to write to.
 *                    Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                              channel A and C
 * @param data - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5686_write_register(ad5686_dev *dev,
			   uint8_t address,
			   uint16_t data)
{
	uint8_t data_offset = MAX_RESOLUTION - \
			      chip_info[dev->act_device].resolution;

	ad5686_set_shift_reg(dev, AD5686_CTRL_WRITE, address,
			     data << data_offset);
}

/**************************************************************************//**
 * @brief Update DAC Register n with contents of Input Register n
 *
 * @param dev     - The device structure.
 * @param address - The chosen channel to write to.
 *                  Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                            channel A and C
 *
 * @return None.
******************************************************************************/
void ad5686_update_register(ad5686_dev *dev,
			    uint8_t address)
{
	ad5686_set_shift_reg(dev, AD5686_CTRL_UPDATE, address, 0);
}

/**************************************************************************//**
 * @brief Write to and update DAC channel n
 *
 * @param dev     - The device structure.
 * @param address - The chosen channel to write to.
 *                  Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                            channel A and C
 * @param data    - Desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5686_write_update_register(ad5686_dev *dev,
				  uint8_t address,
				  uint16_t data)
{
	uint8_t data_offset = MAX_RESOLUTION - \
			      chip_info[dev->act_device].resolution;

	ad5686_set_shift_reg(dev, AD5686_CTRL_WRITEUPDATE, address,
			     data << data_offset);
}

/**************************************************************************//**
 * @brief Read back Input Register n
 *
 * @param dev     - The device structure.
 * @param address - The channel which will be read back. Note: only one
 *                  channel should be selected, if there will be selected
 *                  more than one channel, the channel A will be read back
 *                  by default
 *                  Example: 'AD5686_CH_C' will read back the channel C
 *
 * @return None.
******************************************************************************/
uint16_t ad5686_read_back_register(ad5686_dev *dev,
				   uint8_t address)
{

	uint16_t read_back_data = 0;
	uint16_t offset = MAX_RESOLUTION - \
			  chip_info[dev->act_device].resolution;

	if(chip_info[dev->act_device].communication == SPI) {
		ad5686_set_shift_reg(dev, AD5686_CTRL_RB_REG, address, 0);
		read_back_data = ad5686_set_shift_reg(dev, AD5686_CTRL_NOP, 0, 0);
		read_back_data >>= offset;
	}

	return read_back_data;
}

/**************************************************************************//**
 * @brief Write to and update DAC channel n
 *
 * @param dev     - The device structure.
 * @param address - The chosen channel to write to.
 *                  Example : 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                             channel A and C
 * @param mode    - Power-down operation modes.
 *                  Accepted values:
 *                  'AD5686_PWRM_NORMAL' - Normal Mode
 *                  'AD5686_PWRM_1K' - Power-down mode 1kOhm to GND
 *                  'AD5686_PWRM_100K' - Power-down mode 100kOhm to GND
 *                  'AD5686_PWRM_THREESTATE' - Three-State
 *
 * @return None.
******************************************************************************/
void ad5686_power_mode(ad5686_dev *dev,
		       uint8_t address,
		       uint8_t mode)
{
	uint16_t data = 0;

	data |= (address & AD5686_CH_A) ?
		(mode << AD5686_PWRM_CHA_OFFSET) : 0x0;
	data |= (address & AD5686_CH_B) ?
		(mode << AD5686_PWRM_CHB_OFFSET) : 0x0;
	data |= (address & AD5686_CH_C) ?
		(mode << AD5686_PWRM_CHC_OFFSET) : 0x0;
	data |= (address & AD5686_CH_D) ?
		(mode << AD5686_PWRM_CHD_OFFSET) : 0x0;

	ad5686_set_shift_reg(dev, AD5686_CTRL_PWR, address, data);
}

/**************************************************************************//**
 * @brief Set hardware LDAC mask register
 *
 * @param dev       - The device structure.
 * @param ldac_mask - In case of which channel ignore transitions on the LDAC
 *                    pin.
 *                    Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                              channel A and C
 *
 * @return None.
******************************************************************************/
void ad5686_ldac_mask(ad5686_dev *dev,
		      uint8_t ldac_mask)
{
	ad5686_set_shift_reg(dev, AD5686_CTRL_LDAC_MASK, 0, ldac_mask);
}

/**************************************************************************//**
 * @brief Software reset (power-on reset)
 *
 * @param dev - The device structure.
 *
 * @return None.
******************************************************************************/
void ad5686_software_reset(ad5686_dev *dev)
{
	ad5686_set_shift_reg(dev, AD5686_CTRL_SWRESET, 0, 0);
}


/**************************************************************************//**
 * @brief Write to Internal reference setup register
 *
 * @param dev   - The device structure.
 * @param value - The internal reference register value
 *                Example : 'AD5686_INTREF_EN' - enable internal reference
 *                            'AD5686_INTREF_DIS' - disable internal reference
 *
 * @return None.
******************************************************************************/
void ad5686_internal_reference(ad5686_dev *dev,
			       uint8_t value)
{
	ad5686_set_shift_reg(dev, AD5686_CTRL_IREF_REG, 0, value);
}

/**************************************************************************//**
 * @brief Set up DCEN register (daisy-chain enable)
 *
 * @param dev   - The device structure.
 * @param value - Enable or disable daisy-chain mode
 *                Example : 'AD5686_DC_EN' - daisy-chain enable
 *                          'AD5686_DC_DIS' - daisy-chain disable
 *
 * @return None.
******************************************************************************/
void ad5686_daisy_chain_en(ad5686_dev *dev,
			   uint8_t value)
{
	ad5686_set_shift_reg(dev, AD5686_CTRL_DCEN, 0, value);
}

/**************************************************************************//**
 * @brief Set up readback register (readback enable)
 *
 * @param dev   - The device structure.
 * @param value - Enable or disable daisy-chain mode
 *                Example : 'AD5686_RB_EN' - daisy-chain enable
 *                          'AD5686_RB_DIS' - daisy-chain disable
 *
 * @return None.
******************************************************************************/
void ad5686_read_back_en(ad5686_dev *dev,
			 uint8_t value)
{
	ad5686_set_shift_reg(dev, AD5686_CTRL_RB_REG, 0, value);
}
