/***************************************************************************//**
 *   @file   iio_ltc2378.h
 *   @brief  Header file of the LTC2378 IIO Driver
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef IIO_LTC2378_H
#define IIO_LTC2378_H

#include "iio.h"
#include "ltc2378.h"

struct ltc2378_iio_desc {
	struct ltc2378_dev *ltc2378_dev;
	struct iio_device *iio_dev;
};

struct ltc2378_iio_desc_init_param {
	const struct ltc2378_init_param *ltc2378_init_param;
};

/* Initialize the LTC2378 IIO descriptor */
int ltc2378_iio_init(struct ltc2378_iio_desc **,
		     struct ltc2378_iio_desc_init_param *);

/* Free resources allocated by the IIO initialization function */
int ltc2378_iio_remove(struct ltc2378_iio_desc *);

#endif /* IIO_LTC2378_H */
