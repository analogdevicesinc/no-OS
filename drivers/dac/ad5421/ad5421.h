/**************************************************************************//**
*   @file   AD5421.h
*   @brief  Header file of AD5421 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
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
******************************************************************************/
#ifndef _AD5421_H_
#define _AD5421_H_

#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

/* COMMAND Bytes */
#define AD5421_CMDWRDAC         1
#define AD5421_CMDWRCTRL        2
#define AD5421_CMDWROFFSET      3
#define AD5421_CMDWRGAIN        4
#define AD5421_CMDRST			7
#define AD5421_CMDMEASVTEMP     8
#define AD5421_NO_OP			9
#define AD5421_CMDRDDAC         129
#define AD5421_CMDRDCTRL		130
#define AD5421_CMDRDOFFSET      131
#define AD5421_CMDRDGAIN        132
#define AD5421_CMDRDFAULT       133

/* AD5421 COMMAND mask */
#define AD5421_CMD(x)			((x & 0xFF) << 16)

/* AD5421 GPIO */
#define AD5421_LDAC_OUT			no_os_gpio_direction_output(dev->gpio_ldac,   \
			                NO_OS_GPIO_HIGH)
#define AD5421_LDAC_LOW			no_os_gpio_set_value(dev->gpio_ldac,          \
			                NO_OS_GPIO_LOW)
#define AD5421_LDAC_HIGH		no_os_gpio_set_value(dev->gpio_ldac,          \
			                NO_OS_GPIO_HIGH)
#define AD5421_FAULT_IN 		no_os_gpio_direction_input(dev->gpio_faultin)

/* CONTROL register bits */
#define CTRL_SPI_WATCHDOG		(1 << 12)
#define CTRL_AUTO_FAULT_RDBK    (1 << 11)
#define CTRL_SEL_ADC_INPUT      (1 << 8)
#define CTRL_ONCHIP_ADC         (1 << 7)

struct ad5421_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_ldac;
	struct no_os_gpio_desc	*gpio_faultin;
};

struct ad5421_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_ldac;
	struct no_os_gpio_init_param	gpio_faultin;
};

/* Initialize the communication with the device. */
int32_t ad5421_init(struct ad5421_dev **device,
		    struct ad5421_init_param init_param);
/* Free the resources allocated by ad5421_init(). */
int32_t ad5421_remove(struct ad5421_dev *dev);
/* Set the value of DAC register. */
void ad5421_set_dac(struct ad5421_dev *dev,
		    int32_t dac_value);
/* Set the value of OFFSET register. */
void ad5421_set_offset(struct ad5421_dev *dev,
		       int32_t offset_value);
/* Set the value of GAIN register. */
void ad5421_set_gain(struct ad5421_dev *dev,
		     int32_t gain_value);
/* Read the DAC register. */
int32_t ad5421_get_dac(struct ad5421_dev *dev);
/* Read OFFSET register. */
int32_t ad5421_get_offset(struct ad5421_dev *dev);
/* Read GAIN register. */
int32_t ad5421_get_gain(struct ad5421_dev *dev);
/* Read FAULT register. */
int32_t ad5421_get_fault(struct ad5421_dev *dev);
/* Read the temperature from Fault register. */
int32_t ad5421_get_temp(struct ad5421_dev *dev);
/* Read VLoop-COM from Fault register. */
float ad5421_get_vloop(struct ad5421_dev *dev);
/* Send command via SPI. */
int32_t ad5421_set(struct ad5421_dev *dev,
		   int32_t *i_value);
/* Receive value via SPI. */
int32_t ad5421_get(struct ad5421_dev *dev);
/* Resets the AD5421 device. */
void ad5421_reset(struct ad5421_dev *dev);

#endif /* _AD5421_H_ */
