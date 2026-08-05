/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for ad7124-8pmdz project.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_ad7124.h"
#include "common_data.h"
#include "iio_app.h"
#include "no_os_util.h"

#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE	400
#endif

static uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(uint32_t)];

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct ad7124_dev *ad7124_device;
	struct iio_app_desc *app;
	struct iio_data_buffer adc_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t),
	};
	struct iio_app_init_param app_init_param = { 0 };

	ret = ad7124_setup(&ad7124_device, &ad7124_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("ad7124-8", ad7124_device, &iio_ad7124_device,
			       &adc_buff, NULL, NULL)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad7124_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
