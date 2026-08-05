/***************************************************************************//**
*   @file   iio_max17616.h
*   @brief  Header file of the MAX17616 IIO Driver
*   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __IIO_MAX17616_H__
#define __IIO_MAX17616_H__

#include <stdbool.h>
#include "iio.h"
#include "max17616.h"

/**
 * @brief Structure holding the MAX17616 IIO device descriptor
*/
struct max17616_iio_desc {
	struct max17616_dev *max17616_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the MAX17616 IIO initialization parameter.
*/
struct max17616_iio_desc_init_param {
	struct max17616_init_param *max17616_init_param;
};

/** Initializes the MAX17616 IIO descriptor. */
int max17616_iio_init(struct max17616_iio_desc **iio_desc,
		      struct max17616_iio_desc_init_param *init_param);

/** Free resources allocated by the initialization function. */
int max17616_iio_remove(struct max17616_iio_desc *desc);

#endif /* __IIO_MAX17616_H__ */
