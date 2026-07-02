/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for cn0531 project.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include "no_os_error.h"
#include "ad5791.h"
#include "iio_ad5791.h"
#include "iio_app.h"

#define MAX_SIZE_BASE_ADDR		1024

static uint8_t out_buff[MAX_SIZE_BASE_ADDR];

#define DAC_DDR_BASEADDR	((uint32_t)out_buff)

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int32_t status, val;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct ad5791_iio_desc *ad5791_iio_handle;
	struct iio_data_buffer iio_ad5791_read_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	status = ad5791_iio_init(&ad5791_iio_handle, &cn0531_ad5791_iio_ip);
	if (status != 0)
		return status;

	status = ad5791_set_dac_value(ad5791_iio_handle->ad5791_handle, 0);
	if (status != 0)
		return status;

	status = ad5791_get_register_value(ad5791_iio_handle->ad5791_handle,
					   AD5791_REG_CTRL, &val);
	if (status < 0)
		return status;
	val &= ~(AD5791_CTRL_OPGND | AD5791_CTRL_RBUF_MASK);
	status = ad5791_set_register_value(ad5791_iio_handle->ad5791_handle,
					   AD5791_REG_CTRL, (uint32_t)val);
	if (status != 0)
		return status;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad5791", ad5791_iio_handle,
			       ad5791_iio_handle->ad5791_iio_dev,
			       &iio_ad5791_read_buff, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = cn0531_uart_ip;
#ifdef NO_OS_NETWORKING
	app_init_param.wifi_ssid = WIFI_SSID;
	app_init_param.wifi_pwd = WIFI_PWD;
#endif

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}
