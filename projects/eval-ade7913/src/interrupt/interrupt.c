/***************************************************************************//**
 *   @file   inter.c
 *   @brief  Data ready interrupt implementation file.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
#include "no_os_print_log.h"
#include "common_data.h"
#include "platform.h"
#include "interrupt.h"

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/
static volatile uint8_t drdy_flag;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

/**
 * @brief callback function
 * @param context - context variable
 * @return none
 */
static void interrupt_cb_fn(void *context)
{
	struct no_os_irq_ctrl_desc *irq_desc1 = context;
	int ret;
	int8_t *reg_val;
	struct rms_adc_values rms_values;

	ret = no_os_irq_disable(irq_desc1, GPIO_RDY_PIN);
	drdy_flag++;
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
 * @return none
 */
void reset_drdy_low_flag_state(void)
{
	drdy_flag = 0;
}

/**
 * @brief Initialize data ready interrupt
 * @param dev - device structure
 * @return 0 in case of success, error code otherwise
 */
int inter_init(struct ade7913_dev *dev)
{
	int ret;
	// drdy descriptor
	struct no_os_gpio_desc *drdy;
	struct no_os_irq_ctrl_desc *ade7913_gpio_irq_desc;

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
	ret = no_os_gpio_get(&drdy, &ade7913_gpio_rdy_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(drdy);
	if (ret)
		goto remove_gpio;

	ret = no_os_irq_ctrl_init(&ade7913_gpio_irq_desc, &irq_gpio_ip);
	if (ret)
		goto remove_gpio;

	p2_cb.ctx = ade7913_gpio_irq_desc;

	/* Pe_error interrupt */
	ret = no_os_irq_register_callback(ade7913_gpio_irq_desc, GPIO_RDY_PIN,  &p2_cb);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_trigger_level_set(ade7913_gpio_irq_desc, GPIO_RDY_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_set_priority(ade7913_gpio_irq_desc, GPIO_RDY_PIN, 1);
	if (ret)
		goto remove_irq;


	ret = no_os_irq_enable(ade7913_gpio_irq_desc, GPIO_RDY_PIN);
	if (ret)
		goto remove_irq;

	dev->irq_ctrl = ade7913_gpio_irq_desc;

	return 0;

remove_irq:
	no_os_irq_ctrl_remove(ade7913_gpio_irq_desc);
remove_gpio:
	no_os_gpio_remove(drdy);
error:
	pr_err("ERROR\n");
	return ret;
}
