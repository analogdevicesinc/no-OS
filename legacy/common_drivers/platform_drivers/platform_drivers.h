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
#include <stdint.h>
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SUCCESS		0
#define FAILURE		-1

#define GPIO_OUT	0x01
#define GPIO_IN		0x00

#define GPIO_HIGH	0x01
#define GPIO_LOW	0x00

#ifdef ALTERA
#include <io.h>
#include <unistd.h>
#include <stdio.h>
#include <system.h>
#include <alt_stdio.h>
#include <alt_cache.h>
#include <alt_types.h>
#endif

#ifdef NIOS_II
#endif

#ifdef XILINX
#include <xparameters.h>
#include <xil_printf.h>
//#include "platform.h"
#endif

#ifdef ZYNQ
#include <sleep.h>
#include <xspips.h>
#include <xuartps.h>
#endif

#ifdef MICROBLAZE
#include <xil_io.h>
#include <xil_cache.h>
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

// data types using stdint-

#ifdef ALTERA
#define int8_t alt_8
#define int32_t alt_32
#define uint8_t alt_u8
#define uint16_t alt_u16
#define uint32_t alt_u32
#define uint64_t alt_u64
#endif

// sleep functions

#define mdelay(msecs) usleep(1000*msecs)
#define udelay(usecs) usleep(usecs)

#ifdef MICROBLAZE
void usleep(uint32_t us_count);
#endif

// print functions

#ifdef ALTERA
#define ad_printf printf
#endif

#ifdef XILINX
#define ad_printf xil_printf
#endif

// io read/write

#ifdef ALTERA
#define ad_reg_write(x,y) IOWR_32DIRECT(x,0,y)
#define ad_reg_read(x) IORD_32DIRECT(x,0)
#endif

#ifdef XILINX
#define ad_reg_write(x,y) Xil_Out32(x,y)
#define ad_reg_read(x) Xil_In32(x)
#endif

// cache functions

#ifdef ALTERA
#define ad_icache_flush alt_icache_flush_all
#define ad_dcache_flush alt_icache_flush_all
#endif

#ifdef XILINX
#define ad_icache_flush Xil_ICacheFlush
#define ad_dcache_flush Xil_DCacheFlush
#endif

#ifdef ZYNQ
void Xil_ICacheEnable();
void Xil_ICacheDisable();
void Xil_DCacheEnable();
void Xil_DCacheDisable();
void Xil_DCacheFlush();
#endif

// platform functions

#ifdef XILINX
#define ad_platform_init init_platform
#define ad_platform_close cleanup_platform
#endif

#ifdef ALTERA
void ad_platform_init(void);
void ad_platform_close(void);
#endif

// common macros

#define min(x, y) 			(((x) < (y)) ? (x) : (y))
#define min_t(type, x, y) 		(type)min((type)(x), (type)(y))
#define max(x, y) 			(((x) > (y)) ? (x) : (y))
#define max_t(type, x, y) 		(type)max((type)(x), (type)(y))
#define clamp(val, min_val, max_val) 	(max(min((val), (max_val)), (min_val)))
#define clamp_t(type, val, min_val, max_val)\
		(type)clamp((type)(val), (type)(min_val), (type)(max_val))
#define DIV_ROUND_UP(n,d) 		(((n) + (d) - 1) / (d))
#define DIV_ROUND_CLOSEST(x, y)		(uint32_t)(((double)(x) / (y)) + 0.5)
#define DIV_ROUND_CLOSEST_ull(x, y)	(uint64_t)(((double)(x) / (y)) + 0.5)
#define ARRAY_SIZE(ar)			(sizeof(ar)/sizeof(ar[0]))
#define BIT(nr)				(1 << (nr))
#define ROUND_UP(x,y)			(((x)+(y)-1)/(y))
#define ROUND_CLOSEST(x,y)		(((x)+(y))/(y) + 0.5)

#ifdef ALTERA
#define LONG_MAX			(~0L)
#define ULONG_MAX			(~0UL)
#define FALSE				0
#define TRUE				1
#endif

#define SPI_CHIP_SELECT(x) (~(1 << (x)))

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
	ZYNQ_PS7_SPI,
	ZYNQ_PSU_SPI,
	MICROBLAZE_SPI,
	NIOS_II_SPI
} spi_type;

typedef struct {
	spi_type	type;
	uint8_t     chip_select;
	uint32_t    cpha;
	uint32_t    cpol;
} spi_init_param;

typedef struct {
	spi_type	type;
	uint32_t	id;
	uint32_t    base_address;
	uint32_t    device_id;
	uint8_t     chip_select;
	uint32_t    cpha;
	uint32_t    cpol;
} spi_desc;

typedef enum {
	ZYNQ_PS7_GPIO,
	ZYNQ_PSU_GPIO,
	MICROBLAZE_GPIO,
	NIOS_II_GPIO
} gpio_type;

typedef struct {
	gpio_type	type;
	uint32_t	id;
	uint8_t		number;
} gpio_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the I2C communication peripheral. */
int32_t i2c_init(i2c_desc **desc,
		 const i2c_init_param *param);

/* Free the resources allocated by i2c_init(). */
int32_t i2c_remove(i2c_desc *desc);

/* Write data to a slave device. */
int32_t i2c_write(i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit);

/* Read data from a slave device. */
int32_t i2c_read(i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit);

/* Initialize the SPI communication peripheral. */
int32_t spi_init(spi_desc **desc,
		 const spi_init_param *param);

/* Free the resources allocated by spi_init() */
int32_t spi_remove(spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t spi_write_and_read(spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number);

/* Obtain the GPIO decriptor. */
int32_t gpio_get(gpio_desc **desc,
		 uint8_t gpio_number);

/* Free the resources allocated by gpio_get() */
int32_t gpio_remove(gpio_desc *desc);

/* Enable the input direction of the specified GPIO. */
int32_t gpio_direction_input(gpio_desc *desc);

/* Enable the output direction of the specified GPIO. */
int32_t gpio_direction_output(gpio_desc *desc,
			      uint8_t value);

/* Get the direction of the specified GPIO. */
int32_t gpio_get_direction(gpio_desc *desc,
			   uint8_t *direction);

/* Set the value of the specified GPIO. */
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value);

/* Get the value of the specified GPIO. */
int32_t gpio_get_value(gpio_desc *desc,
		       uint8_t *value);

int32_t ad_gpio_set_range(uint8_t start_pin, uint8_t num_pins, uint8_t data);
int32_t ad_gpio_get_range(uint8_t start_pin, uint8_t num_pins, uint32_t *data);

/******************************************************************************/
/********************* MISC structure and functions ***************************/
/******************************************************************************/

uint64_t do_div(uint64_t* n, uint64_t base);
void ad_reg_write_16(uint32_t addr, uint32_t data);
uint8_t ad_uart_read();
uint32_t ad_pow2(uint32_t number);

/* Generate miliseconds delay. */
//void mdelay(uint32_t msecs);

#endif // PLATFORM_DRIVERS_H_
