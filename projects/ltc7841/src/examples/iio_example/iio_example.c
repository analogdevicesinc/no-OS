/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for ltc7841 project.
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_app.h"
#include "iio_ltc7841.h"
#include "common_data.h"
#include "no_os_print_log.h"

int example_main(void)
{
	int ret;
	struct ltc7841_iio_desc *ltc7841_iio_desc;
	struct ltc7841_iio_desc_init_param ltc7841_iio_ip = {
		.ltc7841_init_param = &ltc7841_ip,
		.LTC7841_rsense_value = RSENSE_VALUE_DC2798A
	};
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltc7841_iio_init(&ltc7841_iio_desc, &ltc7841_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc7841",
			.dev = ltc7841_iio_desc,
			.dev_descriptor = ltc7841_iio_desc->iio_dev,
		}
	};
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltc7841;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_ltc7841:
	ltc7841_iio_remove(ltc7841_iio_desc);
exit:
	if (ret)
		pr_err("Error!\n");

	return ret;
}

