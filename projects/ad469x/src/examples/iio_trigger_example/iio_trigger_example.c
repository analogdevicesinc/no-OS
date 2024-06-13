/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for AD4692 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
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

#include "common_data.h"
#include "iio_trigger_example.h"
#include "iio_ad4692.h"
#include "iio_app.h"
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
int iio_trigger_example_main()
{
	int ret;
	struct iio_hw_trig *ad4692_trig_desc;
	struct no_os_irq_ctrl_desc *ad4692_irq_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct ad4692_iio_desc *ad4692_iio_desc;
	struct ad4692_iio_desc_init_param ad4692_iio_ip = {
		.ad4692_init_param = &ad4692_ip,
	};
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t) * 16,
	};

	ret = ad4692_iio_init(&ad4692_iio_desc, &ad4692_iio_ip);
	if (ret)
		goto exit;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&ad4692_irq_desc, &ad4692_gpio_irq_ip);
	if (ret)
		goto iio_ad4692_remove;

	ret = no_os_irq_set_priority(ad4692_irq_desc, ad4692_gpio_trig_ip.irq_id,
				     1);
	if (ret)
		goto irq_remove;

	ad4692_gpio_trig_ip.irq_ctrl = ad4692_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&ad4692_trig_desc, &ad4692_gpio_trig_ip);
	if (ret)
		goto irq_remove;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad4692",
			.dev = ad4692_iio_desc,
			.dev_descriptor = ad4692_iio_desc->iio_dev,
			.read_buff = &buff,
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

	// update the reference to iio_desc
	ad4692_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

hw_trig_remove:
	iio_hw_trig_remove(ad4692_trig_desc);
irq_remove:
	no_os_irq_ctrl_remove(ad4692_irq_desc);
iio_ad4692_remove:
	ad4692_iio_remove(ad4692_iio_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
