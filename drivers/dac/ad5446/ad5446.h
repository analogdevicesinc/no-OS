/***************************************************************************//**
*   @file   AD5446.h
*   @brief  Header file of AD5446 Driver. This driver supporting the following
*              devices: AD5553, AD5543, AD5542A, AD5541A, AD5512A, AD5453,
*                       AD5452, AD5451, AD5450, AD5446, AD5444
*
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
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
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Custom type for active clock edge */
enum active_clk_t {
	negedge,
	posedge
};

/* Custom boolean type */
enum bool_t {
	false,
	true
};

/* Data structure for chip's attributes */
struct ad5446_chip_info {
	uint8_t resolution;
	enum active_clk_t  data_clock_in;
	enum bool_t        has_ctrl;
};

/* Supported output types */
enum vout_type_t {
	unipolar,       /* 0 .. Vref */
	unipolar_inv,   /* 0 .. -Vref */
	bipolar         /* -Vref .. Vref*/
};

/* Supported devices */
enum ad5446_type_t {
	ID_AD5600,
	ID_AD5553,
	ID_AD5543,
	ID_AD5542A,
	ID_AD5541A,
	ID_AD5512A,
	ID_AD5453,
	ID_AD5452,
	ID_AD5451,
	ID_AD5450,
	ID_AD5446,
	ID_AD5444,
};

struct ad5446_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_ladc;
	struct no_os_gpio_desc	*gpio_clrout;
	/* Device Settings */
	enum ad5446_type_t act_device;
};

struct ad5446_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_ladc;
	struct no_os_gpio_init_param	gpio_clrout;
	/* Device Settings */
	enum ad5446_type_t act_device;
};

/* Control Bits */
#define AD5446_CTRL_LOAD_UPDATE     0x0
#define AD5446_CTRL_ACTIVE_POSEDGE  0x3

/* AD5446 GPIO */
#define AD5446_LDAC_OUT             no_os_gpio_direction_output(dev->gpio_ladc,   \
			            NO_OS_GPIO_HIGH)
#define AD5446_LDAC_LOW             no_os_gpio_set_value(dev->gpio_ladc,          \
			            NO_OS_GPIO_LOW)
#define AD5446_LDAC_HIGH            no_os_gpio_set_value(dev->gpio_ladc,          \
			            NO_OS_GPIO_HIGH)

#define AD5446_CLR_OUT              no_os_gpio_direction_output(dev->gpio_clrout, \
			            NO_OS_GPIO_HIGH)
#define AD5446_CLR_LOW              no_os_gpio_set_value(dev->gpio_clrout,        \
			            NO_OS_GPIO_LOW)
#define AD5446_CLR_HIGH             no_os_gpio_set_value(dev->gpio_clrout,        \
			            NO_OS_GPIO_HIGH)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize SPI and Initial Values for AD5446 Board. */
int8_t ad5446_init(struct ad5446_dev **device,
		   struct ad5446_init_param init_param);

/* Free the resources allocated by ad5446_init(). */
int32_t ad5446_remove(struct ad5446_dev *dev);

/* Write to shift register via SPI. */
void ad5446_set_register(struct ad5446_dev *dev,
			 uint8_t command,
			 uint16_t data);

/* Sets the output voltage. */
float ad5446_set_voltage(struct ad5446_dev *dev,
			 float voltage,
			 float vref,
			 enum vout_type_t vout_type);
