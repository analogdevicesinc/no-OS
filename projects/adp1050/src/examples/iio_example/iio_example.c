/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for adp1050 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "iio_example.h"
#include "iio_adp1050.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

int iio_example_main()
{
	int ret;

	struct adp1050_iio_desc *adp1050_iio_desc;
	struct adp1050_iio_desc_init_param adp1050_iio_ip = {
		.adp1050_init_param = &adp1050_ip,
		.vout_scale_monitor = 0xA155,
		.vin_scale_monitor = 0xB033,
		.iin_scale_monitor = 0x01,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adp1050_iio_init(&adp1050_iio_desc, &adp1050_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp1050",
			.dev = adp1050_iio_desc,
			.dev_descriptor = adp1050_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adp1050_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp1050;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_adp1050:
	adp1050_iio_remove(adp1050_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
