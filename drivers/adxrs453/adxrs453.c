/***************************************************************************//**
 *   @file   ADXRS453.c
 *   @brief  Implementation of ADXRS453 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
 *
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "adxrs453.h"

/***************************************************************************//**
 * @brief Initializes the ADXRS453 and checks if the device is present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful (ID starts
 *                                with 0x52).
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char adxrs453_init(adxrs453_dev **device,
		   adxrs453_init_param init_param)
{
	adxrs453_dev *dev;
	char           status      = 0;
	unsigned short adxrs453_id = 0;

	dev = (adxrs453_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->spi_dev.type = init_param.spi_type;
	dev->spi_dev.id = init_param.spi_id;
	dev->spi_dev.max_speed_hz = init_param.spi_max_speed_hz;
	dev->spi_dev.mode = init_param.spi_mode;
	dev->spi_dev.chip_select = init_param.spi_chip_select;
	status = spi_init(&dev->spi_dev);

	/* Read the value of the ADXRS453 ID register. */
	adxrs453_id = adxrs453_get_register_value(dev, ADXRS453_REG_PID);
	if((adxrs453_id >> 8) != 0x52)
		status = -1;

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev             - The device structure.
 * @param registerAddress - Address of the register.
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
unsigned short adxrs453_get_register_value(adxrs453_dev *dev,
		unsigned char registerAddress)
{
	unsigned char  data_buffer[4] = {0, 0, 0, 0};
	unsigned long  command        = 0;
	unsigned char  bit_no         = 0;
	unsigned char  sum            = 0;
	unsigned short register_value = 0;

	data_buffer[0] = ADXRS453_READ | (registerAddress >> 7);
	data_buffer[1] = (registerAddress << 1);
	command = ((unsigned long)data_buffer[0] << 24) |
		  ((unsigned long)data_buffer[1] << 16) |
		  ((unsigned short)data_buffer[2] << 8) |
		  data_buffer[3];
	for(bit_no = 31; bit_no > 0; bit_no--) {
		sum += ((command >> bit_no) & 0x1);
	}
	if(!(sum % 2)) {
		data_buffer[3] |= 1;
	}
	data_buffer[4] = data_buffer[0];
	data_buffer[5] = data_buffer[1];
	data_buffer[6] = data_buffer[2];
	data_buffer[7] = data_buffer[3];
	spi_write_and_read(&dev->spi_dev, data_buffer, 4);
	spi_write_and_read(&dev->spi_dev, &data_buffer[4], 4);
	register_value = ((unsigned short)data_buffer[1] << 11) |
			 ((unsigned short)data_buffer[2] << 3) |
			 (data_buffer[3] >> 5);

	return register_value;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev             - The device structure.
 * @param registerAddress - Address of the register.
 * @param register_value   - Data value to write.
 *
 * @return None.
*******************************************************************************/
void adxrs453_set_register_value(adxrs453_dev *dev,
				 unsigned char registerAddress,
				 unsigned short register_value)
{
	unsigned char data_buffer[4] = {0, 0, 0, 0};
	unsigned long command        = 0;
	unsigned char bit_no         = 0;
	unsigned char sum            = 0;

	data_buffer[0] = ADXRS453_WRITE | (registerAddress >> 7);
	data_buffer[1] = (registerAddress << 1) |
			 (register_value >> 15);
	data_buffer[2] = (register_value >> 7);
	data_buffer[3] = (register_value << 1);

	command = ((unsigned long)data_buffer[0] << 24) |
		  ((unsigned long)data_buffer[1] << 16) |
		  ((unsigned short)data_buffer[2] << 8) |
		  data_buffer[3];
	for(bit_no = 31; bit_no > 0; bit_no--)
		sum += ((command >> bit_no) & 0x1);

	if(!(sum % 2))
		data_buffer[3] |= 1;

	spi_write_and_read(&dev->spi_dev, data_buffer, 4);
}

/***************************************************************************//**
 * @brief Reads the sensor data.
 *
 * @param dev - The device structure.
 *
 * @return register_value - The sensor data.
*******************************************************************************/
unsigned long adxrs453_get_sensor_data(adxrs453_dev *dev)
{
	unsigned char data_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned long command        = 0;
	unsigned char bit_no         = 0;
	unsigned char sum            = 0;
	unsigned long register_value = 0;

	data_buffer[0] = ADXRS453_SENSOR_DATA;
	command = ((unsigned long)data_buffer[0] << 24) |
		  ((unsigned long)data_buffer[1] << 16) |
		  ((unsigned short)data_buffer[2] << 8) |
		  data_buffer[3];
	for(bit_no = 31; bit_no > 0; bit_no--)
		sum += ((command >> bit_no) & 0x1);

	if(!(sum % 2))
		data_buffer[3] |= 1;

	data_buffer[4] = data_buffer[0];
	data_buffer[5] = data_buffer[1];
	data_buffer[6] = data_buffer[2];
	data_buffer[7] = data_buffer[3];
	spi_write_and_read(&dev->spi_dev, data_buffer, 4);
	spi_write_and_read(&dev->spi_dev, &data_buffer[4], 4);
	register_value = ((unsigned long)data_buffer[0] << 24) |
			 ((unsigned long)data_buffer[1] << 16) |
			 ((unsigned short)data_buffer[2] << 8) |
			 data_buffer[3];

	return register_value;
}

/***************************************************************************//**
 * @brief Reads the rate data and converts it to degrees/second.
 *
 * @param dev - The device structure.
 *
 * @return rate - The rate value in degrees/second.
*******************************************************************************/
float adxrs453_get_rate(adxrs453_dev *dev)
{
	unsigned short register_value = 0;
	float          rate           = 0.0;

	register_value = adxrs453_get_register_value(dev, ADXRS453_REG_RATE);

	/*!< If data received is in positive degree range */
	if(register_value < 0x8000)
		rate = ((float)register_value / 80);
	/*!< If data received is in negative degree range */
	else
		rate = (-1) * ((float)(0xFFFF - register_value + 1) / 80.0);

	return rate;
}

/***************************************************************************//**
 * @brief Reads the temperature sensor data and converts it to degrees Celsius.
 *
 * @param dev - The device structure.
 *
 * @return temperature - The temperature value in degrees Celsius.
*******************************************************************************/
float adxrs453_get_temperature(adxrs453_dev *dev)
{
	unsigned long register_value = 0;
	float         temperature    = 0;

	register_value = adxrs453_get_register_value(dev, ADXRS453_REG_TEM);
	register_value = (register_value >> 6) - 0x31F;
	temperature = (float) register_value / 5;

	return temperature;
}
