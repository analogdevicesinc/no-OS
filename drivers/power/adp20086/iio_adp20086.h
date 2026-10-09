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

#include <stdbool.h>
#include "iio.h"
#include "adp20086.h"

/*
 * Upper bound on the device-global attributes: every entry of the template in
 * iio_adp20086.c, plus the NULL terminator. Keep in step with that table.
 */
#define ADP20086_MAX_GLOBAL_ATTRS	10

/**
 * @brief Structure holding the ADP20086 IIO device descriptor.
 */
struct adp20086_iio_desc {
	struct adp20086_dev *adp20086_dev;
	/* Points to iio_dev_inst; kept for consumers that pass it to the app. */
	struct iio_device *iio_dev;
	/* Per-instance copy, so the attribute list can depend on the wiring. */
	struct iio_device iio_dev_inst;
	/*
	 * Built per instance, because which attributes exist depends on whether
	 * the EN and INTB pins are wired. Embedded rather than heap-allocated,
	 * so adp20086_iio_remove() has nothing extra to free.
	 */
	struct iio_attribute global_attrs[ADP20086_MAX_GLOBAL_ATTRS];
};

/**
 * @brief Structure holding the ADP20086 IIO initialization parameter.
 */
struct adp20086_iio_desc_init_param {
	struct adp20086_init_param *adp20086_init_param;
};

/** Initialize the ADP20086 IIO descriptor. */
int adp20086_iio_init(struct adp20086_iio_desc **iio_desc,
		      struct adp20086_iio_desc_init_param *init_param);

/** Free the resources allocated by adp20086_iio_init(). */
int adp20086_iio_remove(struct adp20086_iio_desc *iio_desc);

#endif /* __IIO_ADP20086_H__ */
