/***************************************************************************//**
 *   @file   AD74xx.c
 *   @brief  Implementation of AD74xx Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
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
#include "ad74xx.h"		// AD74xx definitions.
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD74xx Board.
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
int8_t ad74xx_init(struct ad74xx_dev **device,
		   struct ad74xx_init_param init_param)
{
	struct ad74xx_dev *dev;
	uint8_t status;

	dev = (struct ad74xx_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	status |= no_os_gpio_get(&dev->gpio_cs, &init_param.gpio_cs);

	if (dev->gpio_cs)
		status |= no_os_gpio_direction_output(dev->gpio_cs,
						      NO_OS_GPIO_HIGH);

	dev->device_bits_number = init_param.device_bits_number;
	dev->part_number = init_param.part_number;

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad74xx_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad74xx_remove(struct ad74xx_dev *dev)
{
	int32_t status;

	status = no_os_spi_remove(dev->spi_desc);

	status = no_os_gpio_remove(dev->gpio_cs);

	no_os_free(dev);

	return status;
}

/***************************************************************************//**
 * @brief Powers down the device. This function is supported only by:
 *        AD7475(partial power down), AD7476, AD7477, AD7478, AD7476A, AD7477A,
 *        AD7478A, AD7479(partial power down).
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad74xx_power_down(struct ad74xx_dev *dev)
{
	uint8_t dummy_value = 0;

	AD74XX_CS_LOW;
	no_os_spi_write_and_read(dev->spi_desc, &dummy_value, 1);
	AD74XX_CS_HIGH;     // CS is brought "High" between 2nd falling edge of SCLK
	// and 10th falling edge of SCLK(8th falling edge, here)
	no_os_spi_write_and_read(dev->spi_desc, &dummy_value, 1);
}

/***************************************************************************//**
 * @brief Powers up the device by performing a dummy read. This function is
 *        supported only by: AD7475(partial power down), AD7476, AD7477, AD7478,
 *        AD7476A, AD7477A, AD7478A, AD7479(partial power down).
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad74xx_power_up(struct ad74xx_dev *dev)
{
	uint8_t dummy_value[2] = {0, 0};

	no_os_spi_write_and_read(dev->spi_desc, dummy_value, 2);
}

/***************************************************************************//**
 * @brief Reads the conversion value.
 *
 * @param dev - The device structure.
 *
 * @return convResult - conversion data.
*******************************************************************************/
uint16_t ad74xx_get_register_value(struct ad74xx_dev *dev)
{
	uint8_t data_word[2] = {0, 0};
	uint16_t conv_result = 0;

	no_os_spi_write_and_read(dev->spi_desc, data_word, 2);
	switch(dev->device_bits_number) {
	case 8:
		conv_result = (((uint16_t)(data_word[0] & 0x1F)) << 8) +
			      (data_word[1] & 0xE0);
		conv_result = conv_result >> 5;
		break;
	case 10:
		conv_result = (((uint16_t)(data_word[0] & 0x1F)) << 8) +
			      (data_word[1] & 0xF8);
		conv_result = conv_result >> 3;
		break;
	case 12:
		conv_result = (((uint16_t)(data_word[0] & 0x1F)) << 8) +
			      (data_word[1] & 0xFE);
		conv_result = conv_result >> 1;
		break;
	default: // 16 bits
		conv_result = (((uint16_t)(data_word[0] & 0xFF)) << 8) +
			      data_word[1];
		break;
	}

	return conv_result;
}

/***************************************************************************//**
 * @brief Converts a raw sample to volts.
 *
 * @param dev       - The device structure.
 * @param raw_value - The data sample.
 * @param v_ref     - The value of the voltage reference used by the device.
 *
 * @return voltage  - The result of the conversion expressed as volts.
*******************************************************************************/
float ad74xx_convert_to_volts(struct ad74xx_dev *dev,
			      uint16_t raw_value,
			      float v_ref)
{
	float voltage = 0;

	voltage = (raw_value) * v_ref / (1 << dev->device_bits_number);

	return voltage;
}
