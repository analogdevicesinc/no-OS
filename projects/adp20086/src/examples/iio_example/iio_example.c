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
 * @brief Fault handler invoked on an INTB assertion.
 *
 * Called from adp20086_process_interrupt() in thread context, with the status
 * read just after the interrupt fired. Registering one is optional - the IIO
 * status attributes read the latched STAT registers directly, so faults are
 * visible to the host without any callback at all. It is wired up here to show
 * where an application would hook its own fault response: shedding load,
 * latching a failure for a supervisor, driving an indicator.
 *
 * Deliberately empty rather than logging: the UART carries the libiio protocol
 * in this example, so a pr_info() here would corrupt the client's connection.
 * Use a spare GPIO or a second UART if the handler needs to say something.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param status - Device status read after the interrupt fired.
 * @param ctx - The context passed to adp20086_set_interrupt_callback().
 */
static void adp20086_fault_callback(struct adp20086_dev *dev,
				    const struct adp20086_status *status,
				    void *ctx)
{
}

/**
 * @brief Bottom half of the INTB interrupt, run once per IIO loop iteration.
 *
 * Reads the latched status, hands it to the registered fault callback and
 * re-arms INTB.
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

	/* Install the fault handler before arming, so that an assertion cannot
	 * be delivered before there is something to receive it. */
	ret = adp20086_set_interrupt_callback(adp20086_iio_desc->adp20086_dev,
					      adp20086_fault_callback, NULL);
	if (ret)
		goto remove_iio_adp20086;

	ret = adp20086_arm_interrupt(adp20086_iio_desc->adp20086_dev);
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
