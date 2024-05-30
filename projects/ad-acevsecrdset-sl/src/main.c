/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for the stout project. This includes general
 * 	     initialization routines for the uC.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2022, 2023 Analog Devices, Inc.
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
#include "state_machine.h"
#include "common_data.h"
#include "no_os_error.h"
#include "parameters.h"
#include "no_os_init.h"
#include "no_os_i2c.h"

#define PIN_NO	(25u)
#define INTERRUPT_PORT (0u)

struct no_os_irq_ctrl_desc *stout_nvic_desc;

/**
 * @brief callback function
 * @param context - context variable
 * @return none
 */
static void cb_fn(void *context)
{
	printf("Test\n");
}

struct no_os_gpio_init_param gpio_ip = {
	.port = INTERRUPT_PORT,
	.number = PIN_NO,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param led_ip = {
	.port = 0,
	.number = 31,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};


/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;
	// ADE9113 dev SPI init params
	ade9113_ip.spi_init = &ade9113_spi_ip;
	// ADE9113 dev DATA_RDY init params
	ade9113_ip.gpio_rdy = &ade9113_gpio_rdy_ip;
	// ADE9113 dev RESET init params
	ade9113_ip.gpio_reset = &ade9113_gpio_reset_ip;

	ret = no_os_init();
	if (ret)
		return ret;

	/* Initialize NVIC IRQ controller in order to be able to enable GPIO IRQ interrupt */
	struct no_os_irq_init_param ade9113_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_irq_ctrl_init(&stout_nvic_desc, &ade9113_nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(stout_nvic_desc, GPIO2_IRQn, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(stout_nvic_desc, GPIO2_IRQn);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(stout_nvic_desc, GPIO0_IRQn, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(stout_nvic_desc, GPIO0_IRQn);
	if (ret)
		return ret;

	/* Initialize UART */
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ade9113_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	// // Launch basic example
	// ret = state_machine();
	// if (ret) {
	// 	no_os_uart_remove(uart_desc);
	// }

	printf("Testing\n");

	struct no_os_callback_desc p2_cb = {
		/** Callback to be called when the event occurs. */
		.callback = cb_fn,
		/** Parameter to be passed when the callback is called */
		.ctx = NULL,
		/** Event that triggers the calling of the callback. */
		.event = NO_OS_EVT_GPIO,
		/** Interrupt source peripheral specifier. */
		.peripheral = NO_OS_GPIO_IRQ,
		/** Not used in the case of a GPIO IRQ controller */
		.handle = NULL
	};

	struct no_os_gpio_desc *pin;
	struct no_os_gpio_desc *led;
	struct no_os_irq_ctrl_desc *irq_desc1;

	/* Port 2 pin */
	ret = no_os_gpio_get(&pin, &gpio_ip);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

		/* Port 2 pin */
	ret = no_os_gpio_get(&led, &led_ip);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	ret = no_os_gpio_direction_input(pin);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	ret = no_os_gpio_direction_output(led, NO_OS_GPIO_HIGH);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	no_os_mdelay(1000);
	no_os_gpio_set_value(led, 0);
	no_os_mdelay(1000);
	no_os_gpio_set_value(led, 1);

	/* Initialize GPIO IRQ controller in order to be able to enable GPIO IRQ interrupt */
	struct no_os_irq_init_param irq_gpio_ip = {
		.irq_ctrl_id = INTERRUPT_PORT,
		.platform_ops = &max_gpio_irq_ops,
	};

	ret = no_os_irq_ctrl_init(&irq_desc1, &irq_gpio_ip);
	if (ret)
		return ret;

	/* RCDAC interrupt */
	ret = no_os_irq_register_callback(irq_desc1, PIN_NO,  &p2_cb);
	if (ret)
		return ret;

	ret = no_os_irq_trigger_level_set(irq_desc1, PIN_NO,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(irq_desc1, PIN_NO, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(irq_desc1, PIN_NO);
	if (ret)
		return ret;

	uint8_t value = 0;

	while(1) {
		no_os_gpio_get_value(pin, &value);
		if (value)
			no_os_gpio_set_value(led, 0);
		else
			no_os_gpio_set_value(led, 1);
	}

	return ret;
}

