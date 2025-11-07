/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for eval-adis16607 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include "iio_adis16607.h"
#include "iio_trigger.h"
#include "iio_app.h"
#include "no_os_pwm.h"
#include "no_os_print_log.h"
#include "common_data.h"
#include "maxim_pwm.h"

#define DATA_BUFFER_SIZE 400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 13 * sizeof(int)];

/**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
 */
int example_main()
{
	int ret;
	struct adis_iio_dev *adis16607_iio_desc;
	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 13 * sizeof(int)
	};

	struct iio_hw_trig *adis16607_trig_desc;
	struct no_os_irq_ctrl_desc *adis16607_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	if (adis16607_ip.use_fifo)
		adis16607_gpio_trig_ip.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING;

	ret = adis16607_iio_init(&adis16607_iio_desc, &adis16607_ip, NULL);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&adis16607_irq_desc, &adis16607_gpio_irq_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_set_priority(adis16607_irq_desc, adis16607_gpio_trig_ip.irq_id,
				     1);
	if (ret)
		goto exit;

	adis16607_gpio_trig_ip.irq_ctrl = adis16607_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adis16607_trig_desc, &adis16607_gpio_trig_ip);
	if (ret)
		goto exit;

	adis16607_iio_desc->hw_trig_desc = adis16607_trig_desc;

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adis16607",
			.dev = adis16607_iio_desc,
			.dev_descriptor = adis16607_iio_desc->iio_dev,
			.read_buff = &data_buff,
			.default_trigger_id = "trigger0",
		}
	};

	adis_iio_trig_desc.is_synchronous = false;

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADIS16607_GPIO_TRIG_NAME, adis16607_trig_desc,
				&adis_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adis16607_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = adis16607_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;

	/* Update the reference to iio_desc */
	adis16607_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

exit:
	iio_hw_trig_remove(adis16607_trig_desc);
	no_os_irq_ctrl_remove(adis16607_irq_desc);
	adis16607_iio_remove(adis16607_iio_desc);
	if (ret)
		pr_info("Error!\n");
	return ret;
}
