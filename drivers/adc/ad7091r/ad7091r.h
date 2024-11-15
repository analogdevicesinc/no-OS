/***************************************************************************//**
 *   @file   AD7091R.h
 *   @brief  Header file of AD7091R Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AD7091R_H__
#define __AD7091R_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad7091r_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

struct ad7091r_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the SPI communication peripheral. */
int8_t ad7091r_init(struct ad7091r_dev **device,
		    struct ad7091r_init_param init_param);

/*! Free the resources allocated by ad7091r_init(). */
int32_t ad5686_remove(struct ad7091r_dev *dev);

/*! Initiate a software reset of the device. */
void ad7091r_software_reset(struct ad7091r_dev *dev);

/*! Initiates one conversion and reads back the result. */
uint16_t ad7091r_read_sample(struct ad7091r_dev *dev);

/*! Puts the device in power-down mode. */
void ad7091r_power_down(struct ad7091r_dev *dev);

/*! Powers up the device. */
void ad7091r_power_up(struct ad7091r_dev *dev);

/*! Converts a 12-bit raw sample to volts. */
float ad7091r_convert_to_volts(int16_t raw_sample, float v_ref);

#endif /* __AD7091R_H__ */
