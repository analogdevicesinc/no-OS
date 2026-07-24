/***************************************************************************//**
 *   @file   iio_ltc4284.h
 *   @brief  Header file of LTC4284 IIO Driver
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __IIO_LTC4284_H__
#define __IIO_LTC4284_H__

#include "iio.h"
#include "ltc4284.h"

/**
 * @struct ltc4284_iio_desc
 * @brief LTC4284 IIO descriptor
 */
struct ltc4284_iio_desc {
	struct ltc4284_dev *ltc4284_dev;
	struct iio_device *iio_dev;
};

/**
 * @struct ltc4284_iio_init_param
 * @brief LTC4284 IIO initialization parameters
 */
struct ltc4284_iio_init_param {
	struct ltc4284_init_param *ltc4284_init;
};

/**
 * @enum ltc4284_iio_channels
 * @brief LTC4284 IIO channel enumeration
 */
enum ltc4284_iio_channels {
	LTC4284_IIO_VIN_CHAN,
	LTC4284_IIO_IIN_CHAN,
	LTC4284_IIO_VOUT_CHAN,
	LTC4284_IIO_POWER_CHAN,
	LTC4284_IIO_ENERGY_CHAN,
	LTC4284_IIO_NUM_CHANNELS
};

/* IIO interface functions */
int ltc4284_iio_init(struct ltc4284_iio_desc **iio_desc,
		     const struct ltc4284_iio_init_param *init_param);
int ltc4284_iio_remove(struct ltc4284_iio_desc *desc);

#endif /* __IIO_LTC4284_H__ */
