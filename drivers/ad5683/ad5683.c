/***************************************************************************//**
 *   @file   ad5683.c
 *   @brief  Implementation of AD5683 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include "ad5683.h"
#include "platform_drivers.h"
#include <stdint.h>
#include <stdlib.h>

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/

static const struct ad5683_chip_info chip_info[] = {
	[ID_AD5681R] = {
		.resolution = 12,
		.communication = SPI,
	},
	[ID_AD5682R] = {
		.resolution = 14,
		.communication = SPI,
	},
	[ID_AD5683R] = {
		.resolution = 16,
		.communication = SPI,
	},
	[ID_AD5683] = {
		.resolution = 16,
		.communication = SPI,
	},
	[ID_AD5691R] = {
		.resolution = 12,
		.communication = I2C,
	},
	[ID_AD5692R] = {
		.resolution = 14,
		.communication = I2C,
	},
	[ID_AD5693R] = {
		.resolution = 16,
		.communication = I2C,
	},
	[ID_AD5693] = {
		.resolution = 16,
		.communication = I2C,
	}
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD5683 Board.
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
*/
int32_t ad5683_init(struct ad5683_dev **device,
		    struct ad5683_init_param init_param)
{
	struct ad5683_dev *dev;
	int32_t ret;

	dev = (struct ad5683_dev *)malloc(sizeof(*dev));
	if(!dev)
		return FAILURE;

	if (chip_info[dev->act_device].communication == SPI)
		ret = spi_init(&dev->spi_desc, &init_param.spi_init);
	else
		ret = i2c_init(&dev->i2c_desc, &init_param.i2c_init);

	/* GPIO */
	ret |= gpio_get(&dev->gpio_reset, init_param.gpio_reset);
	ret |= gpio_get(&dev->gpio_ldac, init_param.gpio_ldac);

	if (dev->gpio_ldac)
		ret |= gpio_direction_output(dev->gpio_ldac, GPIO_HIGH);

	if (dev->gpio_reset)
		ret |= gpio_direction_output(dev->gpio_reset, GPIO_HIGH);

	*device = dev;

	return ret;
}

/**
 * @brief Free the resources allocated by ad5683_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*/
int32_t ad5683_remove(struct ad5683_dev *dev)
{
	int32_t ret;

	if (chip_info[dev->act_device].communication == SPI)
		ret = spi_remove(dev->spi_desc);
	else
		ret = i2c_remove(dev->i2c_desc);

	if (dev->gpio_ldac)
		ret |= gpio_remove(dev->gpio_ldac);

	if (dev->gpio_reset)
		ret |= gpio_remove(dev->gpio_reset);

	free(dev);

	return ret;
}

/**
 * @brief Write to input shift register.
 *
 * @param dev     - The device structure.
 * @param command - Command control bits.
 * @param data    - Data to be written in input register.
 *
 * @return  read_back_data - value read from register.
*/
uint16_t ad5683_set_shift_reg(struct ad5683_dev *dev, uint8_t command,
			      uint16_t data)
{
	int32_t ret;
	uint8_t data_buff [PKT_LENGTH] = {0, 0, 0};

	data_buff[0] = ((command & CMD_MASK) << CMD_OFFSET) |
		       ((data >> MSB_OFFSET) & MSB_MASK);
	data_buff[1] = (data >> MIDB_OFFSET) & MIDB_MASK;
	data_buff[2] = (data & LSB_MASK)  << LSB_OFFSET;

	if(chip_info[dev->act_device].communication == SPI)
		ret = spi_write_and_read(dev->spi_desc, data_buff, PKT_LENGTH);
	else
		ret = i2c_write(dev->i2c_desc, data_buff, PKT_LENGTH, 1);

	return ret;
}

/**
 * @brief Write to Input Register
 *
 * @param dev      - The device structure.
 * @param data - desired value to be written in register.
 *
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_write_register(struct ad5683_dev *dev, uint16_t data)
{
	uint8_t data_offset = MAX_RESOLUTION - chip_info[dev->act_device].resolution;

	return ad5683_set_shift_reg(dev, AD5683_CMD_WR_IN_REG, data << data_offset);
}

/**
 * @brief Update DAC Register with contents of Input Register
 *
 * @param dev     - The device structure.
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_update_register(struct ad5683_dev *dev)
{
	return ad5683_set_shift_reg(dev, AD5683_CMD_UPDATE_DAC, 0);
}

/**
 * @brief Write DAC and Input Register
 *
 * @param dev     - The device structure.
 * @param data    - Desired value to be written in register.
 *
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_write_dac_input_register(struct ad5683_dev *dev, uint16_t data)
{
	uint8_t data_offset = MAX_RESOLUTION - chip_info[dev->act_device].resolution;

	return ad5683_set_shift_reg(dev, AD5683_CMD_WR_DAC_REG, data << data_offset);
}

/**
 * @brief Read back Input Register
 *
 * @param dev     - The device structure.
 *
 * @return read_back_data - value read from register.
*/
uint32_t ad5683_read_back_register(struct ad5683_dev *dev)
{

	uint32_t read_back_data = 0;
	uint32_t offset = MAX_RESOLUTION - chip_info[dev->act_device].resolution;

	if(chip_info[dev->act_device].communication == SPI) {
		ad5683_set_shift_reg(dev, AD5683_CMD_RB_IN_REG, 0);
		read_back_data = ad5683_set_shift_reg(dev, AD5683_CMD_NOP, 0);
		read_back_data = (RB_DATA_MASK & read_back_data) >> (offset + LSB_OFFSET);
	} else
		return FAILURE;

	return read_back_data;
}

/**
 * @brief Set Power-down mode.
 *
 * @param dev     - The device structure.
 * @param mode    - Power-down operation modes.
 *                  Accepted values:
 *                  'AD5683_PWRM_NORMAL' - Normal Mode
 *                  'AD5683_PWRM_1K' - Power-down mode 1kOhm to GND
 *                  'AD5683_PWRM_100K' - Power-down mode 100kOhm to GND
 *                  'AD5683_PWRM_THREESTATE' - Three-State
 *
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_power_mode(struct ad5683_dev *dev, uint8_t mode)
{
	return ad5683_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG,
				    AD5683_CTRL_PWRM(mode));
}

/**
 * @brief Software reset
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_software_reset(struct ad5683_dev *dev)
{
	return ad5683_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG,
				    AD5683_SW_RESET(AD5683_SW_RESET_EN));
}

/**
 * @brief Set Internal reference
 *
 * @param dev   - The device structure.
 * @param value - The internal reference register value
 *                Example : 'AD5683_INTREF_EN' - enable internal reference
 *                          'AD5683_INTREF_DIS' - disable internal reference
 *
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_internal_reference(struct ad5683_dev *dev, uint8_t value)
{
	return ad5683_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG,
				    AD5683_CTRL_INT_REF(value));
}

/**
 * @brief Set DCEN (daisy-chain enable)
 *
 * @param dev   - The device structure.
 * @param value - Enable or disable daisy-chain mode
 *                Example : 'AD5683_DC_EN' - daisy-chain enable
 *                          'AD5683_DC_DIS' - daisy-chain disable
 *
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_daisy_chain_en(struct ad5683_dev *dev, uint8_t value)
{
	return ad5683_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG,
				    AD5683_CTRL_DCEN(value));
}

/**
 * @brief Set Gain mode
 *
 * @param dev   - The device structure.
 * @param value - Gain modes.
 * 				  Accepted values:
 *                Example : 'AD5683_GB_VREF' - 0V to VREF
 *                          'AD5683_GB_2VREF' - 0V to 2xVREF
 *
 * @return ret - The result of the write procedure.
*/
int32_t ad5683_gain_mode(struct ad5683_dev *dev, uint8_t value)
{
	return ad5683_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG,
				    AD5683_CTRL_GM(value));
}

