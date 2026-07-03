/***************************************************************************//**
 *   @file   eit_iio.c
 *   @brief  IIO EIT example for cn0565 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "ad5940.h"
#include "iio_ad5940.h"
#include "iio_adg2128.h"
#include "iio_app.h"

struct no_os_i2c_desc *i2c;

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct ad5940_iio_dev *ad5940_iio = NULL;
	struct ad5940_iio_init_param ad5940_iio_ip = {
		.ad5940_init = &cn0565_ad5940_ip,
	};
	struct adg2128_iio_dev *adg2128_iio = NULL;

	ret = no_os_i2c_init(&i2c, &cn0565_i2c_ip);
	if (ret)
		return ret;

	ret = ad5940_iio_init(&ad5940_iio, &ad5940_iio_ip);
	if (ret < 0)
		return ret;

	ret = adg2128_iio_init(&adg2128_iio, i2c);
	if (ret < 0)
		return ret;

	struct iio_app_device devices[] = {
		{
			.name = "ad5940",
			.dev = ad5940_iio,
			.dev_descriptor = ad5940_iio->iio,
			.read_buff = NULL,
			.write_buff = NULL
		},
		{
			.name = "adg2128",
			.dev = adg2128_iio,
			.dev_descriptor = adg2128_iio->iio_dev,
		},
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = cn0565_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
