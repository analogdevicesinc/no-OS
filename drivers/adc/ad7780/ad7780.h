/***************************************************************************//**
*   @file   AD7780.h
*   @brief  AD7780 header file.
*   @author DNechita (dan.nechita@analog.com)
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
#ifndef __AD7780_H__
#define __AD7780_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/************************** AD7780 Definitions ********************************/
/******************************************************************************/

/* DOUT/RDY pin */
#define AD7780_RDY_STATE(value) no_os_gpio_get_value(dev->gpio_miso,             \
		                &value)

/* PDRST pin */
#define AD7780_PDRST_PIN_OUT    no_os_gpio_direction_output(dev->gpio_pdrst,     \
					              NO_OS_GPIO_HIGH);
#define AD7780_PDRST_HIGH       no_os_gpio_set_value(dev->gpio_pdrst,            \
			        NO_OS_GPIO_HIGH)
#define AD7780_PDRST_LOW        no_os_gpio_set_value(dev->gpio_pdrst,            \
			        NO_OS_GPIO_LOW)

/* FILTER pin */
#define AD7780_FILTER_PIN_OUT   no_os_gpio_direction_output(dev->gpio_filter,    \
					              NO_OS_GPIO_HIGH);
#define AD7780_FILTER_HIGH      no_os_gpio_set_value(dev->gpio_filter,           \
			        NO_OS_GPIO_HIGH)
#define AD7780_FILTER_LOW       no_os_gpio_set_value(dev->gpio_filter,           \
			        NO_OS_GPIO_LOW)

/* GAIN pin */
#define AD7780_GAIN_PIN_OUT     no_os_gpio_direction_output(dev->gpio_gain,      \
					              NO_OS_GPIO_HIGH);
#define AD7780_GAIN_HIGH        no_os_gpio_set_value(dev->gpio_gain,             \
			        NO_OS_GPIO_HIGH)
#define AD7780_GAIN_LOW         no_os_gpio_set_value(dev->gpio_gain,             \
			        NO_OS_GPIO_LOW)

/* Status bits */
#define AD7780_STAT_RDY         (1 << 7) // Ready bit.
#define AD7780_STAT_FILTER      (1 << 6) // Filter bit.
#define AD7780_STAT_ERR         (1 << 5) // Error bit.
#define AD7780_STAT_ID1         (1 << 4) // ID bits.
#define AD7780_STAT_ID0         (1 << 3) // ID bits.
#define AD7780_STAT_GAIN        (1 << 2) // Gain bit.
#define AD7780_STAT_PAT1        (1 << 1) // Status pattern bits.
#define AD7780_STAT_PAT0        (1 << 0) // Status pattern bits.

#define AD7780_ID_NUMBER        0x08

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad7780_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_pdrst;
	struct no_os_gpio_desc	*gpio_miso;
	struct no_os_gpio_desc	*gpio_filter;
	struct no_os_gpio_desc	*gpio_gain;
};

struct ad7780_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_pdrst;
	struct no_os_gpio_init_param	gpio_miso;
	struct no_os_gpio_init_param	gpio_filter;
	struct no_os_gpio_init_param	gpio_gain;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the communication peripheral and checks if the device is
present. */
int8_t ad7780_init(struct ad7780_dev **device,
		   struct ad7780_init_param init_param);

/*! Free the resources allocated by ad7780_init(). */
int32_t ad7780_remove(struct ad7780_dev *dev);

/*! Waits for DOUT/RDY pin to go low. */
int8_t ad7780_wait_rdy_go_low(struct ad7780_dev *dev);

/*! Reads a 24-bit sample from the ADC. */
int32_t ad7780_read_sample(struct ad7780_dev *dev,
			   uint8_t* p_status);

/*! Converts the 24-bit raw value to volts. */
float ad7780_convert_to_voltage(uint32_t raw_sample,
				float v_ref,
				uint8_t gain);

#endif /* __AD7780_H__ */
