/***************************************************************************//**
*   @file   iio_lt3074.h
*   @brief  Header file for the LT3074 IIO Driver
*   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_LT3074_H
#define IIO_LT3074_H

#include <stdbool.h>
#include "iio.h"
#include "lt3074.h"

/**
 * @brief Structure holding the LT3074 IIO device descriptor
*/
struct lt3074_iio_desc {
	struct lt3074_dev *lt3074_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LT3074 IIO initialization parameter.
*/
struct lt3074_iio_desc_init_param {
	struct lt3074_init_param *lt3074_init_param;
};

/** Initializes the LT3074 IIO descriptor. */
int lt3074_iio_init(struct lt3074_iio_desc **,
		    struct lt3074_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int lt3074_iio_remove(struct lt3074_iio_desc *);

#endif /* IIO_LT3074_H */
