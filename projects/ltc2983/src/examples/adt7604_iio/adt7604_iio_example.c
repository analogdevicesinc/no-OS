/*******************************************************************************
 *   @file   adt7604_iio_example.c
 *   @brief  IIO example for the EVAL-ADT7604-AZ evaluation board
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include "iio_ltc2983.h"
#include "iio_app.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief EVAL-ADT7604-AZ IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 *******************************************************************************/
int example_main()
{
	struct ltc2983_iio_desc *ltc2983_iio_dev;
	struct ltc2983_iio_desc_init_param ltc2983_iio_ip;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;

	ltc2983_iio_ip.ltc2983_desc_init_param = &ltc2983_ip;
	ret = ltc2983_iio_init(&ltc2983_iio_dev, &ltc2983_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adt7604",
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
