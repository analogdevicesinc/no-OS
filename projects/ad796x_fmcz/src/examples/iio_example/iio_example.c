/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for iio project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
