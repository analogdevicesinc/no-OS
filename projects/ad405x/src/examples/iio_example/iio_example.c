/*******************************************************************************
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for ad405x project.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_app.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "iio_ad405x.h"

#define DATA_BUFFER_SIZE 1000

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(int32_t)];

/*******************************************************************************
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct ad405x_iio_dev *ad405x_iio_desc;
	struct ad405x_iio_dev_init_param ad405x_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer adc_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(int32_t)
	};
	struct iio_app_init_param app_init_param = { 0 };

	ad405x_iio_ip.ad405x_dev_init = &ad405x_ip;
	ret = ad405x_iio_init(&ad405x_iio_desc, &ad405x_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad405x",
			.dev = ad405x_iio_desc,
			.dev_descriptor = ad405x_iio_desc->iio_dev,
			.read_buff = &adc_buff,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad405x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
