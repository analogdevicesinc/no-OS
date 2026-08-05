/***************************************************************************//**
 *   @file   iio_adp5055.h
 *   @brief  Header file for the ADP5055 IIO Driver
 *   @author Angelo Catapang (angelo.catapang@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADP5055_H
#define IIO_ADP5055_H

#include <stdbool.h>
#include "iio.h"
#include "adp5055.h"

/**
 * @brief Structure holding the ADP5055 IIO device descriptor
*/
struct adp5055_iio_desc {
	struct adp5055_desc *adp5055_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the ADP5055 IIO initalization parameter.
*/
struct adp5055_iio_desc_init_param {
	struct adp5055_init_param *adp5055_init_param;
};

/** Initializes the ADP5055 IIO descriptor. */
int adp5055_iio_init(struct adp5055_iio_desc **,
		     struct adp5055_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int adp5055_iio_remove(struct adp5055_iio_desc *);

#endif /* IIO_ADP5055_H */
