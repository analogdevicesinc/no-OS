// /***************************************************************************//**
//  *   @file   iio_ltc7871.h
//  *   @brief  Header file for the LTC7871 IIO Driver
//  *   @author Aldrin Abacan (aldrin.abacan@analog.com)
//  *******************************************************************************
//  * Copyright 2024(c) Analog Devices, Inc.
//  *
//  * SPDX-License-Identifier: BSD-3-Clause
//  ******************************************************************************/

#ifndef __IIO_LTC7871_H__
#define __IIO_LTC7871_H__

#include <stdbool.h>
// #include "iio/iio.h"
#include "iio.h"
#include "ltc7871.h"

/**
 * @struct ltc7871_iio_dev
 * @brief Structure holding the LTC7871 IIO device descriptor
 */
struct ltc7871_iio_dev {
	struct ltc7871_dev *ltc7871_dev;
	struct iio_device *iio_dev;
};

/**
 * @struct ltc7871_iio_dev_init_param
 * @brief Structure holding the LTC7871 IIO initalization parameter.
 */
struct ltc7871_iio_dev_init_param {
	struct ltc7871_init_param *ltc7871_init_param;
};

/**
 * @brief Initializes the LTC7871 IIO descriptor.
 * @param iio_dev - The iio device descriptor.
 * @param init_param - The structure that contains the device
 * 		initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7871_iio_init(struct ltc7871_iio_dev **iio_dev,
		     struct ltc7871_iio_dev_init_param *init_param);

/**
 * @brief Free resources allocated by the init function.
 * @param desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7871_iio_remove(struct ltc7871_iio_dev *desc);

#endif /* __IIO_LTC7871_H__ */
