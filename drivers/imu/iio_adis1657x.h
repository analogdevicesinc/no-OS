/***************************************************************************//**
 *   @file   iio_adis1657x.h
 *   @brief  Implementation of iio_adis1657x.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_ADIS1657X_H
#define IIO_ADIS1657X_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_adis_internals.h"
#include "adis1657x.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis1657x iio device. */
int adis1657x_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param,
		       struct iio_hw_trig *adis1657x_trig_desc);

/*! Remove adis1657x iio device. */
void adis1657x_iio_remove(struct adis_iio_dev *desc);

#endif
