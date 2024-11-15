/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for max22196.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "iio_example.h"
#include "iio_max22196.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int iio_example_main()
{
	int ret;

	struct max22196_iio_desc *max22196_iio_desc;
	struct max22196_iio_desc_init_param max22196_iio_ip = {
		.max22196_init_param = &max22196_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max22196_iio_init(&max22196_iio_desc, &max22196_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max22196",
			.dev = max22196_iio_desc,
			.dev_descriptor = max22196_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max22196_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_max22196;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_max22196:
	max22196_iio_remove(max22196_iio_desc);
exit:
	if (ret)
		printf("Error!\n");
	return ret;
}
