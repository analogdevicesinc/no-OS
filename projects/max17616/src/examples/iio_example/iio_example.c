/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for max17616 project.
 *   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_max17616.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int example_main()
{
	int ret;
	struct max17616_iio_desc *max17616_iio_desc;
	struct max17616_iio_desc_init_param max17616_iio_ip = {
		.max17616_init_param = &max17616_ip,
	};
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max17616_iio_init(&max17616_iio_desc, &max17616_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max17616",
			.dev = max17616_iio_desc,
			.dev_descriptor = max17616_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_max17616;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_max17616:
	max17616_iio_remove(max17616_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");

	return ret;
}
