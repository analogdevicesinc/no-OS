/***************************************************************************//**
 *   @file   iio_max22196.h
 *   @brief  Header file of IIO MAX22196 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_MAX22196_H
#define IIO_MAX22196_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "max22196.h"

#define MAX22196_GLOBAL_CLRFILTR	3

enum max22196_iio_fault {
	MAX22196_IIO_FAULT1,
	MAX22196_IIO_FAULT2
};

/**
 * @brief MAX22196 IIO descriptor
*/
struct max22196_iio_desc {
	struct max22196_desc *max22196_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief MAX22196 IIO initalization parameter
*/
struct max22196_iio_desc_init_param {
	struct max22196_init_param *max22196_init_param;
};

/** Initializes the MAX22196 IIO descriptor. */
int max22196_iio_init(struct max22196_iio_desc **,
		      struct max22196_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int max22196_iio_remove(struct max22196_iio_desc *);

#endif /* IIO_MAX22196_H */
