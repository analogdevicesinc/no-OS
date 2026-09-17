/***************************************************************************//**
 *   @file   iio_ad8460.h
 *   @brief  Implementation of IIO AD8460 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __IIO_AD8460_H__
#define __IIO_AD8460_H__

#include "iio.h"

struct ad8460_iio_device {
	struct ad8460_device *dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_active_channels;
};

struct ad8460_iio_init_param {
	struct ad8460_init_param *init_param;
};

int ad8460_iio_init(struct ad8460_iio_device **iio_device,
		    struct ad8460_iio_init_param *iio_init_param);

int ad8460_iio_remove(struct ad8460_iio_device *iio_device);

#endif	/* __IIO_AD8460_H__ */
