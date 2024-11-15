/***************************************************************************//**
*   @file   iio_adg2128.h
*   @brief  Header file of iio_adg2128
*   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_ADG2128_H
#define IIO_ADG2128_H

#include "iio.h"

struct adg2128_iio_dev {
	struct iio_device *iio_dev;
	struct no_os_i2c_desc *i2c_desc;
};

int32_t adg2128_iio_init(struct adg2128_iio_dev **iio_dev,
			 struct no_os_i2c_desc *i2c_desc);
int32_t adg2128_iio_remove(struct adg2128_iio_dev *desc);

#endif /** IIO_ADG2128_H */
