/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for ada4250_ardz project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_error.h"
#include "ada4250.h"
#include "iio_ada4250.h"
#include "iio_app.h"

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int32_t ret;
	struct ada4250_dev *ada4250_dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ada4250_init(&ada4250_dev, &ada4250_ip);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ADA4250", ada4250_dev,
			       &ada4250_iio_descriptor,
			       NULL, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = ada4250_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
