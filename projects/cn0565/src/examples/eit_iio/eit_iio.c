/***************************************************************************//**
 *   @file   eit_iio.c
 *   @brief  IIO EIT example for cn0565 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "ad5940.h"
#include "iio_ad5940.h"
#include "iio_adg2128.h"
#include "iio_app.h"

struct no_os_i2c_desc *i2c;

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct ad5940_iio_dev *ad5940_iio = NULL;
	struct ad5940_iio_init_param ad5940_iio_ip = {
		.ad5940_init = &cn0565_ad5940_ip,
	};
	struct adg2128_iio_dev *adg2128_iio = NULL;

	ret = no_os_i2c_init(&i2c, &cn0565_i2c_ip);
	if (ret)
		return ret;

	ret = ad5940_iio_init(&ad5940_iio, &ad5940_iio_ip);
	if (ret < 0)
		return ret;

	ret = adg2128_iio_init(&adg2128_iio, i2c);
	if (ret < 0)
		return ret;

	struct iio_app_device devices[] = {
		{
			.name = "ad5940",
			.dev = ad5940_iio,
			.dev_descriptor = ad5940_iio->iio,
			.read_buff = NULL,
			.write_buff = NULL
		},
		{
			.name = "adg2128",
			.dev = adg2128_iio,
			.dev_descriptor = adg2128_iio->iio_dev,
		},
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = cn0565_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
