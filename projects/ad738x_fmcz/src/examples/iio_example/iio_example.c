/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example header for eval-ad738x project
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>

#include "iio_example.h"
#include "common_data.h"
#include "iio.h"
#include "ad738x.h"
#include "iio_ad738x.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "iio_app.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int iio_example_main()
{
	struct ad738x_iio_dev *dev;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	struct iio_app_device iio_devices[] = {
		{ .name = "ad738x" },
	};
	int ret;

	ret = ad738x_iio_init(&dev, &ad738x_init_param);
	if (ret)
		return ret;

	iio_devices[0].dev = dev;
	iio_devices[0].dev_descriptor = dev->iio_dev;
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad738x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		ad738x_iio_remove(dev);
		return ret;
	}

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\n", ret);

	iio_app_remove(app);

	ad738x_iio_remove(dev);

	return ret;
}
