/********************************************************************************
 *   @file   iio_example.c
 *   @brief  IIO example code for the ad8460 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

/*******************************************************************************
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 *******************************************************************************/
int example_main()
{
	int ret;
	struct ad8460_iio_device *ad8460_iio_dev;
	struct ad8460_iio_init_param ad8460_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ad8460_iio_ip.init_param = &ad8460_ip;
	ret = ad8460_iio_init(&ad8460_iio_dev, &ad8460_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad8460",
			.dev = ad8460_iio_dev,
			.dev_descriptor = ad8460_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error;

	ret = iio_app_run(app);
	if (ret)
		pr_err("Error: iio_app_run: %d\r\n", ret);

	iio_app_remove(app);
error:
	ad8460_iio_remove(ad8460_iio_dev);
	return ret;
}
