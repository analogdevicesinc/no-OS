/********************************************************************************
 *   @file   iio_example.c
 *   @brief  IIO example code for the ltc2378 project
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

#include "ltc2378.h"
#include "iio_ltc2378.h"
#include "iio_app.h"
#include "common_data.h"

/*******************************************************************************
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 *******************************************************************************/
int example_main()
{
	int ret;

	struct ltc2378_iio_desc *ltc2378_iio_desc;
	struct ltc2378_iio_desc_init_param ltc2378_iio_ip = {
		.ltc2378_init_param = &ltc2378_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltc2378_iio_init(&ltc2378_iio_desc, &ltc2378_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc2378-20",
			.dev = ltc2378_iio_desc,
			.dev_descriptor = ltc2378_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ltc2378;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_ltc2378:
	ltc2378_iio_remove(ltc2378_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
