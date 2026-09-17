/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for adp5055 project.
 *   @author Angelo Catapang (Angelo.Catapang@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_adp5055.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int example_main()
{
	int ret;

	struct adp5055_iio_desc *adp5055_iio_desc;
	struct adp5055_iio_desc_init_param adp5055_iio_ip = {
		.adp5055_init_param = &adp5055_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adp5055_iio_init(&adp5055_iio_desc, &adp5055_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp5055",
			.dev = adp5055_iio_desc,
			.dev_descriptor = adp5055_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adp5055_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp5055;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_adp5055:
	adp5055_iio_remove(adp5055_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
