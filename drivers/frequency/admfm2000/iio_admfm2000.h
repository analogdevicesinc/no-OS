/***************************************************************************//**
 *   @file   iio_admfm2000.h
 *   @brief  Header file for admfm2000 IIO Driver.
 *   @author Ramona Nechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef SRC_IIO_ADMFM2000_H_
#define SRC_IIO_ADMFM2000_H_

#include <stdint.h>
#include "admfm2000.h"
#include "iio.h"

struct admfm2000_iio_dev {
	struct admfm2000_dev *admfm2000_dev;
	struct iio_device *iio_dev;
};

struct admfm2000_iio_dev_init_param {
	struct admfm2000_init_param *admfm2000_dev_init;
};

int admfm2000_iio_init(struct admfm2000_iio_dev **iio_dev,
		       struct admfm2000_iio_dev_init_param *init_param);

int admfm2000_iio_remove(struct admfm2000_iio_dev *desc);

#endif /* SRC_IIO_ADMFM2000_H_ */
