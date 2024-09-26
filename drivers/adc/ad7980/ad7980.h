/***************************************************************************//**
 *   @file   AD7980.h
 *   @brief  Header file of AD7980 Driver.
 *   @author Bancisor Mihai(Bancisor.Mihai@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#ifndef __AD7980_H__
#define __AD7980_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/******************************** AD7980 **************************************/
/******************************************************************************/
/* AD74XX Chip Select Pin declaration */
#define AD7980_CS_LOW           no_os_gpio_set_value(dev->gpio_cs,  \
			        NO_OS_GPIO_LOW)
#define AD7980_CS_HIGH          no_os_gpio_set_value(dev->gpio_cs,  \
			        NO_OS_GPIO_HIGH)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad7980_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_cs;
};

struct ad7980_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_cs;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the communication peripheral. */
int8_t ad7980_init(struct ad7980_dev **device,
		   struct ad7980_init_param init_param);

/*! Free the resources allocated by ad7980_init(). */
int32_t ad7980_remove(struct ad7980_dev *dev);

/*! Initiates conversion and reads data. */
uint16_t ad7980_conversion(struct ad7980_dev *dev);

/*! Converts a 16-bit raw sample to volts. */
float ad7980_convert_to_volts(uint16_t raw_sample, float v_ref);

#endif /* __AD7980_H__ */
