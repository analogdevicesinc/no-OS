/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for eval-adis1647x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_trigger_example.h"
#include "no_os_print_log.h"
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DATA_BUFFER_SIZE 400

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
 ********************************************************************************/
int iio_trigger_example_main()
{
	int ret;
	struct adis_iio_dev *adis1647x_iio_desc;
	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1647x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1647x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1647x_iio_init(&adis1647x_iio_desc, &adis1647x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1647x_irq_desc, &adis1647x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1647x_irq_desc, adis1647x_gpio_trig_ip.irq_id,
				     1);
	if (ret)
		goto exit;

	adis1647x_gpio_trig_ip.irq_ctrl = adis1647x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1647x_trig_desc, &adis1647x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16477-1",
			.dev = adis1647x_iio_desc,
			.dev_descriptor = adis1647x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1647X_GPIO_TRIG_NAME, adis1647x_trig_desc,
				&adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1647x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1647x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1647x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

exit:
	iio_hw_trig_remove(adis1647x_trig_desc);
	no_os_irq_ctrl_remove(adis1647x_irq_desc);
	adis1647x_iio_remove(adis1647x_iio_desc);
	if (ret)
		pr_info("Error!\n");
	return ret;
}
