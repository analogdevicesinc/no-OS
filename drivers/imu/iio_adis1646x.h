/***************************************************************************//**
 *   @file   iio_adis1646x.h
 *   @brief  Implementation of iio_adis1646x.h
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_ADIS1646X_H
#define IIO_ADIS1646X_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_adis_internals.h"
#include "adis1646x.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis1646x iio device. */
int adis1646x_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param);

/*! Remove adis1646x iio device. */
void adis1646x_iio_remove(struct adis_iio_dev *desc);

#endif
