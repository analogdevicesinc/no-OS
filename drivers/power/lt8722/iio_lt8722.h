/***************************************************************************//**
 *   @file   iio_lt8722.h
 *   @brief  Header file for the LT8722 IIO Driver
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __IIO_LT8722_H__
#define __IIO_LT8722_H__

#include <stdbool.h>
#include "iio.h"
#include "lt8722.h"

/**
 * @struct lt8722_iio_dev
 * @brief Structure holding the LT8722 IIO device descriptor
 */
struct lt8722_iio_dev {
	struct lt8722_dev *lt8722_dev;
	struct iio_device *iio_dev;
};

/**
 * @struct lt8722_iio_dev_init_param
 * @brief Structure holding the LT8722 IIO initalization parameter.
 */
struct lt8722_iio_dev_init_param {
	struct lt8722_init_param *lt8722_init_param;
};

/** Initializes the LT8722 IIO descriptor. */
int lt8722_iio_init(struct lt8722_iio_dev **,
		    struct lt8722_iio_dev_init_param *);

/** Free resources allocated by the initialization function. */
int lt8722_iio_remove(struct lt8722_iio_dev *);

#endif /* __IIO_LT8722_H__ */
