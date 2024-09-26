/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for eval-adxrs290-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_example.h"
#include "iio_adxrs290.h"
#include "common_data.h"

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
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	int ret;
	struct adxrs290_dev *adxrs290_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_data_buffer rd_buf = {
		.buff = (void *)GYRO_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	ret = adxrs290_init(&adxrs290_desc, &adxrs290_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxrs290",
			.dev = adxrs290_desc,
			.dev_descriptor = &adxrs290_iio_descriptor,
			.read_buff = &rd_buf,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adxrs290_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
