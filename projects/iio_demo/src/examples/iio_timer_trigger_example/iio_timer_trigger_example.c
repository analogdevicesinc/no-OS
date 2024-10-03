/***************************************************************************//**
 *   @file   IIO_SW_TRIGGER_EXAMPLE.c
 *   @brief  Implementation of IIO trigger example for iio_demo project.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_timer_trigger_example.h"
#include "iio_adc_demo.h"
#include "iio_dac_demo.h"
#include "common_data.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
*******************************************************************************/
int iio_timer_trigger_example_main()
{
	int32_t ret;

	/* adc instance descriptor. */
	struct adc_demo_desc *adc_desc;

	/* dac instance descriptor. */
	struct dac_demo_desc *dac_desc;

	/* adc trigger instance descriptor. */
	struct iio_hw_trig *adc_timer_trig_desc;

	/* dac trigger instance descriptor. */
	struct iio_hw_trig *dac_timer_trig_desc;

	/* adc timer descriptor */
	struct no_os_timer_desc *adc_demo_tim_desc;

	/* dac timer descriptor */
	struct no_os_timer_desc *dac_demo_tim_desc;

	/* adc demo irq instance descriptor */
	struct no_os_irq_ctrl_desc *adc_demo_irq_desc;

	/* dac demo irq instance descriptor */
	struct no_os_irq_ctrl_desc *dac_demo_irq_desc;

	/* iio application desc */
	struct iio_app_desc *app;

	struct iio_data_buffer adc_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_data_buffer dac_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};

	struct iio_app_init_param app_init_param = { 0 };

	ret = adc_demo_init(&adc_desc, &adc_init_par);
	if (ret)
		return ret;

	ret = dac_demo_init(&dac_desc, &dac_init_par);
	if (ret)
		return ret;

	ret = no_os_timer_init(&adc_demo_tim_desc, &adc_demo_tip);
	if (ret)
		return ret;

	ret = no_os_timer_init(&dac_demo_tim_desc, &dac_demo_tip);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&adc_demo_irq_desc, &adc_demo_timer_irq_ip);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&dac_demo_irq_desc, &dac_demo_timer_irq_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(adc_demo_irq_desc, ADC_DEMO_TIMER_IRQ_ID, 1);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(dac_demo_irq_desc, DAC_DEMO_TIMER_IRQ_ID, 1);
	if (ret)
		return ret;

	adc_demo_timer_irq_ip.extra = adc_demo_tim_desc->extra;
	dac_demo_timer_irq_ip.extra = dac_demo_tim_desc->extra;

	adc_demo_timer_trig_ip.irq_ctrl = adc_demo_irq_desc;
	dac_demo_timer_trig_ip.irq_ctrl = dac_demo_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&adc_timer_trig_desc, &adc_demo_timer_trig_ip);
	if (ret)
		return ret;

	ret = iio_hw_trig_init(&dac_timer_trig_desc, &dac_demo_timer_trig_ip);
	if (ret)
		return ret;

	ret = no_os_timer_start(adc_demo_tim_desc);
	if (ret)
		return ret;

	ret = no_os_timer_start(dac_demo_tim_desc);
	if (ret)
		return ret;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adc_demo", adc_desc,
			       &adc_demo_iio_descriptor,&adc_buff, NULL, NULL),
		IIO_APP_DEVICE("dac_demo", dac_desc,
			       &dac_demo_iio_descriptor,NULL, &dac_buff, NULL)
	};

	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADC_DEMO_TIMER_TRIG_NAME, adc_timer_trig_desc,
				&adc_iio_timer_trig_desc),
		IIO_APP_TRIGGER(DAC_DEMO_TIMER_TRIG_NAME, dac_timer_trig_desc,
				&dac_iio_timer_trig_desc)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_demo_uart_ip;
	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = NULL;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	// update the reference to iio_desc
	adc_timer_trig_desc->iio_desc = app->iio_desc;
	dac_timer_trig_desc->iio_desc = app->iio_desc;

	return iio_app_run(app);
}
