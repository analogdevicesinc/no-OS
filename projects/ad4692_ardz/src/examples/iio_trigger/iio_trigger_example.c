/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for AD4692 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "common_data.h"
#include "parameters.h"
#include "iio_ad4692.h"
#include "iio_app.h"
#include "iio_trigger.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

#define DATA_BUFFER_SIZE	400

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 16 * sizeof(uint32_t)];

/*
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
 */
int example_main()
{
	int ret;
	struct iio_hw_trig *ad4692_trig_desc;
	struct no_os_irq_ctrl_desc *ad4692_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct ad4692_iio_desc *ad4692_iio_desc;
	/*
	 * All non-manual modes are compatible with this trigger example.
	 * In every non-manual mode GPIO0 produces a falling edge at the end
	 * of each conversion (DATA_READYb or ADC_BUSY), which is what the
	 * hardware trigger uses. AD4692_MANUAL_MODE is the only incompatible
	 * mode because the IIO driver does not register a trigger handler for it.
	 */
	struct ad4692_iio_desc_init_param ad4692_iio_ip = {
		.ad4692_init_param = &ad4692_ip,
	};
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t) * 16,
	};

	struct iio_hw_trig_init_param ad4692_gpio_trig_ip = {
		.irq_id = GPIO_DREADY_PIN_NUM,
		.irq_trig_lvl = NO_OS_IRQ_EDGE_FALLING,
		.cb_info = {
			.event = NO_OS_EVT_GPIO,
			.peripheral = NO_OS_GPIO_IRQ,
			.handle = GPIO_HANDLE,
		},
		.name = AD4692_GPIO_TRIG_NAME,
	};

	struct iio_trigger ad4692_iio_trig_desc = {
		.is_synchronous = false,
		.enable = iio_trig_enable,
		.disable = iio_trig_disable
	};

	/* NVIC Interrupt Controller specific for Maxim platform. */
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_desc_param = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_desc_param);
	if (ret)
		goto exit;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto ad4692_irq_ctrl_remove;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&ad4692_irq_desc, &ad4692_gpio_irq_ip);
	if (ret)
		goto ad4692_irq_ctrl_remove;

	ret = no_os_irq_set_priority(ad4692_irq_desc, ad4692_gpio_trig_ip.irq_id,
				     1);
	if (ret)
		goto irq_remove;

	ret = ad4692_iio_init(&ad4692_iio_desc, &ad4692_iio_ip);
	if (ret)
		goto irq_remove;

	ad4692_gpio_trig_ip.irq_ctrl = ad4692_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&ad4692_trig_desc, &ad4692_gpio_trig_ip);
	if (ret)
		goto iio_ad4692_remove;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad4692",
			.dev = ad4692_iio_desc,
			.dev_descriptor = ad4692_iio_desc->iio_dev,
			.read_buff = &buff,
			.default_trigger_id = "trigger0",
		}
	};

	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(AD4692_GPIO_TRIG_NAME, ad4692_trig_desc,
				&ad4692_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad4692_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = ad4692_irq_desc;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto hw_trig_remove;

	/* Update the reference to iio_desc */
	ad4692_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

hw_trig_remove:
	iio_hw_trig_remove(ad4692_trig_desc);
iio_ad4692_remove:
	ad4692_iio_remove(ad4692_iio_desc);
irq_remove:
	no_os_irq_ctrl_remove(ad4692_irq_desc);
ad4692_irq_ctrl_remove:
	no_os_irq_ctrl_remove(nvic_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
