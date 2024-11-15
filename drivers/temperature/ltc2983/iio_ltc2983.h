/***************************************************************************//**
 *   @file   iio_ltc2983.h
 *   @brief  Header file of IIO LTC2983 driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_LTC2983_H
#define IIO_LTC2983_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ltc2983_iio_desc {
	struct ltc2983_desc *ltc2983_dev;
	struct iio_device *iio_dev;
};

struct ltc2983_iio_desc_init_param {
	struct ltc2983_init_param *ltc2983_desc_init_param;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int ltc2983_iio_init(struct ltc2983_iio_desc **,
		     struct ltc2983_iio_desc_init_param *);
int ltc2983_iio_remove(struct ltc2983_iio_desc *);

#endif
