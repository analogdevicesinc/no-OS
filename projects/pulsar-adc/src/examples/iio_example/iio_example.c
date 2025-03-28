/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example header for eval-pulsar_adc project
 *   @author Axel Haslam (ahaslam@baylibre.com)
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

#include <stdio.h>

#include "iio_example.h"
#include "common_data.h"
#include "iio.h"
#include "pulsar_adc.h"
#include "iio_pulsar_adc.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "iio_app.h"

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int iio_example_main()
{
	struct pulsar_adc_iio_dev *dev;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	struct pulsar_adc_iio_init_param pulsar_adc_iio_ip = {
		.init_param = &pulsar_adc_init_param,
		.ref_voltage_mv = PULSAR_ADC_ADC_REF_VOLTAGE,
	};

	ret = pulsar_adc_iio_init(&dev, &pulsar_adc_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("pulsar_adc", dev,
			       dev->iio_dev, &adc_buff, NULL, NULL)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = pulsar_adc_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_info("Error: iio_app_init: %d\n", ret);
		pulsar_adc_iio_remove(dev);
		return ret;
	}

	ret = iio_app_run(app);
	if (ret)
		pr_info("Error: iio_app_run: %d\n", ret);

	iio_app_remove(app);

	pulsar_adc_iio_remove(dev);

	return ret;
}
