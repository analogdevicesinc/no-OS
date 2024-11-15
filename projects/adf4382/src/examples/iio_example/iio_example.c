/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for adf4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common_data.h"
#include "iio_example.h"
#include "iio_adf4382.h"
#include "iio_app.h"
#include "no_os_print_log.h"

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 */
int iio_example_main()
{
	struct adf4382_iio_dev *adf4382_iio_dev;
	struct adf4382_iio_dev_init_param adf4382_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	adf4382_iio_ip.adf4382_dev_init = &adf4382_ip;
	ret = adf4382_iio_init(&adf4382_iio_dev, &adf4382_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4382",
			.dev = adf4382_iio_dev,
			.dev_descriptor = adf4382_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf4382_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	ret = iio_app_run(app);

	iio_app_remove(app);
exit:
	adf4382_iio_remove(adf4382_iio_dev);

	if (ret)
		pr_info("Error!\n");

	return ret;
}
