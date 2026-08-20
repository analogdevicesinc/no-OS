/***************************************************************************//**
 *   @file   iio_adxl37x.h
 *   @brief  Header file of IIO ADXL37x Driver.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADXL37X_H
#define IIO_ADXL37X_H

#include "iio.h"
#include "adxl37x.h"

#define ADXL37X_FIFO_MAX_SAMPLES   170
#define ADXL37X_MAX_BUFFER_SAMPLES 400


struct adxl37x_iio_dev {
	struct adxl37x_dev *adxl37x_dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct adxl37x_iio_dev_init_param {
	struct adxl37x_init_param *adxl37x_dev_init;
};

int adxl37x_iio_init(struct adxl37x_iio_dev **iio_dev,
		     struct adxl37x_iio_dev_init_param *init_param);

int adxl37x_iio_remove(struct adxl37x_iio_dev *desc);

#endif /* IIO_ADXL37X_H */
