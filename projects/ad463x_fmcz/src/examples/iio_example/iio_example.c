/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example header for ad463x_fmcz project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>

#include "iio_example.h"
#include "common_data.h"
#include "iio.h"
#include "iio_ad463x.h"
#include "no_os_util.h"
#include "iio_app.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int iio_example_main()
{
	struct ad463x_dev *dev;
	struct iio_ad463x *iio_ad463x;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;
	struct iio_data_buffer rd_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	ret = ad463x_init(&dev, &ad463x_init_param);
	if (ret)
		return ret;

#if ADAQ4224_DEV
	/* Apply a gain of 1/3 */
#define AD4224_INITIAL_GAIN_IDX 0

	ret = ad463x_fill_scale_tbl(dev);
	if (ret)
		goto out;

	ret = ad463x_set_pgia_gain(dev, AD4224_INITIAL_GAIN_IDX);
	if (ret)
		goto out;
#endif

	ret = iio_ad463x_init(&iio_ad463x, dev);
	if(ret < 0)
		goto out;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad463x", iio_ad463x, &iio_ad463x->iio_dev_desc,
			       &rd_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = ad463x_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto out;

	ret = iio_app_run(app);
out:
	return ad463x_remove(dev);
}
