/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for lt7170 project.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_lt7170.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int example_main()
{
	int ret;

	struct lt7170_iio_desc *lt7170_iio_desc;
	struct lt7170_iio_desc_init_param lt7170_iio_ip = {
		.lt7170_init_param = &lt7170_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = lt7170_iio_init(&lt7170_iio_desc, &lt7170_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt7170",
			.dev = lt7170_iio_desc,
			.dev_descriptor = lt7170_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = lt7170_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_lt7170;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_lt7170:
	lt7170_iio_remove(lt7170_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
