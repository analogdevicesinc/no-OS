/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Source file for iio example.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

#include "parameters.h"
#include "common.h"
#include "common_data.h"
#include "iio_admt4000.h"
#include "iio_admt_evb.h"
#include "iio_trigger.h"
#include "iio_app.h"

#ifdef TMC
#include "iio_tmc5240.h"
#endif

#define DATA_BUFFER_SIZE 400
uint8_t iio_data_buffer[DATA_BUFFER_SIZE * 5 * sizeof(uint32_t)];

#define ADMT4000_GPIO_TRIG_NAME "admt4000-dev0"

/* GPIO trigger */
struct no_os_irq_init_param admt4000_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

struct iio_hw_trig_init_param admt4000_gpio_trig_ip = {
	.irq_id = ADMT4000_GPIO_TRIG_IRQ_ID,
	.irq_trig_lvl = NO_OS_IRQ_EDGE_FALLING,
	.cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADMT4000_GPIO_CB_HANDLE,
	},
	.name = ADMT4000_GPIO_TRIG_NAME,
};

static struct iio_ctx_attr context_attributes[] = {
	{.name = "hw_mezzanine", .value = HW_MEZZANINE_NAME },
	{.name = "hw_carrier", .value = HW_CARRIER_NAME },
	{.name = "hw_name", .value = DEVICE_NAME },
	{.name = "hw_vendor", .value = DEVICE_VENDOR },
	{.name = "fw_version", .value = FW_VERSION },
};

int example_main()
{
	int ret;
	struct admt4000_iio_dev *admt4000_iio_desc;
	struct admt4000_iio_dev_init_param admt4000_iio_ip;
	struct admt_evb_iio_desc *admt_evb_desc;
	struct admt_evb_iio_init_param admt_evb_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_ctx_attr *iio_ctx_attrs;
	struct iio_hw_trig *admt4000_trig_desc;
	struct no_os_eeprom_desc *eeprom_desc;
	struct no_os_irq_ctrl_desc *admt4000_irq_desc;
	struct no_os_gpio_desc *gpio_v_en_desc;
	struct no_os_gpio_desc *gpio_shdn_n_desc;
	struct no_os_gpio_desc *gpio_rstb_desc;
	struct iio_data_buffer data_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE * 5 * sizeof(uint16_t)
	};

#ifdef TMC
	struct tmc5240_iio_dev *tmc_iio_desc;

	struct tmc5240_iio_dev_init_param tmc_iio_ip;
#endif
	uint32_t nb_ctx_attr;
	bool hw_mezzanine_is_valid = false;
	no_os_mdelay(2000);
	ret = no_os_eeprom_init(&eeprom_desc, &eeprom_ip);
	if (ret) {
		return ret;
	}
	no_os_mdelay(2000);
	ret = get_iio_context_attributes_ex(&iio_ctx_attrs,
					    &nb_ctx_attr,
					    eeprom_desc,
					    HW_MEZZANINE_NAME,
					    HW_CARRIER_NAME,
					    &hw_mezzanine_is_valid,
					    FIRMWARE_VERSION);

	if (ret) {
		return ret;
	}

	ret = no_os_gpio_get(&gpio_shdn_n_desc,
			     &gpio_shdn_n_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_output(gpio_shdn_n_desc,
					  NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	admt_evb_ip.gpio_shdn_n_ip = gpio_shdn_n_ip;
	admt_evb_ip.gpio_v_en_ip = gpio_v_en_ip;
	admt_evb_ip.gpio_rstb_ip = gpio_rstb_ip;
	admt_evb_ip.gpio_coil_rs_ip = gpio_coil_rs_ip;
	no_os_mdelay(3);

	ret = admt_evb_iio_init(&admt_evb_desc, &admt_evb_ip);
	if (ret)
		return ret;

	admt4000_iio_ip.admt4000_init_param = &admt4000_ip;

	ret = admt4000_iio_init(&admt4000_iio_desc, &admt4000_iio_ip);

	if (ret)
		return ret;

#ifdef TMC
	tmc_iio_ip.tmc5240_init_param = &tmc5240_ip;
	ret = tmc5240_iio_init(&tmc_iio_desc, &tmc_iio_ip);
	if (ret)
		return ret;
#endif

	/* Initialize interrupt controller */
	ret = no_os_irq_ctrl_init(&admt4000_irq_desc, &admt4000_gpio_irq_ip);

	if (ret)
		return ret;

	ret = no_os_irq_set_priority(admt4000_irq_desc, admt4000_gpio_trig_ip.irq_id,
				     7);
	if (ret)
		return ret;

	admt4000_gpio_trig_ip.irq_ctrl = admt4000_irq_desc;

	/* Initialize hardware trigger */
	ret = iio_hw_trig_init(&admt4000_trig_desc, &admt4000_gpio_trig_ip);
	if (ret)
		return ret;

	/* Declaring iio_devices structure */
	struct iio_app_device iio_devices[] = {
		{
			.name = "admt_evb",
			.dev = admt_evb_desc,
			.dev_descriptor = admt_evb_desc->iio_dev,
		},
		{
			.name = "admt4000",
			.dev = admt4000_iio_desc,
			.dev_descriptor = admt4000_iio_desc->iio_dev,
			.read_buff = &data_buff,
			.default_trigger_id = "trigger0",
		},
#ifdef TMC
		{
			.name = "tmc5240",
			.dev = tmc_iio_desc,
			.dev_descriptor = tmc_iio_desc->iio_dev,
		},
#endif
	};

	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(ADMT4000_GPIO_TRIG_NAME, admt4000_trig_desc,
				&admt4000_iio_trig_desc)
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = admt4000_uart_ip;

	app_init_param.trigs = trigs;
	app_init_param.nb_trigs = NO_OS_ARRAY_SIZE(trigs);
	app_init_param.irq_desc = admt4000_irq_desc;
	app_init_param.ctx_attrs = iio_ctx_attrs;
	app_init_param.nb_ctx_attr = nb_ctx_attr;

	ret = iio_app_init(&app, app_init_param);

	if (ret)
		return ret;

	admt4000_trig_desc->iio_desc = app->iio_desc;

	return iio_app_run(app);
}
