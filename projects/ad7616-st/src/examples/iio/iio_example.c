/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example header for eval-ad7616 project
 *   @author Esteban Blanc (eblanc@baylibre.com)
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
#include "ad7616.h"
#include "iio_ad7616.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "iio_app.h"

extern struct no_os_uart_desc *uart_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int iio_example_main()
{
	struct ad7616_iio_dev *dev;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	struct iio_app_device iio_devices[] = {
		{ .name = "ad7616" },
	};
	int ret;

	// ad7616_ip.crc = 1; // If you want to use CRC, uncomment this

	ret = ad7616_iio_init(&dev, &ad7616_ip);
	if (ret)
		return ret;

	iio_devices[0].dev = dev;
	iio_devices[0].dev_descriptor = dev->iio_dev;
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad7616_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto iio_remove;

	ret = iio_app_run(app);

	iio_app_remove(app);

	if (ret)
		pr_err("IIO app closed with error: %i", ret);

iio_remove:
	ad7616_iio_remove(dev);

	return ret;
}
