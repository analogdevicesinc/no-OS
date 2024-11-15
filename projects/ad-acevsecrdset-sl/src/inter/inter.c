/***************************************************************************//**
 *   @file   inter.c
 *   @brief  Interrupt on port 2 implementation file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_print_log.h"
#include "common_data.h"
#include "inter.h"

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/
static volatile uint8_t gpio_flag, opto_out_1_flag = 0, opto_out_2_flag = 0;

extern struct no_os_irq_ctrl_desc *stout_nvic_desc;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/
/**
 * @brief callback function
 * @param context - context variable
 * @return none
 */
static void gpio_2_3_cb_fn(void *context)
{
	struct no_os_irq_ctrl_desc *irq_desc1 = context;
	int ret;

	gpio_flag++;
	ret = no_os_irq_disable(irq_desc1, INT_PIN);
	if (ret)
		pr_err("ERROR %d \n", ret);
}

/**
 * @brief callback function opt_1
 * @param context - context variable
 * @return none
 */
static void gpio_2_5_cb_fn(void *context)
{
	struct no_os_irq_ctrl_desc *irq_desc2 = context;
	int ret;

	opto_out_1_flag = 1;
	ret = no_os_irq_disable(irq_desc2, INT_PIN_OPTO1);
	if (ret)
		pr_err("ERROR %d \n", ret);
}

/**
 * @brief callback function opt_2
 * @param context - context variable
 * @return none
 */
static void gpio_2_7_cb_fn(void *context)
{
	struct no_os_irq_ctrl_desc *irq_desc3 = context;
	int ret;

	opto_out_2_flag = 1;
	ret = no_os_irq_disable(irq_desc3, INT_PIN_OPTO2);
	if (ret)
		pr_err("ERROR %d \n", ret);
}

/**
 * @brief Get GPIO flag value
 *
 * @return flag value
 */
int get_gpio_flag_state(void)
{
	return gpio_flag;
}

/**
 * @brief Reset GPIO low flag value
 *
 * @return none
 */
void reset_gpio_low_flag_state(void)
{
	gpio_flag = 0;
}

/**
 * @brief Get GPIO Opto_out1 flag value
 *
 * @return flag value
 */
int get_gpio_opto_out1_flag_state(void)
{
	return opto_out_1_flag;
}

/**
 * @brief Reset GPIO Opto_out1 flag value
 *
 * @return none
 */
void reset_gpio_opto_out1_flag_state(void)
{
	opto_out_1_flag = 0;
}

/**
 * @brief Get GPIO Opto_out2 flag value
 *
 * @return flag value
 */
int get_gpio_opto_out2_flag_state(void)
{
	return opto_out_2_flag;
}

/**
 * @brief Reset GPIO Opto_out2 low flag value
 *
 * @return none
 */
void reset_gpio_opto_out2_flag_state(void)
{
	opto_out_2_flag = 0;
}

/**
 * @brief Initialize inter phase
 *
 * @return 0 in case of success, error code otherwise
 */
int inter_init(struct stout *stout)
{
	struct no_os_irq_ctrl_desc *irq_desc1;
	struct no_os_irq_ctrl_desc *irq_desc2;
	struct no_os_irq_ctrl_desc *irq_desc3;
	struct no_os_gpio_desc *pin;
	struct no_os_gpio_desc *pin2;
	struct no_os_gpio_desc *pin3;
	int ret;

	/* Setup GPIO interrupts */
	struct no_os_callback_desc p2_cb = {
		/** Callback to be called when the event occurs. */
		.callback = gpio_2_3_cb_fn,
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
		.irq_ctrl_id = INT_PORT,
		.platform_ops = &max_gpio_irq_ops,
	};

	/* Setup GPIO interrupts */
	struct no_os_callback_desc p2_opt1_cb = {
		/** Callback to be called when the event occurs. */
		.callback = gpio_2_5_cb_fn,
		/** Parameter to be passed when the callback is called */
		.ctx = NULL,
		/** Event that triggers the calling of the callback. */
		.event = NO_OS_EVT_GPIO,
		/** Interrupt source peripheral specifier. */
		.peripheral = NO_OS_GPIO_IRQ,
		/** Not used in the case of a GPIO IRQ controller */
		.handle = NULL
	};

	/* Setup GPIO interrupts */
	struct no_os_callback_desc p2_opt2_cb = {
		/** Callback to be called when the event occurs. */
		.callback = gpio_2_7_cb_fn,
		/** Parameter to be passed when the callback is called */
		.ctx = NULL,
		/** Event that triggers the calling of the callback. */
		.event = NO_OS_EVT_GPIO,
		/** Interrupt source peripheral specifier. */
		.peripheral = NO_OS_GPIO_IRQ,
		/** Not used in the case of a GPIO IRQ controller */
		.handle = NULL
	};

	/* Port 2 pin */
	ret = no_os_gpio_get(&pin, &gpio_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(pin);
	if (ret)
		goto error1;

	ret = no_os_gpio_get(&pin2, &gpio_ip);
	if (ret)
		goto error1;

	ret = no_os_gpio_direction_input(pin2);
	if (ret)
		goto error2;

	ret = no_os_gpio_get(&pin3, &gpio_ip);
	if (ret)
		goto error2;

	ret = no_os_gpio_direction_input(pin3);
	if (ret)
		goto error3;

	ret = no_os_irq_ctrl_init(&irq_desc1, &irq_gpio_ip);
	if (ret)
		goto error3;

	ret = no_os_irq_ctrl_init(&irq_desc2, &irq_gpio_ip);
	if (ret)
		goto error4;

	ret = no_os_irq_ctrl_init(&irq_desc3, &irq_gpio_ip);
	if (ret)
		goto error5;

	p2_cb.ctx = irq_desc1;
	p2_opt1_cb.ctx = irq_desc2;
	p2_opt2_cb.ctx = irq_desc3;

	/* Pe_error interrupt */
	ret = no_os_irq_register_callback(irq_desc1, INT_PIN,  &p2_cb);
	if (ret)
		goto error6;

	/* Pe_opto1 interrupt */
	ret = no_os_irq_register_callback(irq_desc2, INT_PIN_OPTO1,  &p2_opt1_cb);
	if (ret)
		goto error6;

	/* Pe_opto2 interrupt */
	ret = no_os_irq_register_callback(irq_desc3, INT_PIN_OPTO2,  &p2_opt2_cb);
	if (ret)
		goto error6;

	ret = no_os_irq_trigger_level_set(irq_desc2, INT_PIN_OPTO1,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error6;

	ret = no_os_irq_trigger_level_set(irq_desc3, INT_PIN_OPTO2,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error6;

	ret = no_os_irq_trigger_level_set(irq_desc1, INT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error6;

	ret = no_os_irq_set_priority(irq_desc1, INT_PIN, 1);
	if (ret)
		goto error6;

	ret = no_os_irq_set_priority(irq_desc2, INT_PIN_OPTO1, 1);
	if (ret)
		goto error6;

	ret = no_os_irq_set_priority(irq_desc3, INT_PIN_OPTO2, 1);
	if (ret)
		goto error6;

	ret = no_os_irq_enable(irq_desc1, INT_PIN);
	if (ret)
		goto error6;

	ret = no_os_irq_enable(irq_desc2, INT_PIN_OPTO1);
	if (ret)
		goto error6;

	ret = no_os_irq_enable(irq_desc3, INT_PIN_OPTO2);
	if (ret)
		goto error6;

	return 0;

error6:
	no_os_irq_ctrl_remove(irq_desc3);
error5:
	no_os_irq_ctrl_remove(irq_desc2);
error4:
	no_os_irq_ctrl_remove(irq_desc1);
error3:
	no_os_gpio_remove(pin3);
error2:
	no_os_gpio_remove(pin2);
error1:
	no_os_gpio_remove(pin);

error:
	pr_err("ERROR\n");
	return ret;
}
