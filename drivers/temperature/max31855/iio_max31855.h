/***************************************************************************//**
 *   @file   iio_max31855.h
 *   @brief  Header file of IIO MAX31855 driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_MAX31855_H
#define IIO_MAX31855_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct max31855_iio_dev {
	struct max31855_dev *max31855_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

struct max31855_iio_dev_init_param {
	struct max31855_init_param *max31855_dev_init;
};

int max31855_iio_init(struct max31855_iio_dev **,
		      struct max31855_iio_dev_init_param *);
int max31855_iio_remove(struct max31855_iio_dev *);

#endif
