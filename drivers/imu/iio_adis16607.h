/***************************************************************************//**
 *   @file   iio_adis16607.h
 *   @brief  Implementation of iio_adis16607.h
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef IIO_ADIS1657X_H
#define IIO_ADIS1657X_H

#include "iio_adis_internals.h"
#include "adis16607.h"

/*! Initialize adis16607 iio device. */
int adis16607_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param,
		       struct iio_hw_trig *adis16607_trig_desc);

/*! Remove adis16607 iio device. */
void adis16607_iio_remove(struct adis_iio_dev *desc);

#endif
