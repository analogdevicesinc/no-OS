/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for iio_demo_freeRTOS project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_example.h"
#include "iio_adc_demo.h"
#include "iio_dac_demo.h"
#include "common_data.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	int32_t status;

	/* adc instance descriptor. */
	struct adc_demo_desc *adc_desc;

	/* dac instance descriptor. */
	struct dac_demo_desc *dac_desc;

	/* IIO application descriptor. */
	struct iio_app_desc *app;

	/* IIO application initialization parameters. */
	struct iio_app_init_param app_init_param = { 0 };

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_data_buffer dac_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	status = adc_demo_init(&adc_desc, &adc_init_par);
	if (status)
		return status;

	status = dac_demo_init(&dac_desc, &dac_init_par);
	if (status)
		goto error_dac_buffer;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adc_demo", adc_desc,
			       &adc_demo_iio_descriptor,&adc_buff, NULL, NULL),
		IIO_APP_DEVICE("dac_demo", dac_desc,
			       &dac_demo_iio_descriptor,NULL, &dac_buff, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_demo_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		goto error_iio_app;

	status = iio_app_run(app);
	if (status)
		iio_app_remove(app);

error_iio_app:
	dac_demo_remove(dac_desc);
error_dac_buffer:
	adc_demo_remove(adc_desc);
	return status;
}
