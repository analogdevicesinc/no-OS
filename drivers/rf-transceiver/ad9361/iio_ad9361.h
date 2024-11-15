/***************************************************************************//**
*   @file   iio_ad9361.h
*   @brief  Header file of iio_ad9361
*   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_AD9361_H_
#define IIO_AD9361_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "iio.h"
#include "iio_types.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct iio_ad9361_init_param
 * @brief Configuration structure.
 */
struct iio_ad9361_init_param {
	/** ad9361 device instance pointer */
	struct ad9361_rf_phy *ad9361_phy;
};

/**
 * @struct iio_ad9361_desc
 * @brief Structure holding iio descriptor.
 */
struct iio_ad9361_desc {
	/** iio device descriptor */
	struct iio_device dev_descriptor;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init ad9361 iio. */
int32_t iio_ad9361_init(struct iio_ad9361_desc **desc,
			struct iio_ad9361_init_param *init);
/* Get desciptor. */
void iio_ad9361_get_dev_descriptor(struct iio_ad9361_desc *desc,
				   struct iio_device **dev_descriptor);
/* Free the resources allocated by iio_ad9361_init(). */
int32_t iio_ad9361_remove(struct iio_ad9361_desc *desc);

#endif /* IIO_AD9361_H_ */
