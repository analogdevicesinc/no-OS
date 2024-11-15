/***************************************************************************//**
 *   @file   iio_lt7182s.h
 *   @brief  Header file for the LT7182S IIO Driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_LT7182S_H
#define IIO_LT7182S_H

#include <stdbool.h>
#include "iio.h"
#include "lt7182s.h"

/**
 * @brief Structure holding the LT7182S IIO device descriptor
*/
struct lt7182s_iio_desc {
	struct lt7182s_dev *lt7182s_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LT7182S IIO initialization parameter.
*/
struct lt7182s_iio_desc_init_param {
	struct lt7182s_init_param *lt7182s_init_param;
};

/** Initializes the LT7182S IIO descriptor. */
int lt7182s_iio_init(struct lt7182s_iio_desc **,
		     struct lt7182s_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int lt7182s_iio_remove(struct lt7182s_iio_desc *);

#endif /* IIO_LT7182S_H */
