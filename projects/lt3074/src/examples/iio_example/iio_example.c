/***************************************************************************//**
*   @file   iio_example.c
*   @brief  IIO example source file for lt3074 project.
*   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_lt3074.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int example_main()
{
	int ret;

	struct lt3074_iio_desc *lt3074_iio_desc;
	struct lt3074_iio_desc_init_param lt3074_iio_ip = {
		.lt3074_init_param = &lt3074_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = lt3074_iio_init(&lt3074_iio_desc, &lt3074_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt3074",
			.dev = lt3074_iio_desc,
			.dev_descriptor = lt3074_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = lt3074_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_lt3074;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_lt3074:
	lt3074_iio_remove(lt3074_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
