/***************************************************************************//**
 *   @file   iio_adxl355.h
 *   @brief  Header file of IIO ADXL355 Driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_ADXL355_H
#define IIO_ADXL355_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"
#include "no_os_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct iio_trigger adxl355_iio_trig_desc;

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct adxl355_iio_dev {
	struct adxl355_dev *adxl355_dev;
	struct iio_device *iio_dev;
	int adxl355_hpf_3db_table[7][2];
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct adxl355_iio_dev_init_param {
	struct adxl355_init_param *adxl355_dev_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int adxl355_iio_init(struct adxl355_iio_dev **iio_dev,
		     struct adxl355_iio_dev_init_param *init_param);

int adxl355_iio_remove(struct adxl355_iio_dev *desc);

#endif /** IIO_ADXL355_H */
