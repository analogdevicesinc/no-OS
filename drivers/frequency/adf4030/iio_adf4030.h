/***************************************************************************//**
 *   @file   iio_adf4030.h
 *   @brief  Implementation of IIO ADF4030 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADF4030_H
#define IIO_ADF4030_H

#include "iio.h"

struct adf4030_iio_dev {
	struct adf4030_dev *adf4030_dev;
	struct iio_device *iio_dev;
};

struct adf4030_iio_dev_init_param {
	struct adf4030_init_param *adf4030_dev_init;
};

enum adf4030_iio_ch_attr_id {
	ADF4030_IIO_CH_ATTR_DIRECTION,
	ADF4030_IIO_CH_ATTR_DELAY,
	ADF4030_IIO_CH_ATTR_ODIVB_EN,
	ADF4030_IIO_CH_ATTR_TERMINATION,
	ADF4030_IIO_CH_ATTR_PRBS,
	ADF4030_IIO_CH_ATTR_INVERT,
	ADF4030_IIO_CH_ATTR_VOLTAGE,
};

enum adf4030_iio_dev_attr_id {
	ADF4030_IIO_DEV_ATTR_DEFAULT_REGS,
	ADF4030_IIO_DEV_ATTR_CHIP_ADDR,
	ADF4030_IIO_DEV_ATTR_REF_FREQ,
	ADF4030_IIO_DEV_ATTR_VCO_FREQ,
	ADF4030_IIO_DEV_ATTR_BSYNCA_FREQ,
	ADF4030_IIO_DEV_ATTR_BSYNCB_FREQ,
	ADF4030_IIO_DEV_ATTR_TDC_SOURCE_CH,
	ADF4030_IIO_DEV_ATTR_TDC_MEASUREMENT,
	ADF4030_IIO_DEV_ATTR_ALIGNMENT_ITER,
	ADF4030_IIO_DEV_ATTR_ALIGNMENT_THRESHOLD,
	ADF4030_IIO_DEV_ATTR_SINGLE_CH_ALINGMENT,
	ADF4030_IIO_DEV_ATTR_SERIAL_CH_ALINGMENT,
	ADF4030_IIO_DEV_ATTR_BACKGROUND_SERIAL_ALINGMENT,
	ADF4030_IIO_DEV_ATTR_TEMP,
	ADF4030_IIO_DEV_ATTR_NDIV,
	ADF4030_IIO_DEV_ATTR_RDIV,
	ADF4030_IIO_DEV_ATTR_ODIVA,
	ADF4030_IIO_DEV_ATTR_ODIVB,
};

int adf4030_iio_init(struct adf4030_iio_dev **iio_dev,
		     struct adf4030_iio_dev_init_param *init_param);

int adf4030_iio_remove(struct adf4030_iio_dev *desc);

#endif /* IIO_ADF4030_H */
