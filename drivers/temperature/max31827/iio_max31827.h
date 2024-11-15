/***************************************************************************//**
 *   @file   iio_max31827.h
 *   @brief  Implementation of IIO MAX31827 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __IIO_MAX31827_H__
#define __IIO_MAX31827_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct max31827_iio_device {
	struct max31827_device *dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_active_channels;
};

struct max31827_iio_init_param {
	struct max31827_init_param *init_param;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int max31827_iio_init(struct max31827_iio_device **iio_device,
		      struct max31827_iio_init_param *iio_init_param);

int max31827_iio_remove(struct max31827_iio_device *iio_device);

#endif	/* __IIO_MAX31827_H__ */
