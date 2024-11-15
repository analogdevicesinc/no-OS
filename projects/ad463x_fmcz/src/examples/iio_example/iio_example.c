/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example header for ad463x_fmcz project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "iio_ad463x.h"
#include "no_os_util.h"
#include "iio_app.h"

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
	struct ad463x_dev *dev;
	struct iio_ad463x *iio_ad463x;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;
	struct iio_data_buffer rd_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	ret = ad463x_init(&dev, &ad463x_init_param);
	if (ret)
		return ret;

#if ADAQ4224_DEV
	/* Apply a gain of 1/3 */
#define AD4224_INITIAL_GAIN_IDX 0

	ret = ad463x_fill_scale_tbl(dev);
	if (ret)
		goto out;

	ret = ad463x_set_pgia_gain(dev, AD4224_INITIAL_GAIN_IDX);
	if (ret)
		goto out;
#endif

	ret = iio_ad463x_init(&iio_ad463x, dev);
	if(ret < 0)
		goto out;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad463x", iio_ad463x, &iio_ad463x->iio_dev_desc,
			       &rd_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = ad463x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto out;

	ret = iio_app_run(app);
out:
	return ad463x_remove(dev);
}
