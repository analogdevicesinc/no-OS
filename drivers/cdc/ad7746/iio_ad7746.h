/***************************************************************************//**
*   @file   iio_ad7746.h
*   @brief  Header file of iio_ad7746
*   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_AD7746_H
#define IIO_AD7746_H

#include "iio.h"

struct ad7746_iio_dev {
	struct ad7746_dev *ad7746_dev;
	struct iio_device *iio_dev;
	// capdac[0] - first channel, capdac[1] - second channel
	// capdac[_][0] - single-ended, capdac[_][1] - differential
	uint8_t capdac[2][2];
	int8_t capdac_set;
};

struct ad7746_iio_init_param {
	struct ad7746_init_param *ad7746_initial;
};

int32_t ad7746_iio_init(struct ad7746_iio_dev **iio_dev,
			struct ad7746_iio_init_param *init_param);
int32_t ad7746_iio_remove(struct ad7746_iio_dev *desc);

#endif /** IIO_AD7746_H */
