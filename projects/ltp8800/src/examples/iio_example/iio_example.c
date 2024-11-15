/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for ltp8800 project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "iio_example.h"
#include "iio_ltp8800.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int iio_example_main()
{
	int ret;

	struct ltp8800_iio_desc *ltp8800_iio_desc;
	struct ltp8800_iio_desc_init_param ltp8800_iio_ip = {
		.ltp8800_init_param = &ltp8800_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltp8800_iio_init(&ltp8800_iio_desc, &ltp8800_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltp8800",
			.dev = ltp8800_iio_desc,
			.dev_descriptor = ltp8800_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ltp8800_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltp8800;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_ltp8800:
	ltp8800_iio_remove(ltp8800_iio_desc);
exit:
	if (ret)
		pr_err("Error!\n");
	return ret;
}
