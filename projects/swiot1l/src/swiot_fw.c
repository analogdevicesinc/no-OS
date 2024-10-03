/***************************************************************************//**
 *   @file   swiot_fw.c
 *   @brief  Main project file for the swiot1l project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "swiot_fw.h"
#include "iio_ad74413r.h"
#include "iio_max14906.h"
#include "iio_app.h"
#include "iio_adt75.h"
#include "swiot.h"
#include "common_data.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

#include "lwip_adin1110.h"

#define DATA_BUFFER_SIZE 1000
#define IIO_IGNORE_BUFF_OVERRUN_ERR

uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(uint32_t) * 8];

int step_callback(void *arg)
{
	struct swiot_iio_desc *swiot = arg;

	if (swiot->mode_change) {
		swiot->mode_change = false;

		return -ENOTCONN;
	}

	return 0;
}

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int swiot_firmware()
{
	uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x60};
	struct ad74413r_iio_desc_init_param ad74413r_iio_ip;
	struct no_os_irq_ctrl_desc *ad74413r_irq_desc;
	struct ad74413r_iio_desc *ad74413r_iio_desc;
	struct no_os_irq_ctrl_desc *ad74413r_nvic;
	struct iio_hw_trig *ad74413r_trig_desc;
	struct max14906_iio_desc *max14906_iio_desc;
	struct adt75_iio_desc *adt75_iio_desc;
	struct swiot_iio_desc *swiot_iio_desc;
	struct iio_sw_trig *sw_trig;
	int ret;

	struct no_os_gpio_desc *ad74413r_ldac_gpio;
	struct no_os_gpio_desc *ad74413r_irq_gpio;
	struct no_os_gpio_desc *ad74413r_reset_gpio;
	struct no_os_gpio_desc *max14906_en_gpio;
	struct no_os_gpio_desc *max14906_d1_gpio;
	struct no_os_gpio_desc *max14906_d2_gpio;
	struct no_os_gpio_desc *max14906_d3_gpio;
	struct no_os_gpio_desc *max14906_d4_gpio;
	struct no_os_gpio_desc *max14906_synch_gpio;
	struct no_os_gpio_desc *adin1110_swpd_gpio;
	struct no_os_gpio_desc *adin1110_tx2p4_gpio;
	struct no_os_gpio_desc *adin1110_mssel_gpio;
	struct no_os_gpio_desc *adin1110_cfg0_gpio;
	struct no_os_gpio_desc *adin1110_cfg1_gpio;
	struct no_os_gpio_desc *adin1110_int_gpio;
	struct no_os_gpio_desc *swiot_led1_gpio;
	struct no_os_gpio_desc *swiot_led2_gpio;

	struct iio_app_desc *app;
	struct iio_app_device iio_devices[4] = {0};
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * sizeof(uint32_t) * 8,
	};

	struct adt75_iio_init_param adt75_iio_ip;
	struct iio_app_init_param app_init_param = { 0 };

	struct max14906_iio_desc_init_param max14906_iio_ip;

	/* Configuring various GPIOs on the board */
	no_os_gpio_get(&swiot_led1_gpio, &swiot_led1_ip);
	no_os_gpio_get(&swiot_led2_gpio, &swiot_led2_ip);
	no_os_gpio_get(&max14906_d1_gpio, &max14906_d1_ip);
	no_os_gpio_get(&max14906_d2_gpio, &max14906_d2_ip);
	no_os_gpio_get(&max14906_d3_gpio, &max14906_d3_ip);
	no_os_gpio_get(&max14906_d4_gpio, &max14906_d4_ip);
	no_os_gpio_direction_output(max14906_d1_gpio, 0);
	no_os_gpio_direction_output(max14906_d2_gpio, 0);
	no_os_gpio_direction_output(max14906_d3_gpio, 0);
	no_os_gpio_direction_output(max14906_d4_gpio, 0);
	no_os_gpio_get(&max14906_en_gpio, &max14906_en_ip);
	no_os_gpio_direction_output(max14906_en_gpio, 0);
	no_os_gpio_get(&adin1110_cfg0_gpio, &adin1110_cfg0_ip);
	no_os_gpio_get(&ad74413r_ldac_gpio, &ad74413r_ldac_ip);
	no_os_gpio_get(&ad74413r_irq_gpio, &ad74413r_irq_ip);
	no_os_gpio_get(&ad74413r_reset_gpio, &ad74413r_reset_ip);
	no_os_gpio_get(&max14906_synch_gpio, &max14906_synch_ip);
	no_os_gpio_get(&adin1110_swpd_gpio, &adin1110_swpd_ip);
	no_os_gpio_get(&adin1110_tx2p4_gpio, &adin1110_tx2p4_ip);
	no_os_gpio_get(&adin1110_mssel_gpio, &adin1110_mssel_ip);
	no_os_gpio_get(&adin1110_cfg1_gpio, &adin1110_cfg1_ip);
	no_os_gpio_get(&adin1110_int_gpio, &adin1110_int_ip);
	no_os_gpio_direction_output(ad74413r_ldac_gpio, 0);
	no_os_gpio_direction_output(ad74413r_reset_gpio, 1);
	no_os_gpio_direction_output(max14906_synch_gpio, 1);
	no_os_gpio_direction_output(adin1110_swpd_gpio, 1);
	no_os_gpio_direction_output(adin1110_tx2p4_gpio, 0);
	no_os_gpio_direction_output(adin1110_mssel_gpio, 1);
	no_os_gpio_direction_output(adin1110_cfg1_gpio, 1);
	no_os_gpio_direction_output(adin1110_cfg0_gpio, 1);
	no_os_gpio_direction_output(swiot_led1_gpio, 1);
	no_os_gpio_direction_output(swiot_led2_gpio, 1);
	no_os_gpio_direction_input(adin1110_int_gpio);
	no_os_gpio_direction_input(ad74413r_irq_gpio);

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	memcpy(app_init_param.lwip_param.hwaddr, adin1110_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	ret = no_os_irq_ctrl_init(&ad74413r_nvic, &ad74413r_nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_enable(ad74413r_nvic, GPIO1_IRQn);
	if (ret)
		return ret;

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&ad74413r_irq_desc, &ad74413r_gpio_irq_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(ad74413r_irq_desc,
				     ad74413r_gpio_irq_ip.irq_ctrl_id, 0);
	if (ret)
		return ret;

	ad74413r_gpio_trig_ip.irq_ctrl = ad74413r_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&ad74413r_trig_desc, &ad74413r_gpio_trig_ip);
	if (ret)
		return ret;

	struct iio_trigger_init trigs[] = {
		/* Software trigger used as a heartbeat by the IIO client */
		IIO_APP_TRIGGER("sw_trig", sw_trig,
				&ad74413r_iio_trig_desc),
		IIO_APP_TRIGGER(AD74413R_GPIO_TRIG_NAME, ad74413r_trig_desc,
				&ad74413r_iio_trig_desc),
	};

	while (1) {
		adt75_iio_ip.adt75_init_param = &adt75_ip;
		max14906_iio_ip.max14906_init_param = &max14906_ip;
		ad74413r_iio_ip.ad74413r_init_param = &ad74413r_ip;

		ret = swiot_iio_init(&swiot_iio_desc, &swiot_ip);
		if (ret) {
			/*
			 * We either ran out of memory or provided an invalid init parameter.
			 * These errors are not recoverable, so we'll just exit.
			 */
			pr_err("swiot IIO device init error: %d (%s)\n", ret, strerror(-ret));
			return ret;
		}

		iio_devices[0].name = "swiot";
		iio_devices[0].dev = swiot_iio_desc;
		iio_devices[0].dev_descriptor = swiot_iio_desc->iio_dev;

		app_init_param.devices = iio_devices;
		app_init_param.nb_devices = 1;
		app_init_param.trigs = trigs;
		app_init_param.nb_trigs = 1;
		app_init_param.uart_init_params = uart_ip;
		app_init_param.post_step_callback = step_callback;
		app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
		app_init_param.lwip_param.mac_param = &adin1110_ip;
		app_init_param.lwip_param.extra = NULL;

		ret = iio_app_init(&app, app_init_param);
		if (ret) {
			pr_err("Config mode IIO app init error: %d (%s)\n", ret, strerror(-ret));
			goto free_swiot;
		}

		swiot_iio_desc->adin1110 = app->lwip_desc->mac_desc;
		app->arg = swiot_iio_desc;

		ret = iio_app_run(app);
		iio_app_remove(app);
		if (ret != -ENOTCONN) {
			pr_err("Config mode IIO app runtime error: %d (%s)\n", ret, strerror(-ret));
			goto free_swiot;
		}

		memcpy(&max14906_iio_ip.channel_configs, &swiot_iio_desc->max14906_configs,
		       sizeof(max14906_iio_ip.channel_configs));
		memcpy(&ad74413r_iio_ip.diag_channel_configs,
		       &swiot_iio_desc->ad74413r_diag_configs,
		       sizeof(ad74413r_iio_ip.diag_channel_configs));
		memcpy(&ad74413r_iio_ip.channel_configs, &swiot_iio_desc->ad74413r_configs,
		       sizeof(ad74413r_iio_ip.channel_configs));

		ret = max14906_iio_init(&max14906_iio_desc, &max14906_iio_ip);
		if (ret) {
			pr_err("MAX14906 device init error: %d (%s)\n", ret, strerror(-ret));
			goto free_swiot;
		}

		ret = adt75_iio_init(&adt75_iio_desc, &adt75_iio_ip);
		if (ret) {
			pr_err("ADT75 device init error: %d (%s)\n", ret, strerror(-ret));
			goto free_max14906;
		}

		ad74413r_iio_ip.trigger = ad74413r_trig_desc;
		ret = ad74413r_iio_init(&ad74413r_iio_desc, &ad74413r_iio_ip);
		if (ret) {
			pr_err("AD74413R device init error: %d (%s)\n", ret, strerror(-ret));
			goto free_adt75;
		}

		iio_devices[1].name = "max14906";
		iio_devices[1].dev = max14906_iio_desc;
		iio_devices[1].dev_descriptor = max14906_iio_desc->iio_dev;

		iio_devices[2].name = "adt75";
		iio_devices[2].dev = adt75_iio_desc;
		iio_devices[2].dev_descriptor = adt75_iio_desc->iio_dev;

		iio_devices[3].name = "ad74413r";
		iio_devices[3].dev = ad74413r_iio_desc;
		iio_devices[3].dev_descriptor = ad74413r_iio_desc->iio_dev;
		iio_devices[3].read_buff = &buff;

		for (int i = 0; i < AD74413R_N_CHANNELS; i++) {
			ret = ad74413r_set_adc_rate(ad74413r_iio_desc->ad74413r_desc,
						    i, AD74413R_ADC_SAMPLE_4800HZ);
			if (ret) {
				pr_err("SPI transfer error while setting the sample rate"
				       "for the ADC channels of the AD74413R: %d (%s)\n", ret, strerror(-ret));
				goto free_ad74413r;
			}
		}

		for (int i = 0; i < AD74413R_N_DIAG_CHANNELS; i++) {
			ret = ad74413r_set_adc_diag_rate(ad74413r_iio_desc->ad74413r_desc,
							 i, AD74413R_ADC_SAMPLE_4800HZ);
			if (ret) {
				pr_err("SPI transfer error while setting the sample rate"
				       "for the diagnostics channels of the AD74413R: %d (%s)\n", ret, strerror(-ret));
				goto free_ad74413r;
			}
		}

		swiot_iio_desc->ad74413r = ad74413r_iio_desc;
		swiot_iio_desc->max14906 = max14906_iio_desc;
		swiot_iio_desc->mode = SWIOT_RUNTIME;

		app_init_param.devices = iio_devices;
		app_init_param.nb_devices = 4;
		app_init_param.trigs = trigs;
		app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
		app_init_param.uart_init_params = uart_ip;
		app_init_param.post_step_callback = step_callback;

		ret = iio_app_init(&app, app_init_param);
		if (ret) {
			pr_err("Runtime mode IIO app init error: %d\n", ret);
			goto free_ad74413r;
		}

		sw_trig->iio_desc = app->iio_desc;
		ad74413r_trig_desc->iio_desc = app->iio_desc;
		swiot_iio_desc->adin1110 = app->lwip_desc->mac_desc;
		app->arg = swiot_iio_desc;

		no_os_gpio_set_value(max14906_en_gpio, 1);
		ret = iio_app_run(app);
		iio_app_remove(app);
		no_os_gpio_set_value(max14906_en_gpio, 0);
		if (ret != -ENOTCONN)
			pr_err("IIO app runtime error: %d\n", ret);

free_ad74413r:
		ad74413r_iio_remove(ad74413r_iio_desc);
free_max14906:
		max14906_iio_remove(max14906_iio_desc);
free_adt75:
		adt75_iio_remove(adt75_iio_desc);
free_swiot:
		swiot_iio_remove(swiot_iio_desc);
	}

	return 0;
}
