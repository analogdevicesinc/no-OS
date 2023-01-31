/***************************************************************************//**
* @file headless.c
* @brief AD7746-EBZ demo project main file.
* @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#include <stdlib.h>
#include <string.h>

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>
#ifdef IIO_SUPPORT
#include "iio.h"
#include "iio_ad7746.h"
#include "iio_app.h"
#endif
#include "ad7746.h"
#include "no_os_i2c.h"
#include "aducm3029_i2c.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "aducm3029_uart.h"
#include "aducm3029_uart_stdio.h"
#include "platform_init.h"
#include "parameters.h"

int main(void)
{
	int32_t ret;
	char input[10];
	uint32_t capData = 0;
	uint32_t min = 0;
	uint32_t max = 0;
	uint32_t mm = 0;
	uint32_t temperature = 0;
	struct ad7746_init_param adcip;
	struct ad7746_dev *adc = NULL;

	struct no_os_uart_desc *uart;
	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = NULL,
		.platform_ops = &aducm_uart_ops,
	};
#ifdef IIO_SUPPORT
	struct ad7746_iio_dev *adciio = NULL;
	struct ad7746_iio_init_param adciio_init;

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
#endif

	ret = platform_init();
	if (ret < 0)
		goto error;

	adcip.i2c_init = (struct no_os_i2c_init_param) {
		.max_speed_hz = I2C_SPEED,
		.slave_address = AD7746_ADDRESS,
		.platform_ops = &aducm_i2c_ops
	};
	adcip.setup = (struct ad7746_setup) {
		.cap = {
			.capen = true,
			.cin2 = true,
			.capdiff = false,
			.capchop = true
		},
		.vt = {
			.vten = true,
			.vtmd = AD7746_VTMD_INT_TEMP,
			.extref = false,
			.vtshort = false,
			.vtchop = true
		},
		.exc = {
			.clkctrl = false,
			.excon = false,
			.excb = AD7746_EXC_PIN_DISABLED,
			.exca = AD7746_EXC_PIN_NORMAL,
			.exclvl = AD7746_EXCLVL_4_DIV_8
		},
		.config = {
			.vtf = 0,
			.capf = 0,
			.md = AD7746_MODE_CONT
		}
	};
	adcip.id = ID_AD7746;
#ifndef IIO_SUPPORT
	ret = no_os_uart_init(&uart, &uip);
	if (ret < 0)
		goto error;

	no_os_uart_stdio(uart);

	pr_info("Hello!\n");

	ret = ad7746_init(&adc, &adcip);
	if (ret < 0) {
		pr_err("AD7746 initialization failed with: %d\n", ret);
		goto error;
	}
	pr_info("[INIT] AD7746 initialization ok.\n");

	ret = ad7746_set_cap_dac_a(adc, true, 0x42);
	if (ret < 0) {
		pr_err("DACA setting failed with: %d\n", ret);
		goto error;
	}

	pr_info("[CALIB] 1. Remove the ruler and press ENTER. ");
	fflush(stdout);
	scanf("%s", input);
	ret = ad7746_get_cap_data(adc, &min);
	if (ret < 0) {
		pr_err("CAP reading failed with: %d\n", ret);
		goto error;
	}

	pr_info("[CALIB] 2. Place ruler to 51mm (2inch) and press ENTER.");
	fflush(stdout);
	scanf("%s", input);
	ret = ad7746_get_cap_data(adc, &max);
	if (ret < 0) {
		pr_err("CAP reading failed with: %d\n", ret);
		goto error;
	}

	mm = (max - min) / 51;

	pr_info("[DEMO] Move the ruler around, its position will is read and displayed every 2 seconds.\n");
	while(true) {
		ret = ad7746_get_cap_data(adc, &capData);
		if (ret < 0) {
			pr_err("CAP reading failed with: %d\n", ret);
			goto error;
		}

		capData -= min;
		capData /= mm;

		ret = ad7746_get_vt_data(adc, &temperature);
		if (ret < 0) {
			pr_err("CAP reading failed with: %d\n", ret);
			goto error;
		}
		temperature /= 2048;
		temperature -= 4096;

		pr_info("Position: %d mm, Temperature: %d *C\n", capData, temperature);

		no_os_mdelay(2000);
	}

error:
	pr_info("Bye!\n");
	return 0;
#else
	adciio_init.ad7746_initial = &adcip;
	ret = ad7746_iio_init(&adciio, &adciio_init);
	if (ret < 0)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad7746",
			.dev = adciio,
			.dev_descriptor = adciio->iio_dev,
			.read_buff = NULL,
			.write_buff = NULL
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
error:
	ad7746_iio_remove(adciio);
	return ret;
#endif
}
