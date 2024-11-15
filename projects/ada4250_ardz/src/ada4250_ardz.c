/***************************************************************************//**
 *   @file   ada4250_ardz/src/ada4250_ardz.c
 *   @brief  Implementation of Main Function.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <sys/platform.h>
#include "adi_initialize.h"
#include "no_os_error.h"
#include "iio.h"
#include "parameters.h"
#include "ada4250.h"
#include "iio_ada4250.h"
#include "no_os_spi.h"
#include "aducm3029_spi.h"
#include "aducm3029_uart.h"
#include "iio_app.h"

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main()
{
	int32_t ret;

	adi_initComponents();

	struct ada4250_dev *ada4250_dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct aducm_spi_init_param spi_param = {
		.continuous_mode = true,
		.dma = false,
		.half_duplex = false,
		.master_mode = MASTER
	};

	struct no_os_spi_init_param init_param = {
		.device_id = 0,
		.chip_select = 1,
		.extra = &spi_param,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = &aducm_spi_ops
	};

	struct ada4250_init_param ada4250_param = {
		.device_id = ADA4250,
		.spi_init = &init_param,
		.refbuf_en = ADA4250_BUF_DISABLE,
		.bias = ADA4250_BIAS_DISABLE,
		.gain = ADA4250_GAIN_8,
		.avdd_v = 5000,
		.offset_nv = 0,
	};

	struct no_os_uart_init_param ada4250_uart_ip = {
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

	ret = ada4250_init(&ada4250_dev, &ada4250_param);
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
