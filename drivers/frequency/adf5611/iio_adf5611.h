/***************************************************************************//**
 *   @file   iio_adf5611.h
 *   @brief  Implementation of IIO ADF5611 Driver.
 *   @author josemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADF5611_H
#define IIO_ADF5611_H

#include "iio_types.h"
#include "iio.h"

struct adf5611_iio_dev {
	struct adf5611_dev *adf5611_dev;
	struct iio_device *iio_dev;
};

struct adf5611_iio_dev_init_param {
	struct adf5611_init_param *adf5611_dev_init;
};

enum adf5611_iio_ch_attr_id {
	ADF5611_IIO_CH_ATTR_FREQ,
	ADF5611_IIO_CH_ATTR_OPWR,
};

enum adf5611_iio_dev_attr_id {
	ADF5611_IIO_DEV_ATTR_REF_CLK,
	ADF5611_IIO_DEV_ATTR_REF_DIV,
	ADF5611_IIO_DEV_ATTR_CP_I,
	ADF5611_IIO_DEV_ATTR_CP_AVAIL,
	ADF5611_IIO_DEV_ATTR_RFOUTDIV_PWR,
	ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV,
	ADF5611_IIO_DEV_ATTR_EN_RFOUTDIV,
	ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV_AVAIL,
};

int adf5611_iio_init(struct adf5611_iio_dev **iio_dev,
		     struct adf5611_iio_dev_init_param *init_param);

int adf5611_iio_remove(struct adf5611_iio_dev *desc);

#endif
