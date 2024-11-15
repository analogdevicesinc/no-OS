/***************************************************************************//**
 *   @file   iio_adis1647x.h
 *   @brief  Implementation of iio_adis1647x.h
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_ADIS1647X_H
#define IIO_ADIS1647X_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_adis_internals.h"
#include "adis1647x.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis1647x iio device. */
int adis1647x_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param);

/*! Remove adis1647x iio device. */
void adis1647x_iio_remove(struct adis_iio_dev *desc);

#endif
