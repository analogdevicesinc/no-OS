/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for iio project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_example.h"
#include "common_data.h"
#include "ad796x.h"
#include "no_os_util.h"
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	int ret;

	struct ad796x_dev *adc_dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_device *dev_desc;
	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};
	struct scan_type init_scan_type = {
		.sign = 's',
		.realbits = 32,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = false
	};

	ret = ad796x_init(&adc_dev, &ad796x_init_param);
	if (ret) {
		pr_err("Error: ad796x_init: %d\n", ret);
		return ret;
	}

	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = adc_dev->ad796x_core,
		.rx_dmac = adc_dev->axi_dmac,
		.scan_type_common = &init_scan_type,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))DCACHE_INVALIDATE,
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret) {
		pr_err("Error: iio_axi_adc_init: %d\n", ret);
		goto err_adc_init;
	}

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad796x", iio_axi_adc_desc,
			       dev_desc, &adc_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("Error: iio_app_init: %d\n", ret);
		goto err_app_init;
	}

	ret = iio_app_run(app);
	if (ret)
		pr_err("iio_app_run error: %d\n", ret);

	iio_app_remove(app);
err_app_init:
	iio_axi_adc_remove(iio_axi_adc_desc);
err_adc_init:
	ad796x_remove(adc_dev);
	return ret;
}
