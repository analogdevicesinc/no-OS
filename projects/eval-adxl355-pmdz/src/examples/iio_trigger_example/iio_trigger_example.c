/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for eval-adxl355-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "iio_app.h"
#include "parameters.h"
#include "iio_trigger.h"
#include "iio_adxl355.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE 400
#endif


#define ADXL355_GPIO_TRIG_NAME "adxl355-dev0"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 3 * sizeof(int)];

/* GPIO trigger */
struct no_os_irq_init_param adxl355_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

struct iio_hw_trig_init_param adxl355_gpio_trig_ip = {
	.irq_id = ADXL355_GPIO_TRIG_IRQ_ID,
	.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
	.cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADXL355_GPIO_CB_HANDLE,
	},
	.name = ADXL355_GPIO_TRIG_NAME,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct adxl355_iio_dev *adxl355_iio_desc;
	struct adxl355_iio_dev_init_param adxl355_iio_ip;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 3 * sizeof(int)
	};
	struct iio_hw_trig *adxl355_trig_desc;
	struct no_os_irq_ctrl_desc *adxl355_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	/* Initialize IIO device */
	adxl355_iio_ip.adxl355_dev_init = &adxl355_ip;
	ret = adxl355_iio_init(&adxl355_iio_desc, &adxl355_iio_ip);
	if (ret)
		return ret;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adxl355_irq_desc, &adxl355_gpio_irq_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(adxl355_irq_desc, adxl355_gpio_trig_ip.irq_id, 1);
	if (ret)
		return ret;

	adxl355_gpio_trig_ip.irq_ctrl = adxl355_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adxl355_trig_desc, &adxl355_gpio_trig_ip);
	if (ret)
		return ret;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl355",
			.dev = adxl355_iio_desc,
			.dev_descriptor = adxl355_iio_desc->iio_dev,
			.read_buff = &accel_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADXL355_GPIO_TRIG_NAME, adxl355_trig_desc,
				&adxl355_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adxl355_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adxl355_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	// update the reference to iio_desc
	adxl355_trig_desc->iio_desc = app->iio_desc;

	return iio_app_run(app);
}
