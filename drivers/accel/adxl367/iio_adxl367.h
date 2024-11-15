/***************************************************************************//**
 *   @file   iio_adxl367.h
 *   @brief  Header file of IIO ADXL367 Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_ADXL367_H
#define IIO_ADXL367_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct adxl367_iio_dev {
	struct adxl367_dev *adxl367_dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct adxl367_iio_init_param {
	struct adxl367_init_param *adxl367_initial_param;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int adxl367_iio_init(struct adxl367_iio_dev **iio_dev,
		     struct adxl367_iio_init_param *init_param);

int adxl367_iio_remove(struct adxl367_iio_dev *desc);

#endif /** IIO_ADXL367_H */
