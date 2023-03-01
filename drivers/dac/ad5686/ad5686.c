/***************************************************************************//**
*   @file   ad5686.c
*   @brief  Implementation of AD5686 Driver.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*******************************************************************************
* Copyright 2013, 2020(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include "ad5686.h"
#include "no_os_alloc.h"

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
static const uint32_t ad5683_channel_addr [] = {
	[AD5686_CH_0] = 0,
};

static const uint32_t ad5689_channel_addr[] = {
	[AD5686_CH_0] = 1,
	[AD5686_CH_1] = 8,
};

static const uint32_t ad5686_channel_addr[] = {
	[AD5686_CH_0] = 1,
	[AD5686_CH_1] = 2,
	[AD5686_CH_2] = 4,
	[AD5686_CH_3] = 8,
};

static const uint32_t ad5676_channel_addr[] = {
	[AD5686_CH_0] = 0,
	[AD5686_CH_1] = 1,
	[AD5686_CH_2] = 2,
	[AD5686_CH_3] = 3,
	[AD5686_CH_4] = 4,
	[AD5686_CH_5] = 5,
	[AD5686_CH_6] = 6,
	[AD5686_CH_7] = 7,
};

static const uint32_t ad5679_channel_addr[] = {
	[AD5686_CH_0] = 0,
	[AD5686_CH_1] = 1,
	[AD5686_CH_2] = 2,
	[AD5686_CH_3] = 3,
	[AD5686_CH_4] = 4,
	[AD5686_CH_5] = 5,
	[AD5686_CH_6] = 6,
	[AD5686_CH_7] = 7,
	[AD5686_CH_8] = 8,
	[AD5686_CH_9] = 9,
	[AD5686_CH_10] = 10,
	[AD5686_CH_11] = 11,
	[AD5686_CH_12] = 12,
	[AD5686_CH_13] = 13,
	[AD5686_CH_14] = 14,
	[AD5686_CH_15] = 15,
};

static const struct ad5686_chip_info chip_info[] = {
	[ID_AD5671R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5676_channel_addr,
	},
	[ID_AD5672R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5676_channel_addr,
	},
	[ID_AD5673R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5679_channel_addr,
	},
	[ID_AD5674] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5679_channel_addr,
	},
	[ID_AD5674R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5679_channel_addr,
	},
	[ID_AD5675R] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5676_channel_addr,
	},
	[ID_AD5676] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5676_channel_addr,
	},
	[ID_AD5676R] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5676_channel_addr,
	},
	[ID_AD5677R] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5679_channel_addr,
	},
	[ID_AD5679] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5679_channel_addr,
	},
	[ID_AD5679R] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5679_channel_addr,
	},
	[ID_AD5684R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5685R] = {
		.resolution = 14,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5686] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5686R] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5687] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5689_channel_addr,
	},
	[ID_AD5687R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5689_channel_addr,
	},
	[ID_AD5689] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5689_channel_addr,
	},
	[ID_AD5689R] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5689_channel_addr,
	},
	[ID_AD5697R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5689_channel_addr,
	},
	[ID_AD5694] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5694R] = {
		.resolution = 12,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5695R] = {
		.resolution = 14,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5696] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5696R] = {
		.resolution = 16,
		.register_map = AD5686_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5686_channel_addr,
	},
	[ID_AD5681R] = {
		.resolution = 12,
		.register_map = AD5683_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5683_channel_addr,
	},
	[ID_AD5682R] = {
		.resolution = 14,
		.register_map = AD5683_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5683_channel_addr,
	},
	[ID_AD5683R] = {
		.resolution = 16,
		.register_map = AD5683_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5683_channel_addr,
	},
	[ID_AD5683] = {
		.resolution = 16,
		.register_map = AD5683_REG_MAP,
		.communication = SPI,
		.channel_addr = ad5683_channel_addr,
	},
	[ID_AD5691R] = {
		.resolution = 12,
		.register_map = AD5683_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5683_channel_addr,
	},
	[ID_AD5692R] = {
		.resolution = 14,
		.register_map = AD5683_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5683_channel_addr,
	},
	[ID_AD5693R] = {
		.resolution = 16,
		.register_map = AD5683_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5683_channel_addr,
	},
	[ID_AD5693] = {
		.resolution = 16,
		.register_map = AD5683_REG_MAP,
		.communication = I2C,
		.channel_addr = ad5683_channel_addr,
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
 * @return ret - The result of the initialization procedure.
 *               Example: -1 - I2C peripheral was not initialized or the
 *                             device is not present.
 *                         0 - I2C peripheral was initialized and the
 *                             device is present.
*******************************************************************************/
int32_t ad5686_init(struct ad5686_dev **device,
		    struct ad5686_init_param init_param)
{
	struct ad5686_dev *dev;
	int32_t ret;

	dev = (struct ad5686_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->act_device = init_param.act_device;
	dev->power_down_mask = 0;
	dev->ldac_mask = 0;

	if (chip_info[dev->act_device].communication == SPI)
		ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	else
		ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);


	/* GPIO */
	ret |= no_os_gpio_get(&dev->gpio_reset, &init_param.gpio_reset);
	ret |= no_os_gpio_get(&dev->gpio_ldac, &init_param.gpio_ldac);
	ret |= no_os_gpio_get(&dev->gpio_gain, &init_param.gpio_gain);

	if (dev->gpio_ldac)
		ret |= no_os_gpio_direction_output(dev->gpio_ldac, NO_OS_GPIO_LOW);

	if (dev->gpio_reset)
		ret |= no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);

	if (dev->gpio_gain)
		ret |= no_os_gpio_direction_output(dev->gpio_gain, NO_OS_GPIO_LOW);
	*device = dev;

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5686_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad5686_remove(struct ad5686_dev *dev)
{
	int32_t ret;

	if (chip_info[dev->act_device].communication == SPI)
		ret = no_os_spi_remove(dev->spi_desc);
	else
		ret = no_os_i2c_remove(dev->i2c_desc);

	if (dev->gpio_ldac)
		ret |= no_os_gpio_remove(dev->gpio_ldac);

	if (dev->gpio_reset)
		ret |= no_os_gpio_remove(dev->gpio_reset);

	if (dev->gpio_gain)
		ret |= no_os_gpio_remove(dev->gpio_gain);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Write to input shift register.
 *
 * @param dev     - The device structure.
 * @param command - Command control bits.
 * @param address - The address bits.
 * @param data    - Data to be written in input register.
 *
 * @return  read_back_data - value read from register.
******************************************************************************/
uint16_t ad5686_set_shift_reg(struct ad5686_dev *dev,
			      uint8_t command,
			      uint8_t address,
			      uint16_t data)
{
	uint8_t data_buff [ PKT_LENGTH ] = {0, 0, 0};
	uint16_t read_back_data = 0;

	if(chip_info[dev->act_device].register_map == AD5686_REG_MAP) {
		data_buff[0] = ((command & AD5686_CMD_MASK) << CMD_OFFSET) | \
			       (address & ADDR_MASK);
		data_buff[1] = (data & AD5686_MSB_MASK) >> AD5686_MSB_OFFSET;
		data_buff[2] = (data & AD5686_LSB_MASK);
	} else {
		data_buff[0] = ((command & AD5683_CMD_MASK) << CMD_OFFSET) |
			       ((data >> AD5683_MSB_OFFSET) & AD5683_MSB_MASK);
		data_buff[1] = (data >> AD5683_MIDB_OFFSET) & AD5683_MIDB_MASK;
		data_buff[2] = (data & AD5683_LSB_MASK) << AD5683_LSB_OFFSET;
	}

	if(chip_info[dev->act_device].communication == SPI) {
		no_os_spi_write_and_read(dev->spi_desc, data_buff, PKT_LENGTH);
		if(chip_info[dev->act_device].register_map == AD5686_REG_MAP)
			read_back_data = (data_buff[1] << AD5686_MSB_OFFSET) | data_buff[2];
		else
			read_back_data = (data_buff[0] & AD5683_CMD_MASK) << AD5683_MSB_OFFSET |
					 data_buff[1] << AD5683_MIDB_OFFSET |
					 data_buff[2] >> AD5683_LSB_OFFSET;
	} else
		no_os_i2c_write(dev->i2c_desc, data_buff, PKT_LENGTH, 1);

	return read_back_data;
}

/**************************************************************************//**
 * @brief Write to Input Register n (dependent on LDAC)
 *
 * @param dev      - The device structure.
 * @param channel  - The chosen channel to write to.
 *                    Accepted values:	AD5686_CH_0
 *					AD5686_CH_1
 *					AD5686_CH_2
 *					AD5686_CH_3
 *					AD5686_CH_4
 * 					AD5686_CH_5
 *					AD5686_CH_6
 * 					AD5686_CH_7
 *					AD5686_CH_8
 *                              	AD5686_CH_9
 *					AD5686_CH_10
 *					AD5686_CH_11
 *					AD5686_CH_12
 *					AD5686_CH_13
 *					AD5686_CH_14
 *					AD5686_CH_15
 * @param data - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5686_write_register(struct ad5686_dev *dev,
			   enum ad5686_dac_channels channel,
			   uint16_t data)
{
	uint8_t data_offset = MAX_RESOLUTION - \
			      chip_info[dev->act_device].resolution;
	uint8_t address = chip_info[dev->act_device].channel_addr[channel];

	ad5686_set_shift_reg(dev, AD5686_CTRL_WRITE, address,
			     data << data_offset);
}

/**************************************************************************//**
 * @brief Update DAC Register n with contents of Input Register n
 *
 * @param dev     - The device structure.
 * @param channel  - The chosen channel to write to.
 *                    Accepted values:	AD5686_CH_0
 *					AD5686_CH_1
 *					AD5686_CH_2
 *					AD5686_CH_3
 *					AD5686_CH_4
 * 					AD5686_CH_5
 *					AD5686_CH_6
 * 					AD5686_CH_7
 *					AD5686_CH_8
 *                              	AD5686_CH_9
 *					AD5686_CH_10
 *					AD5686_CH_11
 *					AD5686_CH_12
 *					AD5686_CH_13
 *					AD5686_CH_14
 *					AD5686_CH_15
  * @return None.
******************************************************************************/
void ad5686_update_register(struct ad5686_dev *dev,
			    enum ad5686_dac_channels channel)
{
	uint8_t address = chip_info[dev->act_device].channel_addr[channel];

	ad5686_set_shift_reg(dev, AD5686_CTRL_UPDATE, address, 0);
}

/**************************************************************************//**
 * @brief Write to and update DAC channel n
 *
 * @param dev     - The device structure.
 * @param channel  - The chosen channel to write to.
 *                    Accepted values:	AD5686_CH_0
 *					AD5686_CH_1
 *					AD5686_CH_2
 *					AD5686_CH_3
 *					AD5686_CH_4
 * 					AD5686_CH_5
 *					AD5686_CH_6
 * 					AD5686_CH_7
 *					AD5686_CH_8
 *                              	AD5686_CH_9
 *					AD5686_CH_10
 *					AD5686_CH_11
 *					AD5686_CH_12
 *					AD5686_CH_13
 *					AD5686_CH_14
 *					AD5686_CH_15
 * @param data    - Desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5686_write_update_register(struct ad5686_dev *dev,
				  enum ad5686_dac_channels channel,
				  uint16_t data)
{
	uint8_t data_offset = MAX_RESOLUTION - \
			      chip_info[dev->act_device].resolution;
	uint8_t address = chip_info[dev->act_device].channel_addr[channel];

	ad5686_set_shift_reg(dev, AD5686_CTRL_WRITEUPDATE, address,
			     data << data_offset);
}

/**************************************************************************//**
 * @brief Read back Input Register n
 *
 * @param dev     - The device structure.
 * @param channel - The channel which will be read back. Note: only one
 *                  channel should be selected, if there will be selected
 *                  more than one channel, the channel A will be read back
 *                  by default
 *                    Accepted values:	AD5686_CH_0
 *					AD5686_CH_1
 *					AD5686_CH_2
 *					AD5686_CH_3
 *					AD5686_CH_4
 * 					AD5686_CH_5
 *					AD5686_CH_6
 * 					AD5686_CH_7
 *					AD5686_CH_8
 *                              	AD5686_CH_9
 *					AD5686_CH_10
 *					AD5686_CH_11
 *					AD5686_CH_12
 *					AD5686_CH_13
 *					AD5686_CH_14
 *					AD5686_CH_15
 * @return read_back_data - value read from register.
******************************************************************************/
uint16_t ad5686_read_back_register(struct ad5686_dev *dev,
				   enum ad5686_dac_channels channel)
{

	uint16_t read_back_data = 0;
	uint16_t offset = MAX_RESOLUTION - \
			  chip_info[dev->act_device].resolution;
	uint8_t address = chip_info[dev->act_device].channel_addr[channel];
	uint8_t rb_data_i2c[3] = { 0 };

	if(chip_info[dev->act_device].communication == SPI) {
		ad5686_set_shift_reg(dev, AD5686_CTRL_RB_REG, address, 0);
		read_back_data = ad5686_set_shift_reg(dev, AD5686_CTRL_NOP, 0,
						      0);
		read_back_data >>= offset;
	} else {
		if (chip_info[dev->act_device].register_map == AD5683_REG_MAP)
			rb_data_i2c[0] = (AD5683_CTRL_RB_REG << CMD_OFFSET) |
					 address;
		else
			rb_data_i2c[0] = (AD5686_CTRL_RB_REG << CMD_OFFSET) |
					 address;

		no_os_i2c_write(dev->i2c_desc, rb_data_i2c, 3, 0);
		no_os_i2c_read(dev->i2c_desc, rb_data_i2c, 2, 1);
		read_back_data = (rb_data_i2c[0] << 8) | rb_data_i2c[1];
	}

	return read_back_data;
}

/**************************************************************************//**
 * @brief Set Power-down mode for DAC channel n
 *
 * @param dev     - The device structure.
 * @param channel  - The chosen channel to change the power-down mode.
 *                    Accepted values:	AD5686_CH_0
 *					AD5686_CH_1
 *					AD5686_CH_2
 *					AD5686_CH_3
 *					AD5686_CH_4
 * 					AD5686_CH_5
 *					AD5686_CH_6
 * 					AD5686_CH_7
 *					AD5686_CH_8
 *                              	AD5686_CH_9
 *					AD5686_CH_10
 *					AD5686_CH_11
 *					AD5686_CH_12
 *					AD5686_CH_13
 *					AD5686_CH_14
 *					AD5686_CH_15
 * @param mode    - Power-down operation modes.
 *                  Accepted values:
 *                  'AD5686_PWRM_NORMAL' - Normal Mode
 *                  'AD5686_PWRM_1K' - Power-down mode 1kOhm to GND
 *                  'AD5686_PWRM_100K' - Power-down mode 100kOhm to GND
 *                  'AD5686_PWRM_THREESTATE' - Three-State
 *                  'AD5686_PWRM_100K' is not available for AD5674R/AD5679R.
 *
 * @return None.
******************************************************************************/
void ad5686_power_mode(struct ad5686_dev *dev,
		       enum ad5686_dac_channels channel,
		       uint8_t mode)
{
	uint8_t address = chip_info[dev->act_device].channel_addr[channel];

	if(chip_info[dev->act_device].register_map == AD5686_REG_MAP) {
		/* AD5674R/AD5679R have 16 channels and 2 powerdown registers */
		if (channel > AD5686_CH_7)
			channel -= AD5686_CH_7 + 1;
		dev->power_down_mask &= ~(0x3 << (channel *2));
		dev->power_down_mask |= (mode << (channel *2));
		ad5686_set_shift_reg(dev, AD5686_CTRL_PWR, address,
				     dev->power_down_mask);
	} else {
		ad5686_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG, address,
				     AD5683_CTRL_PWRM(mode));
	}

}

/**************************************************************************//**
 * @brief Set hardware LDAC mask register
 *
 * @param dev     - The device structure.
 * @param channel - In case of which channel ignore transitions on the LDAC
 *                    pin.
 *                    Accepted values:	AD5686_CH_0
 *					AD5686_CH_1
 *					AD5686_CH_2
 *					AD5686_CH_3
 *					AD5686_CH_4
 * 					AD5686_CH_5
 *					AD5686_CH_6
 * 					AD5686_CH_7
 *					AD5686_CH_8
 *                              	AD5686_CH_9
 *					AD5686_CH_10
 *					AD5686_CH_11
 *					AD5686_CH_12
 *					AD5686_CH_13
 *					AD5686_CH_14
 *					AD5686_CH_15
 * @param enable - Enable/disable channel.
 * @return None.
******************************************************************************/
void ad5686_ldac_mask(struct ad5686_dev *dev,
		      enum ad5686_dac_channels channel,
		      uint8_t enable)
{
	if(chip_info[dev->act_device].register_map == AD5686_REG_MAP) {
		dev->ldac_mask &= ~(0x1 << channel);
		dev->ldac_mask |= (enable << channel);
		ad5686_set_shift_reg(dev, AD5686_CTRL_LDAC_MASK, 0, dev->ldac_mask);
	}
}

/**************************************************************************//**
 * @brief Software reset (power-on reset)
 *
 * @param dev - The device structure.
 *
 * @return None.
******************************************************************************/
void ad5686_software_reset(struct ad5686_dev *dev)
{
	if(chip_info[dev->act_device].register_map == AD5686_REG_MAP)
		ad5686_set_shift_reg(dev, AD5686_CTRL_SWRESET, 0, 0);
	else
		ad5686_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG, 0, AD5683_SW_RESET);
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
void ad5686_internal_reference(struct ad5686_dev *dev,
			       uint8_t value)
{
	if(chip_info[dev->act_device].register_map == AD5686_REG_MAP)
		ad5686_set_shift_reg(dev, AD5686_CTRL_IREF_REG, 0, value);
	else
		ad5686_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG, 0,
				     AD5683_CTRL_INT_REF(value));
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
void ad5686_daisy_chain_en(struct ad5686_dev *dev,
			   uint8_t value)
{
	if(chip_info[dev->act_device].register_map == AD5686_REG_MAP)
		ad5686_set_shift_reg(dev, AD5686_CTRL_DCEN, 0, value);
	else
		ad5686_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG, 0, AD5683_CTRL_DCEN(value));
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
void ad5686_read_back_en(struct ad5686_dev *dev,
			 uint8_t value)
{
	if(chip_info[dev->act_device].register_map == AD5686_REG_MAP)
		ad5686_set_shift_reg(dev, AD5686_CTRL_RB_REG, 0, value);
}

/**************************************************************************//**
 * @brief Set Gain mode
 *
 * @param dev   - The device structure.
 * @param value - Gain modes.
 *                Accepted values:
 *                Example : 'AD5683_GB_VREF' - 0V to VREF
 *                          'AD5683_GB_2VREF' - 0V to 2xVREF
 *
 * @return None.
******************************************************************************/
int32_t ad5686_gain_mode(struct ad5686_dev *dev, uint8_t value)
{
	if(chip_info[dev->act_device].register_map == AD5683_REG_MAP)
		return ad5686_set_shift_reg(dev, AD5683_CMD_WR_CTRL_REG, 0,
					    AD5683_CTRL_GM(value));
	return -1;
}
