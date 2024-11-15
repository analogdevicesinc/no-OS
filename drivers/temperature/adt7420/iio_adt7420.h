/***************************************************************************//**
 *   @file   iio_adt7420.h
 *   @brief  Header file of IIO ADT7420 Driver.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_ADT7420_H
#define IIO_ADT7420_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "iio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct adt7420_iio_dev
 * @brief iio device descriptor.
 */
struct adt7420_iio_dev {
	struct adt7420_dev *adt7420_dev;
	struct iio_dev *iio_dev;
	uint32_t active_channels;
	uint8_t no_active_channels;
};

/**
 * @struct adt7420_iio_init_param
 * @brief Configuration structure.
 */
struct adt7420_iio_init_param {
	struct adt7420_init_param *adt7420_dev_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int adt7420_iio_init(struct adt7420_iio_dev **iio_dev,
		     struct adt7420_iio_init_param *init_param);

int adt7420_iio_remove(struct adt7420_iio_dev *desc);

#endif /** IIO_ADT7420_H **/
