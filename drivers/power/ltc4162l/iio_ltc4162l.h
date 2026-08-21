/***************************************************************************//**
 *   @file   iio_ltc4162l.h
 *   @brief  Implementation of IIO LTC4162L Driver.
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __IIO_LTC4162L_H__
#define __IIO_LTC4162L_H__

#include "iio.h"

/**
 * @brief Structure holding the LTC4162L IIO device descriptor
*/
struct ltc4162l_iio_device {
	struct ltc4162l_desc *dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_active_channels;
};

/**
 * @brief Structure holding the LTC4162L IIO initialization parameter.
*/
struct ltc4162l_iio_init_param {
	struct ltc4162l_init_param *init_param;
};

/** Initializes the LTC4162L IIO descriptor. */
int ltc4162l_iio_init(struct ltc4162l_iio_device **iio_device,
		      struct ltc4162l_iio_init_param *iio_init_param);

/* Free resources allocated by the initialization function. */
int ltc4162l_iio_remove(struct ltc4162l_iio_device *iio_device);

#endif	/* __IIO_LTC4162L_H__ */
