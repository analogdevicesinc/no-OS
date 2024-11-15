/***************************************************************************//**
 *   @file   iio_adis1650x.h
 *   @brief  Implementation of iio_adis1650x.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_ADIS1650X_H
#define IIO_ADIS1650X_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_adis_internals.h"
#include "adis1650x.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis1650x iio device. */
int adis1650x_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param);

/*! Remove adis1650x iio device. */
void adis1650x_iio_remove(struct adis_iio_dev *desc);

#endif
