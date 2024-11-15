/***************************************************************************//**
 *   @file   iio_max22190.h
 *   @brief  Header file of MAX22190 IIO Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_MAX22190_H
#define IIO_MAX22190_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "iio_types.h"
#include "max22190.h"

enum max22190_iio_attr {
	MAX22190_IIO_FAULT1,
	MAX22190_IIO_FAULT2,
};

/**
 * @brief MAX22190 IIO descriptor
*/
struct max22190_iio_desc {
	struct max22190_desc *max22190_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief MAX22190 IIO initalization parameter
*/
struct max22190_iio_desc_init_param {
	struct max22190_init_param *max22190_init_param;
};

/** Initializes the MAX22190 IIO descriptor. */
int max22190_iio_init(struct max22190_iio_desc **,
		      struct max22190_iio_desc_init_param *);

/** Free resources allocated by the initalization function. */
int max22190_iio_remove(struct max22190_iio_desc *);

#endif /* IIO_MAX22190_H */
