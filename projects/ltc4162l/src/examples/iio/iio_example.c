/********************************************************************************
 *   @file   iio_example.c
 *   @brief  IIO example code for the ltc4162l project
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"


int example_main()
{

	int ret;
	struct ltc4162l_iio_device *ltc4162l_iio_dev;
	struct ltc4162l_iio_init_param ltc4162l_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ltc4162l_iio_ip.init_param = &ltc4162l_ip;
	ret = ltc4162l_iio_init(&ltc4162l_iio_dev, &ltc4162l_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc4162l",
			.dev = ltc4162l_iio_dev,
			.dev_descriptor = ltc4162l_iio_dev->iio_dev,
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
	ltc4162l_iio_remove(ltc4162l_iio_dev);
	return ret;

}
