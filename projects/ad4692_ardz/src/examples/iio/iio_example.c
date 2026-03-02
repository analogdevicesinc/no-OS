/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Source file for iio example.
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
#include "iio_ad4692.h"
#include "parameters.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "iio_app.h"

#define DATA_BUFFER_SIZE	400

static uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 16 * sizeof(uint32_t)];

int example_main()
{
	int ret;

	/** IIO descriptor and initialization parameter. */
	struct ad4692_iio_desc *ad4692_iio_desc;
	struct ad4692_iio_desc_init_param ad4692_iio_ip = {
		.ad4692_init_param = &ad4692_ip,
	};

	/** IIO app. */
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct iio_data_buffer read_buff = {
		.buff = (void *)iio_data_buffer,
		.size = sizeof(iio_data_buffer),
	};

	ret = ad4692_iio_init(&ad4692_iio_desc, &ad4692_iio_ip);
	if (ret)
		goto exit;

	/** Declaring iio_devices structure */
	struct iio_app_device iio_devices[] = {
		{
			.name = "ad4692",
			.dev = ad4692_iio_desc,
			.dev_descriptor = ad4692_iio_desc->iio_dev,
			.read_buff = &read_buff,
		}
	};

	/** Initializing IIO app init param. */
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad4692_uart_ip;

	/** Initializing IIO app. */
	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto iio_ad4692_remove;

	/** Running the IIO app (use iio_info or osc in terminal). */
	ret = iio_app_run(app);

	iio_app_remove(app);

iio_ad4692_remove:
	ad4692_iio_remove(ad4692_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
