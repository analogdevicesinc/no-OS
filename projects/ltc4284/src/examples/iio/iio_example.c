/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for LTC4284 project
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
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

#include "common_data.h"
#include "iio_ltc4284.h"
#include "no_os_print_log.h"
#include "iio_app.h"

/**
 * @brief IIO example main execution
 * @return 0 in case of success, negative error code otherwise
 */
int example_main()
{
	struct ltc4284_iio_desc *ltc4284_iio_desc;
	struct ltc4284_iio_init_param ltc4284_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};
	int ret;

	pr_info("LTC4284 IIO Example\n");
	pr_info("===================\n\n");

	ltc4284_iio_ip.ltc4284_init = &ltc4284_ip;

	ret = ltc4284_iio_init(&ltc4284_iio_desc, &ltc4284_iio_ip);
	if (ret) {
		pr_err("LTC4284 IIO initialization failed: %d\n", ret);
		return ret;
	}

	pr_info("LTC4284 IIO initialized successfully\n");
	pr_info("IIO device: ltc4284\n");
	pr_info("Channels: VIN, IIN, VOUT, Power, Energy\n\n");

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltc4284",
			.dev = ltc4284_iio_desc,
			.dev_descriptor = ltc4284_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ltc4284_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("IIO app initialization failed: %d\n", ret);
		goto remove_iio;
	}

	pr_info("IIO app initialized successfully\n");
	pr_info("\nAccess via:\n");
	pr_info("  - pyadi-iio: uri='serial:/dev/ttyUSB0,115200'\n");
	pr_info("  - libiio: iiod -F ttyUSB0,115200\n\n");
	pr_info("Running IIO app (Ctrl+C to stop)...\n\n");

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio:
	ltc4284_iio_remove(ltc4284_iio_desc);

	if (ret)
		pr_err("IIO example failed: %d\n", ret);
	else
		pr_info("IIO example completed successfully\n");

	return ret;
}
