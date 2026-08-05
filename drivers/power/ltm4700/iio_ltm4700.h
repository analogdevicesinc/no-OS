/***************************************************************************//**
 *   @file   iio_ltm4700.h
 *   @brief  Header file for the LTM4700 IIO Driver
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __IIO_LTM4700_H__
#define __IIO_LTM4700_H__

#include <stdbool.h>
#include "iio.h"
#include "ltm4700.h"

/**
 * @brief Structure holding the LTM4700 IIO device descriptor
 */
struct ltm4700_iio_desc {
	struct ltm4700_dev *ltm4700_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTM4700 IIO initialization parameter.
 */
struct ltm4700_iio_desc_init_param {
	struct ltm4700_init_param *ltm4700_init_param;
};

/* LTM4700 IIO attributes */
enum ltm4700_iio_attr_id {
	LTM4700_IIO_VIN,
	LTM4700_IIO_IIN,
	LTM4700_IIO_VOUT,
	LTM4700_IIO_IOUT,
	LTM4700_IIO_TEMP_EXT,
	LTM4700_IIO_TEMP_IC,
	LTM4700_IIO_FREQ,
	LTM4700_IIO_POUT,
	LTM4700_IIO_PIN,
	LTM4700_IIO_VOUT_COMMAND,
	LTM4700_IIO_VOUT_MAX,
	LTM4700_IIO_VOUT_MARGIN_HIGH,
	LTM4700_IIO_VOUT_MARGIN_LOW,
	LTM4700_IIO_OPERATION,
	LTM4700_IIO_CLEAR_PEAKS,
	LTM4700_IIO_DEVICE_ID,
};

/**
 * @brief Initialize the LTM4700 IIO driver
 * @param iio_desc - Pointer to IIO descriptor pointer
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_iio_init(struct ltm4700_iio_desc **iio_desc,
		     struct ltm4700_iio_desc_init_param *init_param);

/**
 * @brief Free resources allocated by the init function
 * @param desc - IIO descriptor to free
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_iio_remove(struct ltm4700_iio_desc *desc);

#endif /* __IIO_LTM4700_H__ */
