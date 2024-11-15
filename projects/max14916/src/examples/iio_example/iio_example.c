/***************************************************************************//**
 *   @file   max14916/src/examples/iio_example/iio_example.c
 *   @brief  Source file for iio example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "iio_example.h"
#include "iio_max14916.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int iio_example_main()
{
	int ret;

	struct max14916_iio_desc *max14916_iio_desc;
	struct max14916_iio_desc_init_param max14916_iio_ip = {
		.max14916_init_param = &max14916_ip,
		.ch_enable = {
			true, true, true, false, false, false, false, false
		},
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max14916_iio_init(&max14916_iio_desc, &max14916_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max14916",
			.dev = max14916_iio_desc,
			.dev_descriptor = max14916_iio_desc->iio_dev,
			.read_buff = NULL,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max14916_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_max14916;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_max14916:
	max14916_iio_remove(max14916_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
