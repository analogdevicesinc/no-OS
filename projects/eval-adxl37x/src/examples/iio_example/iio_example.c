/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for eval-adxl37x project.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_adxl37x.h"
#include "common_data.h"
#include "iio_app.h"

/*
 * The IIO buffer stores each axis sample at scan_type.storagebits (32 bits =
 * sizeof(int)), NOT the 2-byte FIFO wire width. Size for the worst case of all
 * 3 axes enabled so any 1/2/3-axis selection fits (avoids -ENOMEM in
 * iio_buffer_init when bytes_per_scan * samples exceeds this fixed buffer).
 */
#define IIO_IGNORE_BUFF_OVERRUN_ERR 1
#define IIO_ADXL37X_AXES_MAX 3
static uint8_t iio_data_buf[MAX_SAMPLES * IIO_ADXL37X_AXES_MAX * sizeof(int)];

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct adxl37x_iio_dev *adxl37x_iio_desc;
	struct adxl37x_iio_dev_init_param adxl37x_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buf,
		.size = MAX_SAMPLES * IIO_ADXL37X_AXES_MAX * sizeof(int),
	};

	adxl37x_iio_ip.adxl37x_dev_init = &adxl37x_init_param;
	ret = adxl37x_iio_init(&adxl37x_iio_desc, &adxl37x_iio_ip);
	if (ret)
		return ret;

	ret = adxl37x_write_mask(adxl37x_iio_desc->adxl37x_dev,
				 ADXL37X_POWER_CTL,
				 ADXL37X_POWER_CTL_HPF_DIS_MSK |
				 ADXL37X_POWER_CTL_FIL_SETTLE_MSK,
				 ADXL37X_POWER_CTL_HPF_DIS_MODE(1) |
				 ADXL37X_POWER_CTL_FIL_SETTLE_MODE(1));
	if (ret)
		goto error_dev;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl37x",
			.dev = adxl37x_iio_desc,
			.dev_descriptor = adxl37x_iio_desc->iio_dev,
			.read_buff = &accel_buff,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adxl37x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error_dev;

	ret = iio_app_run(app);

	iio_app_remove(app);
error_dev:
	adxl37x_iio_remove(adxl37x_iio_desc);

	return ret;
}
