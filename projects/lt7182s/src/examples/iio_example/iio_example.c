/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for lt7182s project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "iio_example.h"
#include "iio_lt7182s.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int iio_example_main()
{
	int ret;

	struct lt7182s_iio_desc *lt7182s_iio_desc;
	struct lt7182s_iio_desc_init_param lt7182s_iio_ip = {
		.lt7182s_init_param = &lt7182s_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = lt7182s_iio_init(&lt7182s_iio_desc, &lt7182s_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt7182s",
			.dev = lt7182s_iio_desc,
			.dev_descriptor = lt7182s_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = lt7182s_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_lt7182s;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_lt7182s:
	lt7182s_iio_remove(lt7182s_iio_desc);
exit:
	if (ret)
		pr_err("Error!\n");
	return ret;
}
