/***************************************************************************//**
 *   @file   iio_ltp8800.h
 *   @brief  Header file for the LTP8800 IIO Driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_LTP8800_H
#define IIO_LTP8800_H

#include <stdbool.h>
#include "iio.h"
#include "ltp8800.h"

/**
 * @brief Structure holding the LTP8800 IIO device descriptor
*/
struct ltp8800_iio_desc {
	struct ltp8800_dev *ltp8800_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTP8800 IIO initialization parameter.
*/
struct ltp8800_iio_desc_init_param {
	struct ltp8800_init_param *ltp8800_init_param;
};

/** Initializes the LTP8800 IIO descriptor. */
int ltp8800_iio_init(struct ltp8800_iio_desc **,
		     struct ltp8800_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int ltp8800_iio_remove(struct ltp8800_iio_desc *);

#endif /* IIO_LTP8800_H */
