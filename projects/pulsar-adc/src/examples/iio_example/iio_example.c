/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example header for eval-pulsar_adc project
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>

#include "iio_example.h"
#include "common_data.h"
#include "iio.h"
#include "pulsar_adc.h"
#include "iio_pulsar_adc.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "iio_app.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int iio_example_main()
{
	struct pulsar_adc_iio_dev *dev;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	struct pulsar_adc_iio_init_param pulsar_adc_iio_ip = {
		.init_param = &pulsar_adc_init_param,
		.ref_voltage_mv = PULSAR_ADC_ADC_REF_VOLTAGE,
	};

	ret = pulsar_adc_iio_init(&dev, &pulsar_adc_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE( "pulsar_adc", dev,
				dev->iio_dev, &adc_buff, NULL, NULL)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = pulsar_adc_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_info("Error: iio_app_init: %d\n", ret);
		pulsar_adc_iio_remove(dev);
		return ret;
	}

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\n", ret);

	iio_app_remove(app);

	pulsar_adc_iio_remove(dev);

	return ret;
}
