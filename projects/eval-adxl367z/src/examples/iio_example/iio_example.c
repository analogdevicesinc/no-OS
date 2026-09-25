/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for eval-adxl367z project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_example.h"
#include "iio_adxl367.h"
#include "common_data.h"
#include "no_os_util.h"

#define DATA_BUFFER_SIZE 400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 4 * sizeof(int16_t)];

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct adxl367_iio_dev *adxl367_iio_desc;
	struct adxl367_iio_init_param adxl367_iio_ip;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 4 * sizeof(int16_t)
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

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adxl367_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
