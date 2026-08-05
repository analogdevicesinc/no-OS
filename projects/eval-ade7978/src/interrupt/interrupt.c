/***************************************************************************//**
 *   @file   inter.c
 *   @brief  External interrupt implementation file.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_print_log.h"
#include "common_data.h"
#include "platform.h"
#include "interrupt.h"

static volatile uint8_t drdy_flag;
extern struct no_os_irq_ctrl_desc *ade7978_nvic_desc;

/**
 * @brief callback function
 * @param context - context variable
 */
static void interrupt_cb_fn(void *context)
{
	struct no_os_irq_ctrl_desc *irq_desc1 = context;
	int ret;

	drdy_flag++;
	ret = no_os_irq_disable(irq_desc1, GPIO_RDY_PIN);
	if (ret)
		pr_err("ERROR %d \n", ret);
}

/**
 * @brief Get data ready flag value
 *
 * @return flag value
 */
int get_drdy_flag_state(void)
{
	return drdy_flag;
}

/**
 * @brief Reset data ready low flag value
 *
 */
void reset_drdy_low_flag_state(void)
{
	drdy_flag = 0;
}


/**
 * @brief Initialize interrupt
 * @param dev - device structure
 * @return 0 in case of success, error code otherwise
 */
int inter_init(struct ade7978_dev *dev)
{
	int ret;
	struct no_os_gpio_desc *drdy_pin;
	struct no_os_irq_ctrl_desc *ade7978_gpio_irq_desc;

	if (!dev)
		return -ENODEV;

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

	/* Initialize GPIO IRQ controller in order to be able to enable GPIO IRQ interrupt */
	struct no_os_irq_init_param irq_gpio_ip = {
		.irq_ctrl_id = GPIO_RDY_PORT,
		.platform_ops = &max_gpio_irq_ops,
	};

	/* Port 2 drdy */
	ret = no_os_gpio_get(&drdy_pin, &ade7978_gpio_rdy_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(drdy_pin);
	if (ret)
		goto remove_gpio;

	ret = no_os_irq_ctrl_init(&ade7978_gpio_irq_desc, &irq_gpio_ip);
	if (ret)
		goto remove_gpio;

	p2_cb.ctx = ade7978_gpio_irq_desc;

	/* Dready interrupt */
	ret = no_os_irq_register_callback(ade7978_gpio_irq_desc, GPIO_RDY_PIN,
					  &p2_cb);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_trigger_level_set(ade7978_gpio_irq_desc, GPIO_RDY_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_set_priority(ade7978_gpio_irq_desc, GPIO_RDY_PIN, 1);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_enable(ade7978_gpio_irq_desc, GPIO_RDY_PIN);
	if (ret)
		goto remove_irq;

	dev->irq_ctrl = ade7978_gpio_irq_desc;

	return 0;

remove_irq:
	no_os_irq_ctrl_remove(ade7978_gpio_irq_desc);
remove_gpio:
	no_os_gpio_remove(drdy_pin);

error:
	pr_err("ERROR\n");
	return ret;
}
