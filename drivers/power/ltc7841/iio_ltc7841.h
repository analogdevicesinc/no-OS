/***************************************************************************//**
 *   @file   iio_ltc7841.h
 *   @brief  Header file for the LTC7841 IIO Driver
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef IIO_LTC7841_H
#define IIO_LTC7841_H

#include <stdbool.h>
#include "iio.h"
#include "ltc7841.h"

#define CHANNEL_COUNT                            2
#define SCALED_VOLTAGE                           50 /* in units of uV/bit */
#define DECIMAL_POINT_SCALE_VOLTAGE              10000

/**
 * @brief Structure holding the LTC7841 IIO device descriptor
*/
struct ltc7841_iio_desc {
	struct ltc7841_desc *ltc7841_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTC7841 IIO initialization parameter.
*/
struct ltc7841_iio_desc_init_param {
	struct ltc7841_init_param *ltc7841_init_param;
	/* Rsense value used in units of milliOhm*/
	uint8_t LTC7841_rsense_value;
};

/** Initializes the LTC7841 IIO descriptor. */
int ltc7841_iio_init(struct ltc7841_iio_desc ** iio_desc,
		     struct ltc7841_iio_desc_init_param * init_param);

/** Free resources allocated by the initialization function. */
int ltc7841_iio_remove(struct ltc7841_iio_desc * desc);

#endif /* IIO_LTC7841_H */
