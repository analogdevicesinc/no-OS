/***************************************************************************//**
* @file iio_example.c
* @brief Implementation of an example with the IIO interface
* @author Alexandru Ardelean (aardelean@baylibre.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xil_cache.h>

#include "parameters.h"
#include "common_data.h"

#include "iio.h"
#include "ad7606.h"
#include "iio_ad7606.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "xilinx_uart.h"
#include "iio_app.h"

/******************************************************************************/
/************************ Variables & Definitions *****************************/
/******************************************************************************/

#define AD7606X_FMC_SAMPLE_NO		(1000)
#define MAX_SIZE_BASE_ADDR		(AD7606X_FMC_SAMPLE_NO)

static struct xil_uart_init_param uart_extra_ip = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#else
	.type = UART_PS,
	.irq_id = UART_IRQ_ID
#endif
};

static struct no_os_uart_init_param iio_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = UART_OPS,
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	struct ad7606_init_param *init_param;
	struct ad7606_iio_dev *dev;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	if (AD7606X_MODE == AD7606X_MODE_PARALLEL) {
		pr_info("AD7606X Reference Design - Parallel interface.\n");
		init_param = &ad7606x_init_param_parallel;
	} else {
		pr_info("AD7606X Reference Design - Serial interface.\n");
		init_param = &ad7606x_init_param_serial;
	}

	ret = ad7606_iio_init(&dev, init_param);
	if (ret) {
		pr_err("Error initializing.\n");
		return ret;
	}

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE( "ad7606x", dev,
				dev->iio_dev, &adc_buff, NULL, NULL)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("Error: iio_app_init: %d\n", ret);
		goto err_ad7606_remove;
	}

	ret = iio_app_run(app);
	if (ret) {
		pr_err("Error: iio_app_run: %d\n", ret);
		goto error;
	}

error:
	iio_app_remove(app);

err_ad7606_remove:
	ad7606_iio_remove(dev);

	return ret;
}
