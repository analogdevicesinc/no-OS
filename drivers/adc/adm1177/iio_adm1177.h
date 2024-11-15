/**************************************************************************//**
*   @file   iio_adm1177.h
*   @brief  Header file of iio_adm1177
*   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
* Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_ADM1177_H
#define IIO_ADM1177_H

#include "iio.h"
#include "iio_types.h"

struct adm1177_iio_dev {
	struct adm1177_dev *adm1177_dev;
	struct iio_device *iio_dev;

	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct adm1177_iio_init_param {
	struct adm1177_init_param *adm1177_initial;
};

int adm1177_iio_init(struct adm1177_iio_dev **iio_dev,
		     struct adm1177_iio_init_param *init_param);

int adm1177_iio_remove(struct adm1177_iio_dev *desc);

#endif /** IIO_ADM1177_H */
