/***************************************************************************//**
 *   @file   iio_adp1050.h
 *   @brief  Header file for the ADP1050 IIO Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_ADP1050_H
#define IIO_ADP1050_H

#include <stdbool.h>
#include "iio.h"
#include "adp1050.h"

/**
 * @brief Structure holding the ADP1050 IIO device descriptor
*/
struct adp1050_iio_desc {
	struct adp1050_desc *adp1050_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the ADP1050 IIO initalization parameter.
*/
struct adp1050_iio_desc_init_param {
	struct adp1050_init_param *adp1050_init_param;
	uint16_t vout_scale_monitor;
	uint16_t vin_scale_monitor;
	uint16_t iin_scale_monitor;
};

/** Initializes the ADP1050 IIO descriptor. */
int adp1050_iio_init(struct adp1050_iio_desc **,
		     struct adp1050_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int adp1050_iio_remove(struct adp1050_iio_desc *);

#endif /* IIO_ADP1050_H */
