/********************************************************************************
 *   @file   iio_example.c
 *   @brief  IIO example code for the max31855 project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "iio_example.h"
#include "common_data.h"

#define DATA_BUFFER_SIZE 400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(int32_t)];

/*******************************************************************************
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 *******************************************************************************/
int iio_example_main()
{
	int ret;
	struct max31855_iio_dev *max31855_iio_dev;
	struct max31855_iio_dev_init_param max31855_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer dbuff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(int32_t)
	};
	struct iio_app_init_param app_init_param = {0};

	max31855_iio_ip.max31855_dev_init = &max31855_ip;
	ret = max31855_iio_init(&max31855_iio_dev, &max31855_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "maxim_thermocouple",
			.dev = max31855_iio_dev,
			.dev_descriptor = max31855_iio_dev->iio_dev,
			.read_buff = &dbuff,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error;

	ret = iio_app_run(app);
	if (ret)
		iio_app_remove(app);

error:
	max31855_iio_remove(max31855_iio_dev);

	return ret;
}
