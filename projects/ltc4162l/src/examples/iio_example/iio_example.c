/********************************************************************************
 *   @file   iio_example.c
 *   @brief  IIO example code for the ltc4162l project
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "common_data.h"
#include "no_os_print_log.h"


int iio_example_main()
{

	int ret;
	struct ltc4162l_iio_device *ltc4162l_iio_dev;
	struct ltc4162l_iio_init_param ltc4162l_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ltc4162l_iio_ip.init_param = &ltc4162l_ip;
	ret = ltc4162l_iio_init(&ltc4162l_iio_dev, &ltc4162l_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc4162l",
			.dev = ltc4162l_iio_dev,
			.dev_descriptor = ltc4162l_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error;

	ret = iio_app_run(app);
	if (ret)
		pr_err("Error: iio_app_run: %d\r\n", ret);

	iio_app_remove(app);

error:
	ltc4162l_iio_remove(ltc4162l_iio_dev);
	return ret;

}
