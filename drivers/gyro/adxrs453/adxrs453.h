/***************************************************************************//**
 *   @file   ADXRS453.h
 *   @brief  Header file of ADXRS453 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __ADXRS453_H__
#define __ADXRS453_H__

#include <stdint.h>
#include "no_os_spi.h"

#define ADXRS453_READ           (1 << 7)
#define ADXRS453_WRITE          (1 << 6)
#define ADXRS453_SENSOR_DATA    (1 << 5)

#define ADXRS453_REG_RATE       0x00
#define ADXRS453_REG_TEM        0x02
#define ADXRS453_REG_LOCST      0x04
#define ADXRS453_REG_HICST      0x06
#define ADXRS453_REG_QUAD       0x08
#define ADXRS453_REG_FAULT      0x0A
#define ADXRS453_REG_PID        0x0C
#define ADXRS453_REG_SN_HIGH    0x0E
#define ADXRS453_REG_SN_LOW     0x10

struct adxrs453_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

struct adxrs453_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

/*! Initializes the ADXRS453 and checks if the device is present. */
int32_t adxrs453_init(struct adxrs453_dev **device,
		      struct adxrs453_init_param init_param);

/*! Free the resources allocated by adxrs453_init(). */
int32_t adxrs453_remove(struct adxrs453_dev *dev);

/*! Reads the value of a register. */
uint16_t adxrs453_get_register_value(struct adxrs453_dev *dev,
				     uint8_t register_address);

/*! Writes data into a register. */
void adxrs453_set_register_value(struct adxrs453_dev *dev,
				 uint8_t register_address,
				 uint16_t register_value);

/*! Reads the sensor data. */
uint32_t adxrs453_get_sensor_data(struct adxrs453_dev *dev);

/*! Reads the rate data and converts it to degrees/second. */
float adxrs453_get_rate(struct adxrs453_dev *dev);

/*! Reads the temperature sensor data and converts it to degrees Celsius. */
float adxrs453_get_temperature(struct adxrs453_dev *dev);

#endif // __ADXRS453_H__
