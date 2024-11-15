/***************************************************************************//**
*   @file   AD7780.h
*   @brief  AD7780 header file.
*   @author DNechita (dan.nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
