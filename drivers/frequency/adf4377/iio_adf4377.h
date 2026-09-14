/***************************************************************************//**
 *   @file   iio_adf4377.h
 *   @brief  Implementation of IIO ADF4377 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADF4377_H
#define IIO_ADF4377_H

#include "iio_types.h"
#include "iio.h"

struct adf4377_iio_dev {
	struct adf4377_dev *adf4377_dev;
	struct iio_device *iio_dev;
};

struct adf4377_iio_dev_init_param {
	struct adf4377_init_param *adf4377_dev_init;
};

enum adf4377_iio_ch_attr_id {
	ADF4377_IIO_CH_ATTR_FREQ,
	ADF4377_IIO_CH_ATTR_OPWR,
	ADF4377_IIO_CH_ATTR_EN,
	ADF4377_IIO_DEV_ATTR_CLK_INV,
};

enum adf4377_iio_dev_attr_id {
	ADF4377_IIO_DEV_ATTR_REF_CLK,
	ADF4377_IIO_DEV_ATTR_REF_DIV,
	ADF4377_IIO_DEV_ATTR_RFOUT_DIV,
	ADF4377_IIO_DEV_ATTR_RFOUT_DIV_AVAIL,
	ADF4377_IIO_DEV_ATTR_CP_I,
	ADF4377_IIO_DEV_ATTR_CP_AVAIL,
	ADF4377_IIO_DEV_ATTR_BLEED_CURRENT,
	ADF4377_IIO_DEV_ATTR_BLEED_DELAY,
	ADF4377_IIO_DEV_ATTR_REF_DOUBLER_EN,
	ADF4377_IIO_DEV_ATTR_NDEL,
	ADF4377_IIO_DEV_ATTR_RDEL,
	ADF4377_IIO_DEV_ATTR_SR_DEL_ADJ,
	ADF4377_IIO_DEV_ATTR_SR_INV_ADJ,
	ADF4377_IIO_DEV_ATTR_SR_MONITORING,
	ADF4377_IIO_DEV_ATTR_DEFAULT_REG,
};

int adf4377_iio_init(struct adf4377_iio_dev **iio_dev,
		     struct adf4377_iio_dev_init_param *init_param);

int adf4377_iio_remove(struct adf4377_iio_dev *desc);

#endif
