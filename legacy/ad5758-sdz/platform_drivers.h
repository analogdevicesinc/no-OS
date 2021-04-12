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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <sleep.h>
#include <xgpiops.h>
#include <xspips.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define mdelay(msecs)	usleep(1000 * msecs)

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

enum spi_type {
	GENERIC_SPI,
	PS7_SPI
};

enum spi_mode {
	SPI_MODE_0 = (0 | 0),
	SPI_MODE_1 = (0 | SPI_CPHA),
	SPI_MODE_2 = (SPI_CPOL | 0),
	SPI_MODE_3 = (SPI_CPOL | SPI_CPHA)
};

struct spi_init_param {
	enum spi_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	enum spi_mode	mode;
	uint8_t		chip_select;
};

struct spi_desc {
	enum spi_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	enum spi_mode	mode;
	uint8_t		chip_select;
	XSpiPs_Config   *ps7_config;
	XSpiPs          ps7_instance;
};

enum gpio_type {
	GENERIC_GPIO,
	PS7_GPIO
};

struct gpio_init_param {
	enum gpio_type	type;
	uint32_t	id;
	uint8_t		number;
};

struct gpio_desc {
	enum gpio_type	type;
	uint32_t	id;
	uint8_t		number;
	XGpioPs_Config *ps7_config;
	XGpioPs		ps7_instance;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the SPI communication peripheral. */
int32_t spi_init(struct spi_desc **desc,
		 struct spi_init_param *param);

/* Free the resources allocated by spi_init() */
int32_t spi_remove(struct spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number);

/* Obtain the GPIO decriptor. */
int32_t gpio_get(struct gpio_desc **desc,
		 struct gpio_init_param param);

/* Set the value of the specified GPIO. */
int32_t gpio_set_value(struct gpio_desc *desc,
		       uint8_t value);

/* Free the resources allocated by gpio_get() */
int32_t gpio_remove(struct gpio_desc *desc);

#endif /* PLATFORM_DRIVERS_H_ */
