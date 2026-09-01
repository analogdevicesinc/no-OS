/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for ltm4700 project.
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_ltm4700.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int example_main()
{
	int ret;

	struct ltm4700_iio_desc *ltm4700_iio_desc;
	struct ltm4700_iio_desc_init_param ltm4700_iio_ip = {
		.ltm4700_init_param = &ltm4700_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltm4700_iio_init(&ltm4700_iio_desc, &ltm4700_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltm4700",
			.dev = ltm4700_iio_desc,
			.dev_descriptor = ltm4700_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltm4700;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_ltm4700:
	ltm4700_iio_remove(ltm4700_iio_desc);
exit:
	if (ret)
		pr_err("Error!\n");
	return ret;
}
