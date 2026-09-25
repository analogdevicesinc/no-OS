/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for LTC4284 project
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
