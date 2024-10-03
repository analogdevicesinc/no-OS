/***************************************************************************//**
 *   @file   AD7980.c
 *   @brief  Implementation of 7980 Driver.
 *   @author Bancisor Mihai(Bancisor.Mihai@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad7980.h"           // AD7980 definitions.
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Initialization status.
 *                  Example: -1 - Initialization failed;
 *                            0 - Initialization succeeded.
*******************************************************************************/
int8_t ad7980_init(struct ad7980_dev **device,
		   struct ad7980_init_param init_param)
{
	struct ad7980_dev *dev;
	uint8_t status;

	dev = (struct ad7980_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	/* GPIO */
	status |= no_os_gpio_get(&dev->gpio_cs, &init_param.gpio_cs);

	if (dev->gpio_cs)
		status |= no_os_gpio_direction_output(dev->gpio_cs,
						      NO_OS_GPIO_HIGH);

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7980_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad7980_remove(struct ad7980_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_cs);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Initiates conversion and reads data.
 *
 * @param dev           - The device structure.
 *
 * @return receivedData - Data read from the ADC.
*******************************************************************************/
uint16_t ad7980_conversion(struct ad7980_dev *dev)
{
	uint16_t received_data = 0;
	uint8_t tx_data[1] = {0};
	uint8_t rx_data[2] = {0, 0};

	tx_data[0] = 0x7F;
	no_os_spi_write_and_read(dev->spi_desc,
				 tx_data,
				 1);
	AD7980_CS_LOW;
	rx_data[0] = 0xFF;
	rx_data[1] = 0xFF;
	no_os_spi_write_and_read(dev->spi_desc,
				 rx_data,
				 2);
	AD7980_CS_HIGH;
	received_data = (rx_data[0] << 8) + rx_data[1];

	return(received_data);
}

/***************************************************************************//**
 * @brief Converts a 16-bit raw sample to volts.
 *
 * @param raw_sample - 16-bit data sample.
 * @param v_ref      - The value of the voltage reference used by the device.
 *
 * @return voltage   - The result of the conversion expressed as volts.
*******************************************************************************/
float ad7980_convert_to_volts(uint16_t raw_sample, float v_ref)
{
	float voltage = 0;

	voltage = v_ref * (float)raw_sample / (1ul << 16);

	return voltage;
}
