/***************************************************************************//**
 *   @file   platform_drivers.h
 *   @brief  Header file of Generic Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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

#ifndef PLATFORM_DRIVERS_H_
#define PLATFORM_DRIVERS_H_
#include <xgpiops.h>
#include <xspips.h>
#include "spi_engine.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SUCCESS		0
#define FAILURE		-1

#define	SPI_CPHA	0x01
#define	SPI_CPOL	0x02

#define GPIO_OUT	0x01
#define GPIO_IN		0x00

#define GPIO_HIGH	0x01
#define GPIO_LOW	0x00

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum {
	GENERIC_I2C
} i2c_type;

typedef struct {
	i2c_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t		slave_address;
} i2c_init_param;

typedef struct {
	i2c_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t		slave_address;
} i2c_desc;

typedef enum {
	GENERIC_SPI,
	PS7_SPI
} spi_type;

typedef enum {
	SPI_MODE_0 = (0 | 0),
	SPI_MODE_1 = (0 | SPI_CPHA),
	SPI_MODE_2 = (SPI_CPOL | 0),
	SPI_MODE_3 = (SPI_CPOL | SPI_CPHA)
} spi_mode;

typedef struct {
	spi_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	spi_mode	mode;
	uint8_t		chip_select;
} spi_init_param;

typedef struct {
	spi_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	spi_mode	mode;
	uint8_t		chip_select;
	XSpiPs_Config   *ps7_config;
	XSpiPs          ps7_instance;
} spi_desc;

typedef enum {
	GENERIC_GPIO,
	PS7_GPIO
} gpio_type;

typedef struct {
	gpio_type	type;
	uint32_t	id;
	uint8_t		number;
} gpio_init_param;

typedef struct {
	gpio_type	type;
	uint32_t	id;
	uint8_t		number;
	XGpioPs_Config *ps7_config;
	XGpioPs		ps7_instance;
} gpio_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the SPI communication peripheral. */
int32_t spi_init(spi_desc **desc,
		 spi_init_param param);

/* Free the resources allocated by spi_init() */
int32_t spi_remove(spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t spi_write_and_read(spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number);

/* Obtain the GPIO decriptor. */
int32_t gpio_get(gpio_desc **desc,
		 gpio_init_param param);

/* Free the resources allocated by gpio_get() */
int32_t gpio_remove(gpio_desc *desc);

/* Set the value of the specified GPIO. */
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value);

/* Generate miliseconds delay. */
void mdelay(uint32_t msecs);

#endif // PLATFORM_DRIVERS_H_

