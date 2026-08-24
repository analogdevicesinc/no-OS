/*******************************************************************************
 *   @file   iio_example.c
 *   @brief  IIO example code for eval-ad5933ardz project
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
 ********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <common_data.h>
#include <ad5933.h>
#include <iio_ad5933.h>
#include <iio_app.h>
#include <no_os_util.h>
#include <no_os_alloc.h>

#define START_FREQ 30000U
#define FREQ_INC 1000U
#define FREQ_POINTS 15U

static struct ad5933_iio_dev_init_param ad5933_iio_ip = {
	.ad5933_dev_ip = &ad5933_user_init,
	.start_freq = START_FREQ,
	.freq_increment = FREQ_INC,
	.freq_points = FREQ_POINTS,
};

int example_main()
{
	int ret;
	struct ad5933_iio_dev *ad5933_iio_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ad5933_iio_init(&ad5933_iio_desc, &ad5933_iio_ip);


	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad5933",
			.dev = ad5933_iio_desc,
			.dev_descriptor = ad5933_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;
	app_init_param.arg = ad5933_iio_desc;

	ret = iio_app_init(&app, app_init_param);

	if (ret) {
		ad5933_iio_remove(ad5933_iio_desc);
		return ret;
	}


	return iio_app_run(app);
}
