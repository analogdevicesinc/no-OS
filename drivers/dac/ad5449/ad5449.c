/***************************************************************************//**
*   @file   AD5449.c
*   @brief  Implementation of AD5449 Driver.
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
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdlib.h>
#include "ad5449.h"
#include "no_os_alloc.h"

#define MAX_RESOLUTION  12     /* Maximum resolution of the supported devices */
#define DATA_MASK       0xFFF  /* Mask for 16 data bits */
#define CMD_MASK        0xFF   /* Mask for Command bits */
#define CMD_OFFSET      12     /* Offset for Command */
#define MSB_MASK        0xFF00 /* Most significant byte of the data word */
#define MSB_OFFSET      8
#define LSB_MASK        0x00FF /* Least significant byte of the data word */
#define LSB_OFFSET      0
#define PKT_LENGTH      2      /* SPI packet length in byte */

#define TWO_BIT_MASK    0x3

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
static const struct ad5449_chip_info chip_info[] = {
	[ID_AD5415] = {
		.num_channels = 2,
		.resolution = 12,
		.has_ctrl = true,
	},
	[ID_AD5426] = {
		.num_channels = 1,
		.resolution = 8,
		.has_ctrl = false,
	},
	[ID_AD5429] = {
		.num_channels = 2,
		.resolution = 8,
		.has_ctrl = true,
	},
	[ID_AD5432] = {
		.num_channels = 1,
		.resolution = 10,
		.has_ctrl = false,
	},
	[ID_AD5439] = {
		.num_channels = 2,
		.resolution = 10,
		.has_ctrl = true,
	},
	[ID_AD5443] = {
		.num_channels = 1,
		.resolution = 12,
		.has_ctrl = false,
	},
	[ID_AD5449] = {
		.num_channels = 2,
		.resolution = 12,
		.has_ctrl = true,
	}
};

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5449 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return retValue - Result of the initialization.
 *                    Example: 0 - if initialization was successful;
 *                            -1 - if initialization was unsuccessful.
******************************************************************************/
int8_t ad5449_init(struct ad5449_dev **device,
		   struct ad5449_init_param init_param)
{
	struct ad5449_dev *dev;
	int8_t status;

	dev = (struct ad5449_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->act_device = init_param.act_device;

	/* Initialize SPI communication. */
	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	status |= no_os_gpio_get(&dev->gpio_ldac, &init_param.gpio_ldac);
	status |= no_os_gpio_get(&dev->gpio_clr, &init_param.gpio_clr);

	/* Set GPIO pins. */
	AD5449_LDAC_OUT;
	AD5449_LDAC_LOW;
	AD5449_CLR_OUT;
	AD5449_CLR_HIGH;

	/* Initialize device */
	dev->control_reg = (AD5449_SDO_FULL << AD5449_SDO_BIT) | \
			   (AD5449_DAISY_CHAIN_DIS << AD5449_DSY_BIT) | \
			   (AD5449_ZERO_SCALE << AD5449_HCLR_BIT) | \
			   (AD5449_CLOCK_NEGEDGE << AD5449_SCLK_BIT);
	if(chip_info[dev->act_device].has_ctrl) {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_REG,
					   dev->control_reg);
	} else {
		ad5449_daisy_chain_setup(dev,
					 AD5449_DAISY_CHAIN_DIS);
	}

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5449_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad5449_remove(struct ad5449_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_ldac);
	ret |= no_os_gpio_remove(dev->gpio_clr);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Write to input register and read from output register via SPI.
 *
 * @param   dev      - The device structure.
 * @param   command  - command control bits.
 * @param   data     - data to be written in input register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
uint16_t ad5449_set_input_shift_reg(struct ad5449_dev *dev,
				    uint16_t command,
				    uint16_t data)
{
	uint16_t input_shift_reg = 0;
	uint16_t readback = 0;
	uint8_t spi_data[PKT_LENGTH] = {0, 0};
	uint8_t data_offset = MAX_RESOLUTION - \
			      chip_info[dev->act_device].resolution;

	if(command == AD5449_CTRL_REG) {
		input_shift_reg = ((command & CMD_MASK) << CMD_OFFSET) | \
				  (data & DATA_MASK);
	} else {
		input_shift_reg = ((command & CMD_MASK) << CMD_OFFSET) | \
				  ((data & DATA_MASK) << data_offset);
	}

	spi_data[0] = (input_shift_reg & MSB_MASK) >> MSB_OFFSET;
	spi_data[1] = (input_shift_reg & LSB_MASK) >> LSB_OFFSET;

	no_os_spi_write_and_read(dev->spi_desc,
				 spi_data,
				 PKT_LENGTH);
	readback = ((uint16_t)spi_data[0] << MSB_OFFSET) | \
		   ((uint16_t)spi_data[1] << LSB_OFFSET);

	return readback;
}

/**************************************************************************//**
 * @brief Loads and updates the selected DAC with a given value.
 *
 * @param   dev       - The device structure.
 * @param   channel   - the chosen channel to write to.
 *                      Example: AD5449_CH_A = 0;
 *                               AD5449_CH_B = 1.
 * @param   dac_value - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5449_load_update_channel(struct ad5449_dev *dev,
				uint8_t channel,
				uint16_t dac_value)
{
	if(chip_info[dev->act_device].num_channels > 1) {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_LOADUPDATE(channel),
					   dac_value);
	} else {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_LOADUPDATE(AD5449_CH_A),
					   dac_value);
	}
}

/**************************************************************************//**
 * @brief Load selected DAC input register with a given value.
 *
 * @param   dev       - The device structure.
 * @param   channel   - the chosen channel to write to.
 *                      Example: AD5449_CH_A = 0;
 *                               AD5449_CH_B = 1.
 * @param   dac_value - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5449_load_channel(struct ad5449_dev *dev,
			 uint8_t channel,
			 uint16_t dac_value)
{
	if(chip_info[dev->act_device].num_channels > 1) {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_LOAD(channel),
					   dac_value);
	} else {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_LOAD(AD5449_CH_A),
					   dac_value);
	}
}

/**************************************************************************//**
 * @brief Read from the selected DAC register.
 *
 * @param   dev      - The device structure.
 * @param   channel  - the chosen channel to read from.
 *                      Example: AD5449_CH_A = 0;
 *                               AD5449_CH_B = 1.
 *
 * @return  dac_value - value read from the register.
******************************************************************************/
uint16_t ad5449_readback_channel(struct ad5449_dev *dev,
				 uint8_t channel)
{
	uint16_t dac_value = 0;

	if(chip_info[dev->act_device].num_channels > 1) {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_READBACK(channel),
					   0x0);
		dac_value = ad5449_set_input_shift_reg(dev,
						       AD5449_CTRL_NOP,
						       0x0) & DATA_MASK;
	} else {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_READBACK(AD5449_CH_A),
					   0x0);
		dac_value = ad5449_set_input_shift_reg(dev,
						       AD5449_CTRL_NOP,
						       0x0) & DATA_MASK;
	}
	return dac_value;
}

/**************************************************************************//**
 * @brief Update the DAC outputs. This function has a physical result just with
 *        devices with two channels
 *
 * @param   dev      - The device structure.
 *
 * @return None.
******************************************************************************/
void ad5449_update_all(struct ad5449_dev *dev)
{
	if(chip_info[dev->act_device].num_channels > 1) {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_UPDATEALL,
					   0x0);
	}
}

/**************************************************************************//**
 * @brief Load the DAC input registers. This function has a physical result
 *        just with devices with two channels
 *
 * @param dev       - The device structure.
 * @param dac_value - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void ad5449_load_all(struct ad5449_dev *dev,
		     int16_t dac_value)
{
	if(chip_info[dev->act_device].num_channels > 1) {
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_LOADALL,
					   dac_value);
	}
}

/**************************************************************************//**
 * @brief Set up the scale where to the output will be cleared on active CLR
 *        signal
 *
 * @param  dev  - The device structure.
 * @param  type - target scale when making a clear command
 *                Example: ZERO_SCALE = zero scale.
 *                         MIDSCALE   = midscale.
 *
 * @return None.
******************************************************************************/
void ad5449_clear_scale_setup(struct ad5449_dev *dev,
			      int8_t type)
{
	if(chip_info[dev->act_device].has_ctrl) {
		if(type == AD5449_ZERO_SCALE) {
			/* Reset the HCLR bit in the Control Register */
			dev->control_reg &= ~AD5449_HCLR_MASK;
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_REG,
						   dev->control_reg);
		} else if(type == AD5449_MID_SCALE) {
			/* Set the HCLR bit in the Control Register */
			dev->control_reg |= AD5449_HCLR_MASK;
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_REG,
						   dev->control_reg);
		}
	} else {
		if(type == AD5449_ZERO_SCALE) {
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_CLR_ZERO,
						   0x0);
		} else if (type == AD5449_MID_SCALE) {
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_CLR_MID,
						   0x0);
		}
	}
}

/**************************************************************************//**
 * @brief Enable/disable the Daisy-Chain mode
 *
 * @param  dev  - The device structure.
 * @param  value - target scale when making a clear command
 *                Example: ZERO_SCALE = zero scale.
 *                         MIDSCALE   = midscale.
 *
 * @return None.
******************************************************************************/
void ad5449_daisy_chain_setup(struct ad5449_dev *dev,
			      int8_t value)
{
	if(chip_info[dev->act_device].has_ctrl) {
		if(value == AD5449_DAISY_CHAIN_EN) {
			/* Set the DSY bit in the Control Register */
			dev->control_reg |= AD5449_DSY_MASK;
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_REG,
						   dev->control_reg);
		} else if(value == AD5449_DAISY_CHAIN_DIS) {
			/* Reset the DSY bit in the Control Register */
			dev->control_reg &= ~AD5449_DSY_MASK;
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_REG,
						   dev->control_reg);
		}
	} else {
		/* Disable daisy chain mode */
		if (value == AD5449_DAISY_CHAIN_DIS) {
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_DAISY_CHAIN,
						   0x0);
		}
	}
}

/**************************************************************************//**
 * @brief Control the SDO output driver strength. This function is supported
 * by the devices : AD5415, AD5429, AD5439, AD5449
 *
 * @param  dev          - The device structure.
 * @param  control_bits - target scale when making a clear command
 *                Example: AD5449_SDO_FULL = Full SDO driver.
 *                         AD5449_SDO_WEAK = Weak SDO driver.
 *                         AD5449_SDO_OPEN_DRAIN = SDO configured as open drain
 *                         AD5449_SDO_DISABLE = Disable SDO output
 * @return None.
******************************************************************************/
void ad5449_sdocontrol(struct ad5449_dev *dev,
		       int8_t control_bits)
{
	/* Check if the current device supports this functions */
	if(chip_info[dev->act_device].has_ctrl) {
		/* Reset the SDO bits of the local Control Register */
		dev->control_reg &= ~AD5449_SDO_MASK;
		/* Set up the new SDO bits */
		dev->control_reg |= (control_bits & TWO_BIT_MASK) << AD5449_SDO_BIT;
		/* Send the new Control Register value to the device */
		ad5449_set_input_shift_reg(dev,
					   AD5449_CTRL_REG,
					   dev->control_reg);
	} else { /* Do Nothing! */

	}
}

/**************************************************************************//**
 * @brief Set up the active clock edge of the SPI interface
 *
 * @param  dev   - The device structure.
 * @param  value - target scale when making a clear command
 *                Example: AD5449_CLOCK_NEGEDGE = Falling edge. (Default)
 *                         AD5449_CLOCK_POSEDGE = Rising edge.
 *
 * @return None.
******************************************************************************/
void ad5449_sclksetup(struct ad5449_dev *dev,
		      int8_t value)
{
	if(chip_info[dev->act_device].has_ctrl) {
		if(value == AD5449_CLOCK_POSEDGE) {
			/* Set the SCLK bit in the Control Register */
			dev->control_reg |= AD5449_SCLK_MASK;
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_REG,
						   dev->control_reg);
		} else if(value == AD5449_CLOCK_NEGEDGE) {
			/* Reset the SCLK bit in the Control Register */
			dev->control_reg &= ~AD5449_SCLK_MASK;
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_REG,
						   dev->control_reg);
		}
	} else {
		/* Clock data on rising edge */
		if (value == AD5449_CLOCK_POSEDGE) {
			ad5449_set_input_shift_reg(dev,
						   AD5449_CTRL_CLK_EDGE,
						   0x0);
		}
	}
}
