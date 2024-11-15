/***************************************************************************//**
 *   @file   iio_max14916.h
 *   @brief  Header file of IIO MAX14916 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_MAX14916_H
#define IIO_MAX14916_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "iio_types.h"
#include "max14916.h"

/**
 * @brief MAX14916 specific IIO descriptor.
*/
struct max14916_iio_desc {
	struct max149x6_desc *max14916_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Initialization parameter for the MAX14916 IIO descriptor.
*/
struct max14916_iio_desc_init_param {
	struct max149x6_init_param *max14916_init_param;
	bool ch_enable[MAX14916_CHANNELS];
};

/** Initializes the MAX14916 IIO descriptor */
int max14916_iio_init(struct max14916_iio_desc **,
		      struct max14916_iio_desc_init_param *);

/** Free resources allocated by the init function. */
int max14916_iio_remove(struct max14916_iio_desc *);

#endif /* IIO_MAX14916_H */
