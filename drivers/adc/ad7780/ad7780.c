/***************************************************************************//**
 *   @file   AD7780.c
 *   @brief  Implementation of AD7780 Driver.
 *   @author DNechita (dan.nechita@analog.com)
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
#include "ad7780.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD7780 Board and checks if the device is present.
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
int8_t ad7780_init(struct ad7780_dev **device,
		   struct ad7780_init_param init_param)
{
	struct ad7780_dev *dev;
	uint8_t ad7780status;
	int8_t init_status;

	dev = (struct ad7780_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	init_status = no_os_gpio_get(&dev->gpio_pdrst, &init_param.gpio_pdrst);
	init_status = no_os_gpio_get(&dev->gpio_miso, &init_param.gpio_miso);
	init_status = no_os_gpio_get(&dev->gpio_filter, &init_param.gpio_filter);
	init_status = no_os_gpio_get(&dev->gpio_gain, &init_param.gpio_gain);

	if (dev->gpio_miso)
		init_status = no_os_gpio_direction_input(dev->gpio_miso);

	/* Set PDRST, FILTER and GAIN pins as output. */
	AD7780_PDRST_PIN_OUT;
	AD7780_FILTER_PIN_OUT;
	AD7780_GAIN_PIN_OUT;

	/* Set the initial value of the PDRST, FILTER and GAIN pins. */
	AD7780_PDRST_LOW;    // Places AD7780 in power-down mode.
	AD7780_FILTER_LOW;   // The update rate is set to 16.7 Hz.
	AD7780_GAIN_HIGH;    // Gain is set to 1.
	/* SPI */
	init_status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	if(init_status != 0) {
		return -1;
	}
	AD7780_PDRST_HIGH;
	init_status = ad7780_wait_rdy_go_low(dev);
	if(init_status != 0) {
		return -1;
	}
	ad7780_read_sample(dev,
			   &ad7780status);
	if((ad7780status & (AD7780_STAT_ID1 | AD7780_STAT_ID0)) != AD7780_ID_NUMBER) {
		return -1;
	}

	*device = dev;

	return init_status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7780_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad7780_remove(struct ad7780_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_pdrst);
	ret |= no_os_gpio_remove(dev->gpio_miso);
	ret |= no_os_gpio_remove(dev->gpio_filter);
	ret |= no_os_gpio_remove(dev->gpio_gain);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Waits for DOUT/RDY pin to go low.
 *
 * @param dev - The device structure.
 *
 * @return The result of the operation.
 *         Example: -1 - DOUT/RDY pin is high.
 *                   0 - DOUT/RDY pin is low.
*******************************************************************************/
int8_t ad7780_wait_rdy_go_low(struct ad7780_dev *dev)
{
	uint32_t timeout = 0xFFFFF;
	uint8_t wait = 1;

	while(wait && timeout) {
		AD7780_RDY_STATE(wait);
		timeout--;
	}
	if(timeout == 0) {
		return -1;
	} else {
		return 0;
	}
}

/***************************************************************************//**
 * @brief Reads a 24-bit sample from the ADC.
 *
 * @param dev          - The device structure.
 * @param p_status     - Stores the 8-bit status that is read for every conversion.
 *
 * @return convSample  - The 24-bit sample.
*******************************************************************************/
int32_t ad7780_read_sample(struct ad7780_dev *dev,
			   uint8_t* p_status)
{
	uint8_t rx_buff[4]  = {0, 0, 0, 0};
	int32_t conv_sample = 0;

	no_os_spi_write_and_read(dev->spi_desc,
				 rx_buff,
				 4);
	conv_sample = ((int32_t)rx_buff[0] << 16) +
		      ((int16_t)rx_buff[1] << 8) +
		      rx_buff[2];
	*p_status = rx_buff[3];

	return conv_sample;
}

/***************************************************************************//**
 * @brief Converts the 24-bit raw value to milivolts.
 *
 * @param raw_sample - 24-bit raw sample(offset binary).
 * @param v_ref      - The reference voltage.
 * @param gain       - The gain. Accepted values: 1, 128.
 *
 * @return voltage   - The voltage obtained from the raw value.
*******************************************************************************/
float ad7780_convert_to_voltage(uint32_t raw_sample,
				float v_ref,
				uint8_t gain)
{
	float voltage = 0;

	voltage = 1000 * ((float)raw_sample / (1ul << 23) - 1) * v_ref / gain;

	return voltage;
}









