/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for eval-adis1650x project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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

#include "iio_trigger_example.h"
#include "no_os_print_log.h"
#include "common_data.h"

#define DATA_BUFFER_SIZE 400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];

/***************************************************************************//**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
 ********************************************************************************/
int iio_trigger_example_main()
{
	int ret;
	struct adis_iio_dev *adis1650x_iio_desc;
	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis1650x_trig_desc;
	struct no_os_irq_ctrl_desc *adis1650x_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adis1650x_iio_init(&adis1650x_iio_desc, &adis1650x_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis1650x_irq_desc, &adis1650x_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis1650x_irq_desc, adis1650x_gpio_trig_ip.irq_id,
				     1);
	if (ret)
		goto exit;

	adis1650x_gpio_trig_ip.irq_ctrl = adis1650x_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis1650x_trig_desc, &adis1650x_gpio_trig_ip);
	if (ret)
		goto exit;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16505-2",
			.dev = adis1650x_iio_desc,
			.dev_descriptor = adis1650x_iio_desc->iio_dev,
			.read_buff = &data_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS1650X_GPIO_TRIG_NAME, adis1650x_trig_desc,
				&adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis1650x_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis1650x_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis1650x_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

exit:
	iio_hw_trig_remove(adis1650x_trig_desc);
	no_os_irq_ctrl_remove(adis1650x_irq_desc);
	adis1650x_iio_remove(adis1650x_iio_desc);
	if (ret)
		pr_info("Error!\n");
	return ret;
}
