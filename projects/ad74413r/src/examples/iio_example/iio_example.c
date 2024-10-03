/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for AD74413R project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "iio_example.h"
#include "iio_ad74413r.h"
#include "common_data.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DATA_BUFFER_SIZE 400

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(uint32_t) * 8];

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
	struct ad74413r_iio_desc *ad74413r_iio_desc;
	struct ad74413r_iio_desc_init_param ad74413r_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t) * 8
	};
	struct ad74413r_init_param ad74413r_ip = {
		.chip_id = AD74412R,
		.comm_param = ad74413r_spi_ip
	};
	struct iio_app_init_param app_init_param = { 0 };

	ad74413r_iio_ip.ad74413r_init_param = &ad74413r_ip;
	ad74413r_iio_ip.channel_configs[0] = (struct ad74413r_channel_config) {
		.enabled = true,
		.function = AD74413R_DIGITAL_INPUT
	};
	ad74413r_iio_ip.channel_configs[1] = (struct ad74413r_channel_config) {
		.enabled = true,
		.function = AD74413R_VOLTAGE_IN
	};
	ad74413r_iio_ip.channel_configs[2] = (struct ad74413r_channel_config) {
		.enabled = true,
		.function = AD74413R_VOLTAGE_OUT
	};
	ad74413r_iio_ip.channel_configs[3] = (struct ad74413r_channel_config) {
		.enabled = true,
		.function = AD74413R_CURRENT_IN_EXT
	};

	ret = ad74413r_iio_init(&ad74413r_iio_desc, &ad74413r_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad74413r",
			.dev = ad74413r_iio_desc,
			.dev_descriptor = ad74413r_iio_desc->iio_dev,
			.read_buff = &buff,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad74413r_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
