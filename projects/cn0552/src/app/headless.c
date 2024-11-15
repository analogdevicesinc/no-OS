/***************************************************************************//**
* @file headless.c
* @brief CN0552 IIO demo project main file.
* @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>

#include "iio.h"
#include "iio_ad7746.h"
#include "iio_app.h"
#include "ad7746.h"
#include "no_os_i2c.h"
#include "aducm3029_i2c.h"
#include "aducm3029_uart.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "platform_init.h"
#include "parameters.h"

int32_t main(void)
{
	int32_t ret;
	struct ad7746_init_param adcip;
	struct ad7746_iio_dev *adciio = NULL;
	struct ad7746_iio_init_param adciio_init;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct no_os_uart_init_param cn0552_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = NULL,
		.platform_ops = &aducm_uart_ops,
	};

	ret = platform_init();
	if (ret < 0)
		goto error;

	adcip.i2c_init = (struct no_os_i2c_init_param) {
		.max_speed_hz = I2C_SPEED,
		.slave_address = AD7746_ADDRESS,
		.platform_ops = &aducm_i2c_ops
	};
	adcip.setup = (struct ad7746_setup) {
		.exc = {
			.excb = AD7746_EXC_PIN_NORMAL,
			.exca = AD7746_EXC_PIN_NORMAL,
			.exclvl = AD7746_EXCLVL_4_DIV_8
		},
	};
	adcip.id = ID_AD7746;

	adciio_init.ad7746_initial = &adcip;
	ret = ad7746_iio_init(&adciio, &adciio_init);
	if (ret < 0)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad7746",
			.dev = adciio,
			.dev_descriptor = adciio->iio_dev,
			.read_buff = NULL,
			.write_buff = NULL
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = cn0552_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
error:
	ad7746_iio_remove(adciio);
	return ret;
}
