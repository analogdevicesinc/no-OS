/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for adaq776x-1fmcz project.
 *   @author Celine Joy Capua (celinejoy.capua@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "iio_example.h"
#include <stdio.h>
#include <sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "clk_axi_clkgen.h"
#include "xil_printf.h"
#include "xilinx_irq.h"
#include "iio_ad77681.h"
#include "spi_engine.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_error.h"


int iio_example_main()
{
	int ret;
	struct ad77681_iio_dev *ad77681_iio_desc;
	struct ad77681_iio_dev_init_param ad77681_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct axi_clkgen *clkgen;

	ad77681_iio_ip.ad77681_init_param = &adaq776x1_init_param;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = axi_clkgen_init(&clkgen, &clkgen_init);
	if (ret != 0) {
		printf("error: axi_clkgen_init() failed\n");
		return -1;
	} else {
		printf("success: axi_clkgen_init() passed\n");
	}

	ret = axi_clkgen_set_rate(clkgen, 80000000);
	if (ret != 0) {
		return -1;
	} else {
		printf("success: axi_clkgen_set_rate() passed\n");
	}

	ret = ad77681_iio_init(&ad77681_iio_desc, &ad77681_iio_ip);
	if (ret)
		return ret;

	/** Declaring iio_devices structure */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adaq7769-1",
			.dev = ad77681_iio_desc,
			.dev_descriptor = ad77681_iio_desc->iio_dev,
		},
	};

	/** Initializing IIO app init param. */
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adaq776x1_uart_ip;

	/** Initializing IIO app. */
	ret = iio_app_init(&app, app_init_param);

	if (ret)
		return ret;

	/** Running the IIO app (use iio_info or osc in terminal). */
	return iio_app_run(app);
}
