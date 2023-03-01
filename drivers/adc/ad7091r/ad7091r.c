/***************************************************************************//**
 *   @file   AD7091R.c
 *   @brief  Implementation of AD7091R Driver.
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
#include "ad7091r.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD7092R Board.
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
int8_t ad7091r_init(struct ad7091r_dev **device,
		    struct ad7091r_init_param init_param)
{
	struct ad7091r_dev *dev;
	uint8_t status = 0;
	uint8_t tmp_val = 0xFF;

	dev = (struct ad7091r_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	/* Ensures that last state of SDO is high. */
	no_os_spi_write_and_read(dev->spi_desc, &tmp_val, 1);
	ad7091r_software_reset(dev);

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7091r_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad7091r_remove(struct ad7091r_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Initiate a software reset of the device. The AD7091R requires the user
 *        to initiate a software reset when power is first applied.
 *
 * @param dev     - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad7091r_software_reset(struct ad7091r_dev *dev)
{
	uint8_t write_byte = 0xBF;

	/* Initiate a conversion. */
	no_os_spi_write_and_read(dev->spi_desc, &write_byte, 1);
	/* Short cycle the read operation. */
	write_byte = 0xFF;
	no_os_spi_write_and_read(dev->spi_desc, &write_byte, 1);
	/* Perform another conversion in order to reset the device. */
	write_byte = 0xBF;
	no_os_spi_write_and_read(dev->spi_desc, &write_byte, 1);
}

/***************************************************************************//**
 * @brief Initiates one conversion and reads back the result. During this
 *        process the device runs in normal mode and operates without the busy
 *        indicator.
 *
 * @param dev     - The device structure.
 *
 * @return conversionResult - 12bit conversion result.
*******************************************************************************/
uint16_t ad7091r_read_sample(struct ad7091r_dev *dev)
{
	uint16_t conversion_result = 0;
	uint8_t write_byte         = 0xBF;
	uint8_t buffer[2]          = {0xFF, 0xFF};

	/* Initiate a conversion. */
	no_os_spi_write_and_read(dev->spi_desc, &write_byte, 1);
	/* Read conversion data. */
	no_os_spi_write_and_read(dev->spi_desc, buffer, 2);
	conversion_result = (buffer[0] << 8) + buffer[1];
	conversion_result >>= 4;

	return conversion_result;
}

/***************************************************************************//**
 * @brief Puts the device in power-down mode.
 *
 * @param dev     - The device structure.
 *
 * @return None.
*******************************************************************************/
void ad7091r_power_down(struct ad7091r_dev *dev)
{
	uint8_t buffer[2]   = {0, 0};
	uint8_t write_value = 0x00;

	/* Initiate a conversion. */
	no_os_spi_write_and_read(dev->spi_desc, &write_value, 1);
	/* Perform a dummy read. */
	no_os_spi_write_and_read(dev->spi_desc, buffer, 2);
}

/***************************************************************************//**
 * @brief Powers up the device by pulling CONVST high. After calling this
 *        function, a time delay is required before initiating another
 *        conversion:
 *         - 50 ms delay, when internal reference is used.(with 2.2uF capacitor)
 *         - 100 us delay, when external reference is used.
 *
 * @param dev     - The device structure.
 *
 * @return  None.
*******************************************************************************/
void ad7091r_power_up(struct ad7091r_dev *dev)
{
	uint8_t write_value = 0xFF;

	/* Pull CONVST signal high. */
	no_os_spi_write_and_read(dev->spi_desc, &write_value, 1);
}


/***************************************************************************//**
 * @brief Converts a 12-bit raw sample to volts.
 *
 * @param raw_sample - 12-bit data sample.
 * @param v_ref     - The value of the voltage reference used by the device.
 *
 * @return voltage  - The result of the conversion expressed as volts.
*******************************************************************************/
float ad7091r_convert_to_volts(int16_t raw_sample, float v_ref)
{
	float voltage = 0;

	if(v_ref == 0) {
		v_ref = 2.5;
	}
	voltage = v_ref * (float)raw_sample / (1 << 12);

	return voltage;
}
