/********************************************************************************
 *   @file   iio_example.c
 *   @brief  IIo example code for eval-ad7490sdz project
 *   @author Radu Sabau (radu.sabau@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE 400
#endif

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 16 * sizeof(int)];

/*******************************************************************************
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 *******************************************************************************/
int example_main()
{
	int ret;
	struct ad7490_iio_desc *ad7490_iio_desc;
	struct ad7490_iio_init_param ad7490_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer voltage_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 16 * sizeof(int)
	};
	struct iio_app_init_param app_init_param = {0};

	ad7490_iio_ip.ad7490_init = &ad7490_ip;
	ad7490_iio_ip.dout_state = AD7490_DOUT_TRISTATE;
	ad7490_iio_ip.range = AD7490_RANGE_REFIN;
	ad7490_iio_ip.coding = AD7490_CODING_BINARY;
	ret = ad7490_iio_init(&ad7490_iio_desc, &ad7490_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad7490",
			.dev = ad7490_iio_desc,
			.dev_descriptor = ad7490_iio_desc->iio_dev,
			.read_buff = &voltage_buff,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
