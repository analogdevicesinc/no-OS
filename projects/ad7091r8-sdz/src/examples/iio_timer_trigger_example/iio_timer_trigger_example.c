/***************************************************************************//**
 *   @file   iio_timer_trigger_example.c
 *   @brief  Implementation of IIO timer trigger example for eval-ad7091r8-sdz project.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_timer_trigger_example.h"
#include "iio_ad7091r8.h"
#include "common_data.h"
#include "iio_app.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t ad7091r8_data_buffer[IIO_DATA_BUFFER_SIZE];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/

int iio_timer_trigger_example_main()
{
	struct ad7091r8_iio_dev *ad7091r8_iio_desc;
	struct ad7091r8_iio_dev_init_param ad7091r8_iio_param;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct ad7091r8_init_param *ad7091r8_init_p;
	struct iio_data_buffer adc_buff = {
		.buff = (void *)ad7091r8_data_buffer,
		.size = IIO_DATA_BUFFER_SIZE
	};
	int ret;

	/* adc trigger instance descriptor. */
	struct iio_hw_trig *ad7091r8_timer_trig_desc;

	/* adc timer descriptor */
	struct no_os_timer_desc *ad7091r8_timer_desc;

	/* ad7091r8 irq instance descriptor */
	struct no_os_irq_ctrl_desc *ad7091r8_irq_desc;


	/* Use ad7091r4_ip and ad7091r2_ip for AD7091R-4 and AD7091R-2 respectively */
	ad7091r8_init_p = &ad7091r8_ip;
	ad7091r8_iio_param.ad7091r8_dev_init = ad7091r8_init_p;
	ret = ad7091r8_iio_init(&ad7091r8_iio_desc, &ad7091r8_iio_param);
	if (ret)
		return ret;

	ret = no_os_timer_init(&ad7091r8_timer_desc, &ad7091r8_timer_ip);
	if (ret)
		goto remove_iio_ad7091r8;


	ret = no_os_irq_ctrl_init(&ad7091r8_irq_desc, &ad7091r8_timer_irq_ip);
	if (ret)
		goto remove_timer;


	ret = no_os_irq_set_priority(ad7091r8_irq_desc, AD7091R8_TIMER_IRQ_ID, 1);
	if (ret)
		goto remove_irq_ctrl;


	ad7091r8_timer_irq_ip.extra = ad7091r8_timer_desc->extra;

	ad7091r8_timer_trig_ip.irq_ctrl = ad7091r8_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&ad7091r8_timer_trig_desc, &ad7091r8_timer_trig_ip);
	if (ret)
		goto remove_irq_ctrl;

	ret = no_os_timer_start(ad7091r8_timer_desc);
	if (ret)
		goto remove_hw_trigger;

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE(ad7091r8_names[ad7091r8_init_p->device_id],
			       ad7091r8_iio_desc, ad7091r8_iio_desc->iio_dev,
			       &adc_buff, NULL, NULL),
	};

	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(AD7091R8_TIMER_TRIG_NAME, ad7091r8_timer_trig_desc,
				&ad7091r8_iio_timer_trig_desc),
	};

	/* Set device trigger */
	iio_devices[0].default_trigger_id = "trigger0";

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad7091r8_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = NULL;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_hw_trigger;

	/* Update the reference to iio_desc */
	ad7091r8_timer_trig_desc->iio_desc = app->iio_desc;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_hw_trigger:
	iio_hw_trig_remove(ad7091r8_timer_trig_desc);

remove_irq_ctrl:
	no_os_irq_ctrl_remove(ad7091r8_irq_desc);

remove_timer:
	no_os_timer_remove(ad7091r8_timer_desc);

remove_iio_ad7091r8:
	ad7091r8_iio_remove(ad7091r8_iio_desc);

	return ret;
}
