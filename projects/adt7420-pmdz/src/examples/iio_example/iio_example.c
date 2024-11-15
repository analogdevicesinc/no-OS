/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example code for adt7420-pmdz project
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_example.h"
#include "iio_adt7420.h"
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DATA_BUFFER_SIZE 400

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(int16_t)];

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
	int ret;
	struct adt7420_iio_dev *adt7420_iio_desc;
	struct adt7420_iio_init_param adt7420_init_param;
	struct iio_app_desc *app;
	struct iio_data_buffer dbuff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint16_t)
	};
	struct iio_app_init_param app_init_param = { 0 };

	adt7420_init_param.adt7420_dev_init = &adt7420_user_init;
	ret = adt7420_iio_init(&adt7420_iio_desc, &adt7420_init_param);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adt7420",
			.dev = adt7420_iio_desc,
			.dev_descriptor = adt7420_iio_desc->iio_dev,
			.read_buff = &dbuff
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
