/***************************************************************************//**
 *   @file   iio_max11205.h
 *   @brief  Header file of iio max11205 driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __IIO_MAX11205_H__
#define __IIO_MAX11205_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum max11205_dev_id {
	MAX11205A,
	MAX11205B,
};

struct max11205_iio_dev {
	/** MAX11205 device descriptor */
	struct max11205_dev 	*max11205_dev;
	/** IIO device descriptor */
	struct iio_device 	*iio_dev;
	/** MAX11205 device sampling frequency */
	int32_t 		sampling_frequency;
};

struct max11205_iio_dev_init_param {
	/** MAX11205 device initialization data */
	struct max11205_init_param *max11205_dev_init;
	/** MAX11205 device id */
	enum max11205_dev_id dev_id;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int max11205_iio_init(struct max11205_iio_dev **iio_dev,
		      struct max11205_iio_dev_init_param *init_param);
int max11205_iio_remove(struct max11205_iio_dev *desc);

#endif /* __IIO_MAX11205_H__ */
