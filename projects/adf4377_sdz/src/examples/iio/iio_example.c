/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for adf4377 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "iio_adf4377.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 */
int example_main()
{
	struct adf4377_iio_dev *adf4377_iio_dev;
	struct adf4377_iio_dev_init_param adf4377_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;


	adf4377_iio_ip.adf4377_dev_init = &adf4377_ip;
	ret = adf4377_iio_init(&adf4377_iio_dev, &adf4377_iio_ip);
	if (ret)
		return ret;
	// pr_info("ADF4377 IIO initialized\n");
	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4377",
			.dev = adf4377_iio_dev,
			.dev_descriptor = adf4377_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf4377_uart_ip;
	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;
	iio_app_run(app);

	iio_app_remove(app);
exit:
	adf4377_iio_remove(adf4377_iio_dev);

	if (ret)
		pr_info("Error!\n");
	return ret;
}
