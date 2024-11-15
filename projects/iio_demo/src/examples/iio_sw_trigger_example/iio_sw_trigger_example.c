/***************************************************************************//**
 *   @file   iio_sw_trigger_example.c
 *   @brief  Implementation of IIO sw trigger example for iio_demo project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_sw_trigger_example.h"
#include "iio_adc_demo.h"
#include "iio_dac_demo.h"
#include "common_data.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO sw trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
*******************************************************************************/
int iio_sw_trigger_example_main()
{
	int32_t status;

	/* adc instance descriptor. */
	struct adc_demo_desc *adc_desc;

	/* dac instance descriptor. */
	struct dac_demo_desc *dac_desc;

	/* adc trigger instance descriptor. */
	struct iio_sw_trig *adc_trig_desc;

	/* dac trigger instance descriptor. */
	struct iio_sw_trig *dac_trig_desc;

	/* iio desc */
	struct iio_desc *iio_desc;

	/* iio application desc */
	struct iio_app_desc *app;

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_data_buffer dac_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_app_init_param app_init_param = { 0 };

	status = adc_demo_init(&adc_desc, &adc_init_par);
	if (status)
		return status;

	status = dac_demo_init(&dac_desc, &dac_init_par);
	if (status)
		return status;

	adc_trig_ip.iio_desc = &iio_desc;
	status = iio_sw_trig_init(&adc_trig_desc, &adc_trig_ip);
	if (status)
		return status;

	dac_trig_ip.iio_desc = &iio_desc;
	status = iio_sw_trig_init(&dac_trig_desc, &dac_trig_ip);
	if (status)
		return status;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adc_demo", adc_desc,
			       &adc_demo_iio_descriptor,&adc_buff, NULL, NULL),
		IIO_APP_DEVICE("dac_demo", dac_desc,
			       &dac_demo_iio_descriptor,NULL, &dac_buff, NULL)
	};

	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADC_DEMO_TRIG_NAME, adc_trig_desc,
				&adc_iio_sw_trig_desc),
		IIO_APP_TRIGGER(DAC_DEMO_TRIG_NAME, dac_trig_desc,
				&dac_iio_sw_trig_desc),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_demo_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	// update the reference to iio_desc
	adc_trig_desc->iio_desc = app->iio_desc;
	dac_trig_desc->iio_desc = app->iio_desc;

	return iio_app_run(app);
}
