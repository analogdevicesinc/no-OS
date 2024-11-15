/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for AD74413R project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "iio_example.h"
#include "iio_adt75.h"
#include "no_os_util.h"

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
	struct adt75_iio_desc *adt75_iio_desc;
	struct adt75_iio_init_param adt75_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(int16_t)
	};
	struct iio_app_init_param app_init_param = { 0 };

	adt75_iio_ip.adt75_init_param = &adt75_ip;
	ret = adt75_iio_init(&adt75_iio_desc, &adt75_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adt75",
			.dev = adt75_iio_desc,
			.dev_descriptor = adt75_iio_desc->iio_dev,
			.read_buff = &buff,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adt75_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
