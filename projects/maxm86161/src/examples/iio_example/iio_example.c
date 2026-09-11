/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the MAXM86161 driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "iio_maxm86161.h"
#include "iio_app.h"
#include "no_os_circular_buffer.h"
#include "no_os_delay.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "parameters.h"

#define DATA_BUFFER_SIZE 512

int example_main(void)
{
	struct maxm86161_iio_desc *maxm86161_iio;
	struct no_os_irq_ctrl_desc *irq_ctrl;
	struct iio_device *iio_dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	ret = no_os_irq_ctrl_init(&irq_ctrl, &maxm86161_gpio_irq_ip);
	if (ret) {
		pr_err("IRQ ctrl init failed: %d\n", ret);
		return ret;
	}

	struct maxm86161_iio_init_param maxm86161_iio_ip = {
		.drv_init_param = &maxm86161_ip,
		.fifo_watermark = 50,
		.fifo_buf_size = DATA_BUFFER_SIZE,
		.irq_ctrl = irq_ctrl,
		.irq_id = MAXM86161_GPIO_TRIG_IRQ_ID,
	};

	pr_info("MAXM86161 IIO Example\n");

	ret = maxm86161_iio_init(&maxm86161_iio, &maxm86161_iio_ip);
	if (ret) {
		pr_err("MAXM86161 IIO init failed: %d\n", ret);
		no_os_irq_ctrl_remove(irq_ctrl);
		return ret;
	}

	ret = maxm86161_iio_get_dev_descriptor(maxm86161_iio, &iio_dev);
	if (ret)
		goto err;

	struct iio_app_device iio_devices[] = {
		{
			.name = "maxm86161",
			.dev = maxm86161_iio,
			.dev_descriptor = iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = (struct no_os_uart_init_param) {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto err;

	ret = iio_app_run(app);

	iio_app_remove(app);

err:
	maxm86161_iio_remove(maxm86161_iio);
	no_os_irq_ctrl_remove(irq_ctrl);
	return ret;
}
