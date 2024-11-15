/***************************************************************************//**
 *   @file   iio_lt8491.h
 *   @brief  Implementation of IIO LT8491 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __IIO_LT8491_H__
#define __IIO_LT8491_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct lt8491_iio_device {
	struct lt8491_desc *dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_active_channels;
};

struct lt8491_iio_init_param {
	struct lt8491_init_param *init_param;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int lt8491_iio_init(struct lt8491_iio_device **iio_device,
		    struct lt8491_iio_init_param *iio_init_param);

int lt8491_iio_remove(struct lt8491_iio_device *iio_device);

#endif	/* __IIO_LT8491_H__ */
