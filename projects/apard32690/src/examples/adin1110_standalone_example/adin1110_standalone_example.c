/***************************************************************************//**
 *   @file   adin1110_standalone_example.c
 *   @brief  Implementation of the ADIN1110 standalone example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <stdio.h>
#include "common_data.h"

#include "no_os_error.h"
#include "adin1110.h"
#include "iio_adin1110.h"
#include "iio_app.h"
#include "iio.h"

uint8_t iio_data_buffer[32];

/***************************************************************************//**
 * @brief Configure the ADIN1110 and read the device ID.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int adin1110_standalone_example_main()
{
	struct iio_app_desc *app;
	struct adin1110_iio_desc *adin1110_iio;
	struct no_os_uart_desc *uart_desc;
	struct adin1110_desc *adin1110;
	uint32_t device_id;
	int ret;

	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = 32
	};

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adin1110_init(&adin1110, &adin1110_ip);
	if (ret) {
		printf("Error during ADIN1110 config (%d)\n", ret);
		return ret;
	}

	ret = adin1110_reg_read(adin1110, 0x1, &device_id);
	if (ret) {
		printf("Error reading the ADIN1110's device id (%d)\n", ret);
		goto out;
	}

	printf("Got device id 0x%X\n", device_id);

	struct adin1110_iio_init_param adin1110_iio_ip = {
		.dev = adin1110
	};

	ret = adin1110_iio_init(&adin1110_iio, &adin1110_iio_ip);
	if (ret)
		goto out;

	struct iio_app_init_param app_init_param = { 0 };
	struct iio_app_device iio_devices[] = {
		{
			.name = "adin1110",
			.dev = adin1110_iio,
			.dev_descriptor = adin1110_iio->iio_dev,
			.read_buff = &buff
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto out;

	return iio_app_run(app);

out:
	adin1110_remove(adin1110);

	return 0;
}
