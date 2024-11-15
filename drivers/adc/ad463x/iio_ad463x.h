/***************************************************************************//**
*   @file   iio_ad463x.h
*   @brief  Header file of iio_ad463x
*   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_AD463x
#define IIO_AD463x

#ifdef IIO_SUPPORT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "iio_types.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct iio_ad463x {
	/* Mask of active ch */
	uint32_t mask;
	/** iio device descriptor */
	struct iio_device iio_dev_desc;
	/** Device Descriptor */
	struct ad463x_dev *ad463x_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init function. */
int32_t iio_ad463x_init(struct iio_ad463x **desc,
			struct ad463x_dev *dev);

/* Free the resources allocated by iio_ad463x_init(). */
int32_t iio_ad463x_remove(struct iio_ad463x *desc);

#endif /* IIO_SUPPORT */

#endif /* IIO_AD463x */
