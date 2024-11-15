/***************************************************************************//**
*   @file   iio_ad413x.h
*   @brief  Header file of iio_ad413x
*   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
* Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_AD413X_H
#define IIO_AD413X_H

#include "iio.h"
#include "ad413x.h"

struct ad413x_iio_dev {
	struct ad413x_dev *ad413x_dev;
	struct iio_device *iio_dev;
};

struct ad413x_iio_init_param {
	struct ad413x_init_param ad413x_ip;
};

int32_t ad413x_iio_init(struct ad413x_iio_dev **iio_dev,
			struct ad413x_iio_init_param init_param);
int32_t ad413x_iio_remove(struct ad413x_iio_dev *desc);

#endif /** IIO_AD413X_H */
