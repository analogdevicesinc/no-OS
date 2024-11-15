/***************************************************************************//**
 *   @file   iio_ad5940.h
 *   @brief  Header file of ad5940 iio driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_AD5940_H
#define IIO_AD5940_H

#include "iio.h"
#include "ad5940.h"

enum ad5940_iio_attr {
	AD5940_IIO_EXCITATION_FREQUENCY,
	AD5940_IIO_EXCITATION_AMPLITUDE,
	AD5940_IIO_IMPEDANCE_MODE,
	AD5940_IIO_MAGNITUDE_MODE,
	AD5940_IIO_GPIO1_TOGGLE,
};

struct ad5940_iio_dev {
	struct ad5940_dev *ad5940;
	struct iio_device *iio;
	bool magnitude_mode;
	bool gpio1;
	uint32_t AppBuff[512];
};

struct ad5940_iio_init_param {
	struct ad5940_init_param *ad5940_init;
};

int32_t ad5940_iio_init(struct ad5940_iio_dev **iio_dev,
			struct ad5940_iio_init_param *init_param);
int32_t ad5940_iio_remove(struct ad5940_iio_dev *desc);

#endif /** IIO_AD5940_H */

