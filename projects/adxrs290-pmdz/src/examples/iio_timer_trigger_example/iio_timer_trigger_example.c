/***************************************************************************//**
 *   @file   iio_timer_trigger_example.c
 *   @brief  Implementation of IIO trigger example for iio_demo project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_timer_trigger_example.h"
#include "iio_adxrs290.h"
#include "iio_trigger.h"
#include "common_data.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX_SIZE_BASE_ADDR		3000
static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
#define GYRO_DDR_BASEADDR		((uint32_t)in_buff)

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
int iio_timer_trigger_example_main()
{
	int ret;
	struct adxrs290_dev *adxrs290_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_data_buffer rd_buf = {
		.buff = (void *)GYRO_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};
	struct iio_hw_trig *adxrs290_trig_desc;
	struct no_os_timer_desc *adxrs290_timer_desc;
	struct no_os_irq_ctrl_desc *adxrs290_timer_irq_desc;

	ret = adxrs290_init(&adxrs290_desc, &adxrs290_ip);
	if (ret)
		return ret;

	ret = no_os_timer_init(&adxrs290_timer_desc, &adxrs290_tip);
	if (ret)
		return ret;

	adxrs290_timer_irq_ip.extra = adxrs290_timer_desc->extra;

	ret = no_os_irq_ctrl_init(&adxrs290_timer_irq_desc, &adxrs290_timer_irq_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(adxrs290_timer_irq_desc, ADXRS290_TIMER_IRQ_ID, 1);
	if (ret)
		return ret;

	adxrs290_timer_trig_ip.irq_ctrl = adxrs290_timer_irq_desc;

	adxrs290_timer_trig_ip.cb_info.handle = adxrs290_timer_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adxrs290_trig_desc, &adxrs290_timer_trig_ip);
	if (ret)
		return ret;

	ret = no_os_timer_start(adxrs290_timer_desc);
	if (ret)
		return ret;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adxrs290",
			.dev = adxrs290_desc,
			.dev_descriptor = &adxrs290_iio_descriptor,
			.read_buff = &rd_buf,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADXRS290_TIMER_TRIG_NAME, adxrs290_trig_desc,
				&adxrs290_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adxrs290_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adxrs290_timer_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	// update the reference to iio_desc
	adxrs290_trig_desc->iio_desc = app->iio_desc;

	return iio_app_run(app);
}
