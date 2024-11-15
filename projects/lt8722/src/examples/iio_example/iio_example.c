/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for lt8722 project.
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "no_os_print_log.h"
#include "common_data.h"
#include "iio_app.h"
#include "iio_example.h"
#include "iio_lt8722.h"

int iio_example_main()
{
	int ret;

	struct lt8722_iio_dev *lt8722_iio_dev;
	struct lt8722_iio_dev_init_param lt8722_iio_ip = {
		.lt8722_init_param = &lt8722_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = lt8722_iio_init(&lt8722_iio_dev, &lt8722_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt8722",
			.dev = lt8722_iio_dev,
			.dev_descriptor = lt8722_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = lt8722_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_lt8722;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_lt8722:
	lt8722_iio_remove(lt8722_iio_dev);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
