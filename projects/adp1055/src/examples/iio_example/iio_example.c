/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for adp1055 project.
 *   @author Ivan Gil Mercano (Ivangil.mercano@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_adp1055.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int example_main()
{
	int ret;

	struct adp1055_iio_desc *adp1055_iio_desc;
	struct adp1055_iio_desc_init_param adp1055_iio_ip = {
		.adp1055_init_param = &adp1055_ip,
		.vout_scale_monitor = 0xA155,
		.vin_scale_monitor = 0xB033,
		.iin_scale_monitor = 0x01,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adp1055_iio_init(&adp1055_iio_desc, &adp1055_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp1055",
			.dev = adp1055_iio_desc,
			.dev_descriptor = adp1055_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adp1055_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp1055;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_adp1055:
	adp1055_iio_remove(adp1055_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
