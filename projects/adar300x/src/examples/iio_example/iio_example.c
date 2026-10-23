/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the ADAR300X project.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdint.h>
#include "iio_example.h"
#include "common_data.h"
#include "iio_adar300x.h"
#include "iio_app.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* Room for every RAM beamstate of every beam in a single buffer push */
#define ADAR300X_IIO_BUFFER_SIZE	(ADAR300X_RAM_STATES_PER_BEAM * \
					 ADAR300X_IIO_BEAMS * \
					 ADAR300X_UNPACKED_BEAMSTATE_LEN)

static uint8_t adar300x_iio_buffer[ADAR300X_IIO_BUFFER_SIZE];

int iio_example_main(void)
{
	struct adar300x_iio_dev_init_param adar300x_iio_ip = {
		.adar300x_dev_init = &adar300x_ip,
	};
	struct iio_data_buffer write_buff = {
		.buff = adar300x_iio_buffer,
		.size = sizeof(adar300x_iio_buffer),
	};
	struct iio_app_init_param app_init_param = { 0 };
	struct adar300x_iio_dev *adar300x_iio_dev;
	struct iio_app_desc *app;
	int ret;

	ret = adar300x_iio_init(&adar300x_iio_dev, &adar300x_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adar3000",
			.dev = adar300x_iio_dev,
			.dev_descriptor = adar300x_iio_dev->iio_dev,
			.write_buff = &write_buff,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adar300x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error_iio_dev;

	ret = iio_app_run(app);

	iio_app_remove(app);
error_iio_dev:
	adar300x_iio_remove(adar300x_iio_dev);

	if (ret)
		pr_err("IIO example failed: %d\n", ret);

	return ret;
}
