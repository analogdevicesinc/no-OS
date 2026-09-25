/***************************************************************************//**
 *   @file   iio_lt7170.h
 *   @brief  Header file for the LT7170 IIO Driver
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_LT7170_H
#define IIO_LT7170_H

#include <stdbool.h>
#include "iio.h"
#include "lt7170.h"

/**
 * @brief Structure holding the LT7170 IIO device descriptor
*/
struct lt7170_iio_desc {
	struct lt7170_dev *lt7170_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LT7170 IIO initialization parameter.
*/
struct lt7170_iio_desc_init_param {
	struct lt7170_init_param *lt7170_init_param;
};

/** Initializes the LT7170 IIO descriptor. */
int lt7170_iio_init(struct lt7170_iio_desc **,
		    struct lt7170_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int lt7170_iio_remove(struct lt7170_iio_desc *);

#endif /* IIO_LT7170_H */
