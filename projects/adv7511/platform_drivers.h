/***************************************************************************//**
 *   @file   platform_drivers.h
 *   @brief  Header file of Xilinx Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include <stdint.h>
#include <stdio.h>
#include <xparameters.h>
#include "config.h"
#include <xil_cache.h>
#include <xiic.h>
#include <xil_exception.h>
#include <xil_io.h>
#ifdef _XPARAMETERS_PS_H_
#include <xspips.h>
#include <xgpiops.h>
#else
#include <xspi.h>
#include <xgpio.h>
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SUCCESS		0
#define FAILURE		-1

#define	SPI_CPHA	0x01
#define	SPI_CPOL	0x02

#define SPI_CS_DECODE	0x01

#define GPIO_OUT	0x01
#define GPIO_IN		0x00

#define GPIO_HIGH	0x01
#define GPIO_LOW	0x00

#define ad_reg_write(x,y) Xil_Out32(x,y)
#define ad_reg_read(x) Xil_In32(x)
#define ad_printf printf

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum i2c_type {
	XILINX_I2C
} i2c_type;

typedef struct i2c_init_param {
	enum i2c_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t		slave_address;
} i2c_init_param;

typedef struct i2c_desc {
	enum i2c_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	uint8_t		slave_address;
	XIic_Config *config;
	XIic instance;
} i2c_desc;

typedef enum spi_type {
	XILINX_SPI
} spi_type;

typedef enum spi_mode {
	SPI_MODE_0 = (0 | 0),
	SPI_MODE_1 = (0 | SPI_CPHA),
	SPI_MODE_2 = (SPI_CPOL | 0),
	SPI_MODE_3 = (SPI_CPOL | SPI_CPHA)
} spi_mode;

typedef struct spi_init_param {
	enum spi_type	type;
	uint32_t	id;
	uint32_t	max_speed_hz;
	enum spi_mode	mode;
	uint8_t		chip_select;
	uint32_t	flags;
} spi_init_param;

typedef struct spi_desc {
	enum spi_type	type;
	uint32_t		id;
	uint32_t		max_speed_hz;
	enum spi_mode	mode;
	uint8_t			chip_select;
	uint32_t		flags;
#ifdef _XPARAMETERS_PS_H_
	XSpiPs_Config	*config;
	XSpiPs			instance;
#else
	XSpi			instance;
#endif
} spi_desc;

typedef enum gpio_type {
	XILINX_GPIO
} gpio_type;

typedef struct gpio_desc {
	enum gpio_type	type;
	uint32_t		id;
	uint8_t			number;
#ifdef _XPARAMETERS_PS_H_
	XGpioPs_Config	*config;
	XGpioPs			instance;
#else
	XGpio			instance;
#endif
} gpio_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the I2C communication peripheral. */
int32_t i2c_init(struct i2c_desc **desc,
		 const struct i2c_init_param *param);

/* Free the resources allocated by i2c_init(). */
int32_t i2c_remove(struct i2c_desc *desc);

/* Write data to a slave device. */
int32_t i2c_write(struct i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit);

/* Read data from a slave device. */
int32_t i2c_read(struct i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit);

/* Set the slave device address that the master must call. */
int32_t i2c_set_address(struct i2c_desc *dev, uint8_t new_addr);

/* Initialize the SPI communication peripheral. */
int32_t spi_init(struct spi_desc **desc,
		 const struct spi_init_param *param);

/* Free the resources allocated by spi_init() */
int32_t spi_remove(struct spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number);

/* Obtain the GPIO decriptor. */
int32_t gpio_get(struct gpio_desc **desc,
		 uint8_t gpio_number);

/* Free the resources allocated by gpio_get() */
int32_t gpio_remove(struct gpio_desc *desc);

/* Enable the input direction of the specified GPIO. */
int32_t gpio_direction_input(struct gpio_desc *desc);

/* Enable the output direction of the specified GPIO. */
int32_t gpio_direction_output(struct gpio_desc *desc,
			      uint8_t value);

/* Get the direction of the specified GPIO. */
int32_t gpio_get_direction(struct gpio_desc *desc,
			   uint8_t *direction);

/* Set the value of the specified GPIO. */
int32_t gpio_set_value(struct gpio_desc *desc,
		       uint8_t value);

/* Get the value of the specified GPIO. */
int32_t gpio_get_value(struct gpio_desc *desc,
		       uint8_t *value);

/* Generate microseconds delay. */
void udelay(uint32_t usecs);

/* Generate miliseconds delay. */
void mdelay(uint32_t msecs);

#endif // PLATFORM_DRIVERS_H_
