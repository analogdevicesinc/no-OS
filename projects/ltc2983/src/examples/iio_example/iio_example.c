/********************************************************************************
 *   @file   iio_example.c
 *   @brief  IIO example code for the ltc2983 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "iio_example.h"
#include "common_data.h"
#include "no_os_print_log.h"

/*******************************************************************************
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 *******************************************************************************/
int iio_example_main()
{
	int ret;
	struct ltc2983_iio_desc *ltc2983_iio_dev;
	struct ltc2983_iio_desc_init_param ltc2983_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ltc2983_iio_ip.ltc2983_desc_init_param = &ltc2983_ip;
	ret = ltc2983_iio_init(&ltc2983_iio_dev, &ltc2983_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc2983",
			.dev = ltc2983_iio_dev,
			.dev_descriptor = ltc2983_iio_dev->iio_dev,
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
		pr_info("Error: iio_app_run: %d\r\n", ret);

	iio_app_remove(app);
error:
	ltc2983_iio_remove(ltc2983_iio_dev);
	return ret;
}
