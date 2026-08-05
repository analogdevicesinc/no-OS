/***************************************************************************//**
 *   @file   iio_adp1055.h
 *   @brief  Header file for the ADP1055 IIO Driver
 *   @author Ivan Gil Mercano (IvanGil.Mercano@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADP1055_H
#define IIO_ADP1055_H

#include <stdbool.h>
#include "iio.h"
#include "adp1055.h"

/**
 * @brief Structure holding the adp1055 IIO device descriptor
*/
struct adp1055_iio_desc {
	struct adp1055_desc *adp1055_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the adp1055 IIO initalization parameter.
*/
struct adp1055_iio_desc_init_param {
	struct adp1055_init_param *adp1055_init_param;
	uint16_t vout_scale_monitor;
	uint16_t vin_scale_monitor;
	uint16_t iin_scale_monitor;
};

/** Initializes the adp1055 IIO descriptor. */
int adp1055_iio_init(struct adp1055_iio_desc **,
		     struct adp1055_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int adp1055_iio_remove(struct adp1055_iio_desc *);

#endif /* IIO_adp1055_H */
