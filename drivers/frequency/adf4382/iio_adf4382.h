/***************************************************************************//**
 *   @file   iio_adf4382.h
 *   @brief  Implementation of IIO ADF4382 Driver.
 *   @author Ciprian Hegbeli (ciprian.hegbeli@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
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

enum adf4382_iio_ch_attr_id {
	ADF4382_IIO_CH_ATTR_EN,
	ADF4382_IIO_CH_ATTR_FREQ,
	ADF4382_IIO_CH_ATTR_OPWR,
	ADF4382_IIO_CH_ATTR_PHASE_ADJ,
};

enum adf4382_iio_dev_attr_id {
	ADF4382_IIO_DEV_ATTR_REF_FREQ,
	ADF4382_IIO_DEV_ATTR_BLEED_CURRENT,
	ADF4382_IIO_DEV_ATTR_CHARGE_PUMP_CURRENT,
	ADF4382_IIO_DEV_ATTR_CHARGE_PUMP_AVAILABLE,
	ADF4382_IIO_DEV_ATTR_REF_DIVIDER,
	ADF4382_IIO_DEV_ATTR_REF_DOUBLER_EN,
	ADF4382_IIO_DEV_ATTR_SW_SYNC,
	ADF4382_IIO_DEV_ATTR_EZ_SYNC,
	ADF4382_IIO_DEV_ATTR_FASTCAL_EN,
	ADF4382_IIO_DEV_ATTR_TIMED_SYNC,
	ADF4382_IIO_DEV_ATTR_FASTCAL_LUT_EN,
	ADF4382_IIO_DEV_ATTR_FASTCAL_CHANGE_FREQ,
	ADF4382_IIO_DEV_ATTR_FASTCAL_START_CALIBRATION,
};

int adf4382_iio_init(struct adf4382_iio_dev **iio_dev,
		     struct adf4382_iio_dev_init_param *init_param);

int adf4382_iio_remove(struct adf4382_iio_dev *desc);

#endif
