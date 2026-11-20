/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for MAX34451 project.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_max34451.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

struct max34451_init_param max34451_ip = {
	.i2c_init = &max34451_i2c_ip,
	.id = ID_MAX34451
};

int example_main()
{
	int ret;
	struct max34451_iio_desc *max34451_iio_desc;
	struct max34451_iio_desc_init_param max34451_iio_ip = {
		.max34451_init_param = &max34451_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	struct iio_app_device iio_devices[] = {
		{
			.name = "max34451",
		}
	};

	ret = max34451_iio_init(&max34451_iio_desc, &max34451_iio_ip);
	if (ret)
		goto exit;

	iio_devices[0].dev = max34451_iio_desc;
	iio_devices[0].dev_descriptor = max34451_iio_desc->iio_dev;
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max34451_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_max34451;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_max34451:
	max34451_iio_remove(max34451_iio_desc);
exit:
	if (ret)
		pr_err("Error!\n");
	return ret;
}
