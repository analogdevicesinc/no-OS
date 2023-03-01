/***************************************************************************//**
*   @file   AD5446.c
*   @brief  Implementation of AD5446 Driver. This driver supporting the
*              following devices: AD5553, AD5543, AD5542A, AD5541A, AD5512A,
*                                 AD5453, AD5452, AD5451, AD5450, AD5446, AD5444
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
#include "ad5446.h"
#include "no_os_alloc.h"

#define MAX_RESOLUTION  16      /* Maximum resolution of supported devices */
#define DATA_MASK(x)    (0xFFFF >> (MAX_RESOLUTION - (x)))
#define DATA_OFFSET(x)  ((MAX_RESOLUTION - 2) - (x))
#define MSB_MASK        0xFF00
#define MSB_OFFSET      8
#define LSB_MASK        0x00FF
#define LSB_OFFSET      0
#define CMD_MASK        0x3
#define CMD_OFFSET      15
#define PKT_LENGTH      2       /* SPI packet length in byte */

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/

/* Device 'table' */
static const struct ad5446_chip_info chip_info[] = {
	[ID_AD5600] = {
		.resolution = 16,
		.data_clock_in = posedge,
		.has_ctrl = false,
	},
	[ID_AD5553] = {
		.resolution = 14,
		.data_clock_in = posedge,
		.has_ctrl = false,
	},
	[ID_AD5543] = {
		.resolution = 16,
		.data_clock_in = posedge,
		.has_ctrl = false,
	},
	[ID_AD5542A] = {
		.resolution = 16,
		.data_clock_in = posedge,
		.has_ctrl = false,
	},
	[ID_AD5541A] = {
		.resolution = 16,
		.data_clock_in = posedge,
		.has_ctrl = false,
	},
	[ID_AD5512A] = {
		.resolution = 12,
		.data_clock_in = posedge,
		.has_ctrl = false,
	},
	[ID_AD5453] = {
		.resolution = 14,
		.data_clock_in = negedge,
		.has_ctrl = true,
	},
	[ID_AD5452] = {
		.resolution = 12,
		.data_clock_in = negedge,
		.has_ctrl = true,
	},
	[ID_AD5451] = {
		.resolution = 10,
		.data_clock_in = negedge,
		.has_ctrl = true,
	},
	[ID_AD5450] = {
		.resolution = 8,
		.data_clock_in = negedge,
		.has_ctrl = true,
	},
	[ID_AD5446] = {
		.resolution = 14,
		.data_clock_in = negedge,
		.has_ctrl = true,
	},
	[ID_AD5444] = {
		.resolution = 12,
		.data_clock_in = negedge,
		.has_ctrl = true,
	}
};

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5446 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return retValue - Result of the initialization.
 *                    Example: 0 - if initialization was successful;
 *                            -1 - if initialization was unsuccessful.
******************************************************************************/
int8_t ad5446_init(struct ad5446_dev **device,
		   struct ad5446_init_param init_param)
{
	struct ad5446_dev *dev;
	int8_t status;

	dev = (struct ad5446_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->act_device = init_param.act_device;

	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	status |= no_os_gpio_get(&dev->gpio_ladc, &init_param.gpio_ladc);
	status |= no_os_gpio_get(&dev->gpio_clrout, &init_param.gpio_clrout);

	/* Initialize configuration pins, if exist. */
	if(dev->act_device == ID_AD5542A) {
		AD5446_LDAC_OUT;
		AD5446_LDAC_LOW;
		AD5446_CLR_OUT;
		AD5446_CLR_HIGH;
	} else if(dev->act_device == ID_AD5541A ||
		  dev->act_device == ID_AD5600) {
		AD5446_LDAC_OUT;
		AD5446_LDAC_LOW;
	} else if(dev->act_device == ID_AD5446) { /* Enable the SDO line */
		/* AD5446_CLR is mapped to GPIO0 */
		AD5446_CLR_OUT;
		AD5446_CLR_LOW;
	}

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5446_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad5446_remove(struct ad5446_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_ladc);
	ret |= no_os_gpio_remove(dev->gpio_clrout);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Writes to input register via SPI.
 *
 * @param dev      - The device structure.
 * @param command  - Command to be transmitted to the device.
 * @param data     - Data to be written in input register.
 *
 * @return  None.
******************************************************************************/
void ad5446_set_register(struct ad5446_dev *dev,
			 uint8_t command,
			 uint16_t data)
{
	uint16_t input_shift_reg = 0;
	uint8_t spi_data[PKT_LENGTH] = {0, 0};

	if(chip_info[dev->act_device].has_ctrl == true) {
		input_shift_reg = ((command & CMD_MASK) << CMD_OFFSET) |
				  (data & DATA_MASK(chip_info[dev->act_device].resolution) \
				   << DATA_OFFSET(chip_info[dev->act_device].resolution));
		spi_data[0] = (input_shift_reg & MSB_MASK) >> MSB_OFFSET;
		spi_data[1] = (input_shift_reg & LSB_MASK) >> LSB_OFFSET;
	} else {
		input_shift_reg = data & (DATA_MASK(chip_info[dev->act_device].resolution));
		spi_data[0] = (input_shift_reg & MSB_MASK) >> MSB_OFFSET;
		spi_data[1] = (input_shift_reg & LSB_MASK);
	}
	no_os_spi_write_and_read(dev->spi_desc,
				 spi_data,
				 PKT_LENGTH);
}
/***************************************************************************//**
 * @brief Sets the output voltage.
 *
 * @param dev        - The device structure.
 * @param voltage - The voltage value in volts
 * @param vref       - The voltage reference used by the device in volts.
 * @param vout_type - Voltage output type: unipolar/unipolar_inv/bipolar
 *
 * @return Actual voltage that the device can output.
*******************************************************************************/
float ad5446_set_voltage(struct ad5446_dev *dev,
			 float voltage,
			 float vref,
			 enum vout_type_t vout_type)
{
	uint16_t register_value = 0;
	float actual_vout = 0;
	float code = 0;
	uint16_t  max_value = DATA_MASK(chip_info[dev->act_device].resolution);

	/* Get raw data from the user's desired voltage value. */
	switch(vout_type) {
	case unipolar : {
		code = (voltage * max_value) / vref;
		break;
	}
	case unipolar_inv : {
		code = (-1) * (voltage * max_value) / vref;
		break;
	}
	case bipolar : {
		code = ((voltage + vref) * (max_value/2)) / vref;
		break;
	}
	}

	/* Round to the nearest integer. */
	register_value = (uint16_t)code;

	/* Check to value which will be written to register. */
	if (register_value > (max_value - 1) ) {
		register_value = (max_value - 1);
	}

	/* Write to DAC register. */
	ad5446_set_register(dev,
			    0,
			    register_value);
	/* Calculate the output voltage value. */
	switch(vout_type) {
	case unipolar : {
		actual_vout = ((float)register_value / max_value) * vref;
		break;
	}
	case unipolar_inv : {
		actual_vout = (-1) * ((float)register_value / max_value) * vref;
		break;
	}
	case bipolar : {
		actual_vout = (vref * (float)register_value / (max_value/2)) - vref;
		break;
	}
	}

	return actual_vout;
}
