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
int32_t AD5686_Init(ad5686_dev **device,
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
 * @param   command  - command control bits.
 *          data     - data to be written in input register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
unsigned short AD5686_SetShiftReg(ad5686_dev *dev,
				  unsigned char command,
                                  unsigned char address,
                                  unsigned short data)
{
    unsigned char dataBuff[PKT_LENGTH] = {0, 0, 0};
    unsigned short readBackData = 0;

    dataBuff[0] = ((command & CMD_MASK) << CMD_OFFSET) | \
                  (address & ADDR_MASK);
    dataBuff[1] = (data & MSB_MASK) >> MSB_OFFSET;
    dataBuff[2] = (data & LSB_MASK);

    if(chip_info[dev->act_device].communication == SPI)
    {
        spi_write_and_read(&dev->spi_dev, dataBuff, PKT_LENGTH);
        readBackData = (dataBuff[1] << MSB_OFFSET) | dataBuff[2];
    }
    else
    {
        i2c_write(&dev->i2c_dev, dataBuff, PKT_LENGTH, 1);
    }
return readBackData;
}

/**************************************************************************//**
 * @brief Write to Input Register n (dependent on LDAC)
 *
 * @param   address  - the chosen channel to write to.
 *                      Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *          data - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5686_WriteRegister(ad5686_dev *dev, unsigned char address, unsigned short data)
{
    unsigned char dataOffset = MAX_RESOLUTION - \
                                chip_info[dev->act_device].resolution;

    AD5686_SetShiftReg(dev, AD5686_CTRL_WRITE, address, data << dataOffset);
}

/**************************************************************************//**
 * @brief Update DAC Register n with contents of Input Register n
 *
 * @param   address  - the chosen channel to write to.
 *                     Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *
 * @return None.
******************************************************************************/
void AD5686_UpdateRegister(ad5686_dev *dev, unsigned char address)
{
    AD5686_SetShiftReg(dev, AD5686_CTRL_UPDATE, address, 0);
}

/**************************************************************************//**
 * @brief Write to and update DAC channel n
 *
 * @param   address  - the chosen channel to write to.
 *                      Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *          data - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5686_WriteUpdateRegister(ad5686_dev *dev, unsigned char address, unsigned short data)
{
    unsigned dataOffset = MAX_RESOLUTION - \
                                chip_info[dev->act_device].resolution;

    AD5686_SetShiftReg(dev, AD5686_CTRL_WRITEUPDATE, address, data << dataOffset);
}

/**************************************************************************//**
 * @brief Read back Input Register n
 *
 * @param   address  - the channel which will be read back. Note: only one
 *                     channel should be selected, if there will be selected
 *                     more than one channel, the channel A will be read back
 *                     by default
 *                     Example: 'AD5686_CH_C' will read back the channel C
 *
 * @return None.
******************************************************************************/
unsigned short AD5686_ReadBackRegister(ad5686_dev *dev, unsigned char address)
{

    unsigned short readBackData = 0;
    unsigned short offset = MAX_RESOLUTION - \
            chip_info[dev->act_device].resolution;

    if(chip_info[dev->act_device].communication == SPI)
    {
        AD5686_SetShiftReg(dev, AD5686_CTRL_RB_REG, address, 0);
        readBackData = AD5686_SetShiftReg(dev, AD5686_CTRL_NOP, 0, 0);
        readBackData >>= offset;
    }

return readBackData;
}

/**************************************************************************//**
 * @brief Write to and update DAC channel n
 *
 * @param   address  - the chosen channel to write to.
 *                     Example : 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *          mode     - Power-down operation modes.
                       Accepted values:
                        'AD5686_PWRM_NORMAL' - Normal Mode
 *                      'AD5686_PWRM_1K' - Power-down mode 1kOhm to GND
 *                      'AD5686_PWRM_100K' - Power-down mode 100kOhm to GND
 *                      'AD5686_PWRM_THREESTATE' - Three-State
 *
 * @return None.
******************************************************************************/
void AD5686_PowerMode(ad5686_dev *dev, unsigned char address, unsigned char mode)
{
    unsigned short data = 0;

    data |= (address & AD5686_CH_A) ? (mode << AD5686_PWRM_CHA_OFFSET) : 0x0;
    data |= (address & AD5686_CH_B) ? (mode << AD5686_PWRM_CHB_OFFSET) : 0x0;
    data |= (address & AD5686_CH_C) ? (mode << AD5686_PWRM_CHC_OFFSET) : 0x0;
    data |= (address & AD5686_CH_D) ? (mode << AD5686_PWRM_CHD_OFFSET) : 0x0;

    AD5686_SetShiftReg(dev, AD5686_CTRL_PWR, address, data);
}

/**************************************************************************//**
 * @brief Set hardware LDAC mask register
 *
 * @param   ldacMask  - in case of which channel ignore transitions on the LDAC *                      pin
 *                      Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *
 * @return None.
******************************************************************************/
void AD5686_LdacMask(ad5686_dev *dev, unsigned char ldacMask)
{
    AD5686_SetShiftReg(dev, AD5686_CTRL_LDAC_MASK, 0, ldacMask);
}

/**************************************************************************//**
 * @brief Software reset (power-on reset)
 *
 * @param  None.
 *
 * @return None.
******************************************************************************/
void AD5686_SoftwareReset(ad5686_dev *dev)
{
    AD5686_SetShiftReg(dev, AD5686_CTRL_SWRESET, 0, 0);
}


/**************************************************************************//**
 * @brief Write to Internal reference setup register
 * @param  value - the internal reference register value
 *                  Example : 'AD5686_INTREF_EN' - enable internal reference
 *                            'AD5686_INTREF_DIS' - disable internal reference
 *
 * @return None.
******************************************************************************/
void AD5686_InternalReference(ad5686_dev *dev, unsigned char value)
{
    AD5686_SetShiftReg(dev, AD5686_CTRL_IREF_REG, 0, value);
}

/**************************************************************************//**
 * @brief Set up DCEN register (daisy-chain enable)
 *
 * @param  value - Enable or disable daisy-chain mode
                   Example : 'AD5686_DC_EN' - daisy-chain enable
 *                           'AD5686_DC_DIS' - daisy-chain disable
 *
 * @return None.
******************************************************************************/
void AD5686_DaisyChainEn(ad5686_dev *dev, unsigned char value)
{
    AD5686_SetShiftReg(dev, AD5686_CTRL_DCEN, 0, value);
}

/**************************************************************************//**
 * @brief Set up readback register (readback enable)
 *
 * @param  value - Enable or disable daisy-chain mode
                   Example : 'AD5686_RB_EN' - daisy-chain enable
 *                           'AD5686_RB_DIS' - daisy-chain disable
 *
 * @return None.
******************************************************************************/
void AD5686_ReadBackEn(ad5686_dev *dev, unsigned char value)
{
    AD5686_SetShiftReg(dev, AD5686_CTRL_RB_REG, 0, value);
}
