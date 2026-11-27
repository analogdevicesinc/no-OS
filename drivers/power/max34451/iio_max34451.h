/***************************************************************************//**
 *   @file   iio_max34451.h
 *   @brief  Header file for the MAX34451 IIO Driver
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __IIO_MAX34451_H__
#define __IIO_MAX34451_H__

#include <stdbool.h>
#include "iio.h"
#include "max34451.h"

/**
 * @brief Structure holding the MAX34451 IIO device descriptor
*/
struct max34451_iio_desc {
	struct max34451_dev *max34451_dev;
	struct iio_device *iio_dev;
	/* Page targeted by the debug register interface, independent of the
	 * driver's cache of the page currently selected on the device. */
	int page;
};

/**
 * @brief Structure holding the MAX34451 IIO initialization parameter.
*/
struct max34451_iio_desc_init_param {
	struct max34451_init_param *max34451_init_param;
};

/** Initializes the MAX34451 IIO descriptor. */
int max34451_iio_init(struct max34451_iio_desc **,
		      struct max34451_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int max34451_iio_remove(struct max34451_iio_desc *);

#endif /* __IIO_MAX34451_H__ */
