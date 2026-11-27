/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for the AD4170 project.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include "iio_ad4170.h"
#include "iio_app.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "no_os_util.h"

#define DATA_BUFFER_SIZE	400

static uint8_t iio_data_buffer[DATA_BUFFER_SIZE * AD4170_NUM_CHANNELS *
						sizeof(uint32_t)];

/***************************************************************************//**
 * @brief IIO example main execution.
 * @return ret - Result of the example execution.
*******************************************************************************/
int example_main()
{
	struct ad4170_iio_dev_init_param ad4170_iio_ip = {
		.ad4170_init_param = &ad4170_ip,
		.vref_mv = AD4170_VREF_MV,
	};
	struct iio_app_init_param app_init_param = { 0 };
	struct ad4170_iio_dev *ad4170_iio_desc;
	struct no_os_uart_desc *uart_desc;
	struct iio_app_desc *app;
	int ret;

	struct iio_data_buffer read_buff = {
		.buff = (void *)iio_data_buffer,
		.size = sizeof(iio_data_buffer),
	};

	ret = no_os_uart_init(&uart_desc, &ad4170_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("AD4170 IIO Example\r\n");

	ret = ad4170_iio_init(&ad4170_iio_desc, &ad4170_iio_ip);

	no_os_uart_stdio(NULL);
	no_os_uart_remove(uart_desc);

	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad4170",
			.dev = ad4170_iio_desc,
			.dev_descriptor = ad4170_iio_desc->iio_dev,
			.read_buff = &read_buff,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad4170_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto iio_ad4170_remove;

	ret = iio_app_run(app);

	iio_app_remove(app);

iio_ad4170_remove:
	ad4170_iio_remove(ad4170_iio_desc);
exit:
	if (ret)
		pr_info("Error!\r\n");

	return ret;
}
