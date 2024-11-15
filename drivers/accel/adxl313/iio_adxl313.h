/***************************************************************************//**
 *   @file   iio_adxl313.h
 *   @brief  Header file of IIO ADXL355 Driver.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_ADXL313_H
#define IIO_ADXL313_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct adxl313_iio_dev {
	struct adxl313_dev *adxl313_dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct adxl313_iio_dev_init_param {
	struct adxl313_init_param *adxl313_dev_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int adxl313_iio_init(struct adxl313_iio_dev **iio_dev,
		     struct adxl313_iio_dev_init_param *init_param);

int adxl313_iio_remove(struct adxl313_iio_dev *desc);

#endif /** IIO_ADXL355_H */
