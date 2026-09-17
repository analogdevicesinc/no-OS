/***************************************************************************//**
 *   @file   iio_lm75.h
 *   @brief  IIO Header file of LM75 Driver.
 *   @author Niel Acuna (niel.acuna@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __IIO_LM75_H__
#define __IIO_LM75_H__
#include "lm75.h"
#include "iio_types.h"

#define LM75_UPDATE_INTERVAL 	100

#define LM75_CH_ATTRIBUTE_INPUT 	0
#define LM75_CH_ATTRIBUTE_MAX 		1
#define LM75_CH_ATTRIBUTE_HYST 		2

struct iio_lm75 {
	struct lm75_dev *lm75;
	struct iio_device iio_dev;
};

int iio_lm75_init(struct iio_lm75 **iio_lm75pp,
		  struct lm75_dev *dev);
int iio_lm75_remove(struct iio_lm75 *iio_lm75);

#endif /* __IIO_LM75_H__ */
