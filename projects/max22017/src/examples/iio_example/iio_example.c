/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for max22017.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "iio_example.h"
#include "iio_max22017.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int iio_example_main()
{
	int ret;

	struct max22017_iio_desc *max22017_iio_desc;
	struct max22017_iio_desc_init_param max22017_iio_ip = {
		.max22017_init_param = &max22017_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max22017_iio_init(&max22017_iio_desc, &max22017_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max22017",
			.dev = max22017_iio_desc,
			.dev_descriptor = max22017_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max22017_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_max22017;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_max22017:
	max22017_iio_remove(max22017_iio_desc);
exit:
	if (ret)
		printf("Error!\n");
	return ret;
}
