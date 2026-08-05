/***************************************************************************//**
 *   @file   interrupt.c
 *   @brief  Interrupt configuration for eval-ublox-gnss project.
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "interrupt.h"
#include "platform.h"
#include "no_os_irq.h"
#include "no_os_error.h"

/* Global flag for interrupt */
static volatile uint8_t irq_flag;

/**
 * @brief callback function
 * @param context - context variable
 */
static void interrupt_cb_fn(void *context)
{
	struct no_os_irq_ctrl_desc *irq_desc1 = context;
	int ret;

	irq_flag++;
	ret = no_os_irq_disable(irq_desc1, GPIO_IRQ_PIN);
	if (ret)
		pr_err("ERROR %d \n", ret);
}

/**
 * @brief Get irqn flag value
 *
 * @return flag value
 */
int get_irq_flag_state(void)
{
	return irq_flag;
}

/**
 * @brief Reset irqn flag value
 *
 */
void reset_irq_flag_state(void)
{
	irq_flag = 0;
}

/**
 * @brief Initialize interrupt handling for the GNSS device.
 *
 * @param gnss_dev - GNSS device structure.
 *
 * @return ret     - Result of the initialization - 0 in case of success,
 *                   negative error code otherwise.
 */
int interrupt_init(struct gnss_dev *gnss_dev)
{
	int ret;
	struct no_os_gpio_desc *irq_pin;
	struct no_os_irq_ctrl_desc *gnss_irq_desc;

	/* Setup GPIO interrupts */
	struct no_os_callback_desc p2_cb = {
		/** Callback to be called when the event occurs. */
		.callback = interrupt_cb_fn,
		/** Parameter to be passed when the callback is called */
		.ctx = NULL,
		/** Event that triggers the calling of the callback. */
		.event = NO_OS_EVT_GPIO,
		/** Interrupt source peripheral specifier. */
		.peripheral = NO_OS_GPIO_IRQ,
		/** Not used in the case of a GPIO IRQ controller */
		.handle = NULL
	};

	/* Initialize GPIO IRQ controller in order to be able
	to enable GPIO IRQ interrupt */
	struct no_os_irq_init_param irq_gpio_ip = {
		.irq_ctrl_id = GPIO_IRQ_PORT,
		.platform_ops = &max_gpio_irq_ops,
	};

	if (!gnss_dev)
		return -ENODEV;

	ret = no_os_gpio_get(&irq_pin, &gnss_gpio_irq_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(irq_pin);
	if (ret)
		goto remove_gpio;

	ret = no_os_irq_ctrl_init(&gnss_irq_desc, &irq_gpio_ip);
	if (ret)
		goto remove_gpio;

	p2_cb.ctx = gnss_irq_desc;

	ret = no_os_irq_register_callback(gnss_irq_desc, GPIO_IRQ_PIN,
					  &p2_cb);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_trigger_level_set(gnss_irq_desc, GPIO_IRQ_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto clean_callback;

	ret = no_os_irq_set_priority(gnss_irq_desc, GPIO_IRQ_PIN, 1);
	if (ret)
		goto clean_callback;

	gnss_dev->irq_ctrl = gnss_irq_desc;
	gnss_dev->irq_gpio = irq_pin;
	gnss_dev->irq_cb = p2_cb;

	return 0;

clean_callback:
	no_os_irq_unregister_callback(gnss_irq_desc, GPIO_IRQ_PIN, &p2_cb);
remove_irq:
	no_os_irq_ctrl_remove(gnss_irq_desc);
remove_gpio:
	no_os_gpio_remove(irq_pin);

error:
	pr_err("ERROR\n");
	return ret;
}

/**
 * @brief Remove interrupts for GNSS device
 * @param gnss_dev - GNSS device structure
 * @return 0 in case of success, negative error code otherwise
 */
int interrupt_remove(struct gnss_dev *gnss_dev)
{
	int ret;

	if (!gnss_dev)
		return -ENODEV;

	ret = no_os_irq_disable(gnss_dev->irq_ctrl, GPIO_IRQ_PIN);
	if (ret)
		return ret;

	ret = no_os_irq_unregister_callback(gnss_dev->irq_ctrl, GPIO_IRQ_PIN,
					    &gnss_dev->irq_cb);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_remove(gnss_dev->irq_ctrl);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(gnss_dev->irq_gpio);
	if (ret)
		return ret;

	return 0;
}
