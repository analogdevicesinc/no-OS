/***************************************************************************//**
 *   @file   iio_adf4382.h
 *   @brief  Implementation of IIO ADF4382 Driver.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_ADF4382_H
#define IIO_ADF4382_H

#include "iio.h"

struct adf4382_iio_dev {
	struct adf4382_dev *adf4382_dev;
	struct iio_device *iio_dev;
};

struct adf4382_iio_dev_init_param {
	struct adf4382_init_param *adf4382_dev_init;
};

int adf4382_iio_init(struct adf4382_iio_dev **iio_dev,
		     struct adf4382_iio_dev_init_param *init_param);

int adf4382_iio_remove(struct adf4382_iio_dev *desc);

#endif
