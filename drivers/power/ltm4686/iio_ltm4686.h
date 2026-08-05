/***************************************************************************//**
 *   @file   iio_ltm4686.h
 *   @brief  Header file for the LTM4686 IIO Driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_LTM4686_H
#define IIO_LTM4686_H

#include <stdbool.h>
#include "iio.h"
#include "ltm4686.h"

/**
 * @brief Structure holding the LTM4686 IIO device descriptor
*/
struct ltm4686_iio_desc {
	struct ltm4686_dev *ltm4686_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTM4686 IIO initialization parameter.
*/
struct ltm4686_iio_desc_init_param {
	struct ltm4686_init_param *ltm4686_init_param;
};

/** Initializes the LTM4686 IIO descriptor. */
int ltm4686_iio_init(struct ltm4686_iio_desc **,
		     struct ltm4686_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int ltm4686_iio_remove(struct ltm4686_iio_desc *);

#endif /* IIO_LTM4686_H */
