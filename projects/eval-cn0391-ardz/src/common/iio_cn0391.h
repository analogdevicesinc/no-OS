/***************************************************************************//**
 *   @file   iio_cn0391.h
 *   @brief  Header file of IIO CN0391 driver.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef IIO_CN0391_H_
#define IIO_CN0391_H_

#include "iio.h"

#ifdef CN0391_IIO_SUPPORT
extern struct iio_device iio_cn0391_device;
#endif /* CN0391_IIO_SUPPORT */

/* Custom AD7124-8 IIO device exposing the same 4 thermocouple channels
 * (CH0..CH3) backed by the cn0391_dev descriptor — used for demo purposes
 * with SCOPY application. */
extern struct iio_device iio_ad7124_cn0391_device;

#endif /* IIO_CN0391_H_ */
