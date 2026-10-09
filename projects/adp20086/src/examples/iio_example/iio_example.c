/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example source file for the adp20086 project.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_adp20086.h"
#include "common_data.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "iio_app.h"

/**
 * @brief Report a fault signalled on INTB.
 *
 * Invoked from adp20086_process_interrupt() in thread context, so printing
 * and further bus access are both safe here.
 */
static void adp20086_fault_callback(struct adp20086_dev *dev,
				    const struct adp20086_device_status *dev_status,
				    const struct adp20086_channel_status *ch_status,
				    void *ctx)
{
	uint8_t faults;
	int ch;

	for (ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		faults = ch_status->overcurrent | ch_status->overvoltage |
			 ch_status->undervoltage | ch_status->thermal_shutdown |
			 ch_status->open_load;

		if (!adp20086_channel_asserted(faults, ch))
			continue;

		pr_info("INTB CH%d:%s%s%s%s%s\n", ch + 1,
			adp20086_channel_asserted(ch_status->overcurrent, ch) ? " OC" : "",
			adp20086_channel_asserted(ch_status->overvoltage, ch) ? " OV" : "",
			adp20086_channel_asserted(ch_status->undervoltage, ch) ? " UV" : "",
			adp20086_channel_asserted(ch_status->thermal_shutdown,
						  ch) ? " TSD" : "",
			adp20086_channel_asserted(ch_status->open_load, ch) ? " OPEN" : "");
	}

	if (dev_status->ovin || dev_status->uvin)
		pr_info("INTB input fault:%s%s\n",
			dev_status->ovin ? " OVIN" : "",
			dev_status->uvin ? " UVIN" : "");
}

/**
 * @brief Bottom half of the INTB interrupt, run once per IIO loop iteration.
 *
 * Always returns 0: iio_app_run() tears the application down on a non-zero
 * return, and a transient bus error while reading status is not fatal.
 */
static int adp20086_step_callback(void *arg)
{
	adp20086_process_interrupt(arg);

	return 0;
}

int example_main()
{
	int ret;

	struct adp20086_iio_desc *adp20086_iio_desc;
	struct adp20086_iio_desc_init_param adp20086_iio_ip = {
		.adp20086_init_param = &adp20086_ip,
	};
	struct no_os_irq_ctrl_desc *adp20086_gpio_irq_desc;

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = no_os_irq_ctrl_init(&adp20086_gpio_irq_desc,
				  &adp20086_gpio_irq_ip);
	if (ret)
		goto exit;

	adp20086_ip.irq_ctrl = adp20086_gpio_irq_desc;

	ret = adp20086_iio_init(&adp20086_iio_desc, &adp20086_iio_ip);
	if (ret)
		goto remove_irq;

	ret = adp20086_set_int_callback(adp20086_iio_desc->adp20086_dev,
					adp20086_fault_callback, NULL);
	if (ret)
		goto remove_iio_adp20086;

	ret = adp20086_enable_intb(adp20086_iio_desc->adp20086_dev);
	if (ret)
		goto remove_iio_adp20086;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp20086",
			.dev = adp20086_iio_desc,
			.dev_descriptor = adp20086_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adp20086_uart_ip;
	/* Service a pending INTB assertion once per loop iteration. */
	app_init_param.post_step_callback = adp20086_step_callback;
	app_init_param.arg = adp20086_iio_desc->adp20086_dev;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp20086;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_adp20086:
	adp20086_iio_remove(adp20086_iio_desc);
remove_irq:
	no_os_irq_ctrl_remove(adp20086_gpio_irq_desc);
exit:
	if (ret)
		pr_info("Error!\n");

	return ret;
}
