/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for ad7124-8pmdz project.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "iio_ad7124.h"
#include "common_data.h"
#include "iio_app.h"
#include "no_os_util.h"

#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE	400
#endif

static uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(uint32_t)];

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct ad7124_dev *ad7124_device;
	struct iio_app_desc *app;
	struct iio_data_buffer adc_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t),
	};
	struct iio_app_init_param app_init_param = { 0 };

	ret = ad7124_setup(&ad7124_device, &ad7124_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("ad7124-8", ad7124_device, &iio_ad7124_device,
			       &adc_buff, NULL, NULL)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad7124_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
