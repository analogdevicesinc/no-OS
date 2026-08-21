/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for ltc7871 project.
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "no_os_print_log.h"
#include "common_data.h"
#include "iio_app.h"
#include "iio_ltc7871.h"

int example_main()
{
	int ret;

	struct ltc7871_iio_dev *ltc7871_iio_dev;
	struct ltc7871_iio_dev_init_param ltc7871_iio_ip = {
		.ltc7871_init_param = &ltc7871_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltc7871_iio_init(&ltc7871_iio_dev, &ltc7871_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc7871",
			.dev = ltc7871_iio_dev,
			.dev_descriptor = ltc7871_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ltc7871_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltc7871;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_ltc7871:
	ltc7871_iio_remove(ltc7871_iio_dev);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
