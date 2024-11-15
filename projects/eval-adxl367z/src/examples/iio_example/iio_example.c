/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for eval-adxl367z project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_example.h"
#include "iio_adxl367.h"
#include "common_data.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DATA_BUFFER_SIZE 400

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE*4*sizeof(int16_t)];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	int ret;
	struct adxl367_iio_dev *adxl367_iio_desc;
	struct adxl367_iio_init_param adxl367_iio_ip;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE*4*sizeof(int16_t)
	};

	adxl367_iio_ip.adxl367_initial_param = &init_param;
	ret = adxl367_iio_init(&adxl367_iio_desc, &adxl367_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl367",
			.dev = adxl367_iio_desc,
			.dev_descriptor = adxl367_iio_desc->iio_dev,
			.read_buff = &accel_buff,
		}
	};

	return iio_app_run(NULL, 0, iio_devices, NO_OS_ARRAY_SIZE(iio_devices));
}
