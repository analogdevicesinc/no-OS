/***************************************************************************//**
 *   @file   iio_adf4368.h
 *   @brief  Implementation of IIO ADF4368 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADF4368_H
#define IIO_ADF4368_H

#include "iio.h"

struct adf4368_iio_dev {
	struct adf4368_dev *adf4368_dev;
	struct iio_device *iio_dev;
};

struct adf4368_iio_dev_init_param {
	struct adf4368_init_param *adf4368_dev_init;
};

enum adf4368_iio_ch_attr_id {
	ADF4368_IIO_CH_ATTR_ENABLE,
	ADF4368_IIO_CH_ATTR_FREQ,
	ADF4368_IIO_CH_ATTR_OPWR,
	ADF4368_IIO_CH_ATTR_PHASE,
};

enum adf4368_iio_dev_attr_id {
	ADF4368_IIO_DEV_ATTR_BLEED,
	ADF4368_IIO_DEV_ATTR_CP,
	ADF4368_IIO_DEV_ATTR_CP_AVAIL,
	ADF4368_IIO_DEV_ATTR_DEF_REG,
	ADF4368_IIO_DEV_ATTR_REF_DIV,
	ADF4368_IIO_DEV_ATTR_REF_DB,
	ADF4368_IIO_DEV_ATTR_REF_FREQ,
	ADF4368_IIO_DEV_ATTR_SYNC_SETUP,
	ADF4368_IIO_DEV_ATTR_SW_SYNC,
	ADF4368_IIO_DEV_ATTR_TEMP,
};

int adf4368_iio_init(struct adf4368_iio_dev **iio_dev,
		     struct adf4368_iio_dev_init_param *init_param);

int adf4368_iio_remove(struct adf4368_iio_dev *desc);

#endif /* IIO_ADF4368_H */
