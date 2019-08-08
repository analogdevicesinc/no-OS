/***************************************************************************//**
 *   @file   xil_drivers.h
 *   @brief  Header file of Xilinx Platform Drivers.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef XIL_DRIVERS_H_
#define XIL_DRIVERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xparameters.h>
#ifdef _XPARAMETERS_PS_H_
#include <xspips.h>
#include <xgpiops.h>
#include <xiic.h>
#include <xil_exception.h>
#else
#include <xspi.h>
#include <xgpio.h>
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SPI_CS_DECODE	0x01

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum i2c_type {
	XILINX_I2C
} i2c_type;

typedef struct xil_i2c_init_param {
	enum i2c_type	type;
	uint32_t	id;
} xil_i2c_init_param;

typedef struct xil_i2c_desc {
	enum i2c_type	type;
	uint32_t	id;
#ifdef _XPARAMETERS_PS_H_
	XIic_Config *config;
	XIic instance;
#else
#endif
} xil_i2c_desc;

typedef enum spi_type {
	XILINX_SPI
} spi_type;

typedef struct xil_spi_init_param {
	enum spi_type	type;
	uint32_t	id;
	uint32_t	flags;
} xil_spi_init_param;

typedef struct xil_spi_desc {
	enum spi_type	type;
	uint32_t		id;
	uint32_t		flags;
#ifdef _XPARAMETERS_PS_H_
	XSpiPs_Config	*config;
	XSpiPs			instance;
#else
	XSpi			instance;
#endif
} xil_spi_desc;

typedef enum gpio_type {
	XILINX_GPIO
} gpio_type;

typedef struct xil_gpio_desc {
	enum gpio_type	type;
	uint32_t		id;
#ifdef _XPARAMETERS_PS_H_
	XGpioPs_Config	*config;
	XGpioPs			instance;
#else
	XGpio			instance;
#endif
} xil_gpio_desc;

#endif // XIL_DRIVERS_H_


