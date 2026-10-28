/***************************************************************************//**
 *   @file   iio_adp20086.h
 *   @brief  Header file for the ADP20086 IIO driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __IIO_ADP20086_H__
#define __IIO_ADP20086_H__

#include "iio.h"
#include "adp20086.h"

/**
 * @brief ADP20086 IIO device descriptor.
 */
struct adp20086_iio_desc {
	struct adp20086_dev *adp20086_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief ADP20086 IIO device initialization parameter.
 */
struct adp20086_iio_desc_init_param {
	struct adp20086_init_param *adp20086_init_param;
};

int adp20086_iio_init(struct adp20086_iio_desc **iio_desc,
		      struct adp20086_iio_desc_init_param *init_param);

int adp20086_iio_remove(struct adp20086_iio_desc *iio_desc);

#endif /* __IIO_ADP20086_H__ */
