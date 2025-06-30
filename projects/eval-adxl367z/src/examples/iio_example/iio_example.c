/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for eval-adxl367z project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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

#include "iio_example.h"
#include "iio_adxl367.h"
#include "common_data.h"
#include "no_os_util.h"

#define DATA_BUFFER_SIZE 400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 4 * sizeof(int16_t)];

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	int ret;
	struct adxl367_iio_dev *adxl367_iio_desc;
	struct adxl367_iio_init_param adxl367_iio_ip;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 4 * sizeof(int16_t)
	};

	adxl367_iio_ip.adxl367_initial_param = &init_param;
	ret = adxl367_iio_init(&adxl367_iio_desc, &adxl367_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl367",
			.dev = adxl367_iio_desc,
			.dev_descriptor = adxl367_iio_desc->iio_dev,
			.read_buff = &accel_buff,
		}
	};

#ifdef MAXIM_PLATFORM
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adxl367;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_adxl367:
	adxl367_iio_remove(adxl367_iio_desc);
exit:
	if (ret)
		printf("Error!\n");
	return ret;
#endif

#ifdef XILINX_PLATFORM
	return iio_app_run(NULL, 0, iio_devices, NO_OS_ARRAY_SIZE(iio_devices));
#endif
}
