/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for eval-adxl313z project.
 *   @author GMois (george.mois@analog.com)
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

#include <errno.h>
#include "iio_example.h"
#include "iio_adxl313.h"
#include "common_data.h"

#define DATA_BUFFER_SIZE 400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 3 * sizeof(int)];

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	int ret;
	char *dev_name = "adxl312";
	struct adxl313_iio_dev *adxl313_iio_desc;
	struct adxl313_iio_dev_init_param adxl313_init_par;
	struct iio_app_desc *app;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 3 * sizeof(int)
	};
	struct iio_app_init_param app_init_param = { 0 };

	adxl313_init_par.adxl313_dev_init = &adxl313_user_init;
	ret = adxl313_iio_init(&adxl313_iio_desc, &adxl313_init_par);
	if (ret)
		return ret;

	switch (adxl313_iio_desc->adxl313_dev->dev_type) {
	case ID_ADXL312:
		dev_name = "adxl312";
		break;
	case ID_ADXL313:
		dev_name = "adxl313";
		break;
	case ID_ADXL314:
		dev_name = "adxl314";
		break;
	default:
		return -ENODEV;
	}

	struct iio_app_device iio_devices[] = {
		{
			.name = dev_name,
			.dev = adxl313_iio_desc,
			.dev_descriptor = adxl313_iio_desc->iio_dev,
			.read_buff = &accel_buff
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
