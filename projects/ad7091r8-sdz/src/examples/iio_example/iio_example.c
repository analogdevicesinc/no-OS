/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for eval-ad7091r8-sdz project.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_example.h"
#include "iio_ad7091r8.h"
#include "common_data.h"
#include "iio_app.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/

int iio_example_main()
{
	struct ad7091r8_iio_dev *ad7091r8_iio_desc;
	struct ad7091r8_iio_dev_init_param ad7091r8_iio_param;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct ad7091r8_init_param *ad7091r8_init_p;
	int ret;

	/* Use ad7091r4_ip and ad7091r2_ip for AD7091R-4 and AD7091R-2 respectively */
	ad7091r8_init_p = &ad7091r8_ip;
	ad7091r8_iio_param.ad7091r8_dev_init = ad7091r8_init_p;
	ret = ad7091r8_iio_init(&ad7091r8_iio_desc, &ad7091r8_iio_param);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = ad7091r8_names[ad7091r8_init_p->device_id],
			.dev = ad7091r8_iio_desc,
			.dev_descriptor = ad7091r8_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad7091r8_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ad7091r8;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_ad7091r8:
	ad7091r8_iio_remove(ad7091r8_iio_desc);

	return ret;
}
