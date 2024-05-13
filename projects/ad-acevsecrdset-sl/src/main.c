/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for the stout project. This includes general
 * 	     initialization routines for the uC.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2022, 2023 Analog Devices, Inc.
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

#include "state_machine.h"
#include "common_data.h"
#include "no_os_error.h"
#include "parameters.h"
#include "no_os_init.h"
#include "no_os_i2c.h"

#ifdef BT_ENABLED
#include "cordio_uart.h"
#include "cordio_init.h"
#endif

struct no_os_irq_ctrl_desc *stout_nvic_desc;

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

#ifdef BT_ENABLED
	cordio_init();
	while (1) {
		/* Run the WSF OS */
		wsfOsDispatcher();

		if (!WsfOsActive()) {
			/* No WSF tasks are active, optionally sleep */
		}
	}
#endif

	ret = no_os_irq_ctrl_init(&stout_nvic_desc, &ade9113_nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(stout_nvic_desc, NVIC_GPIO_IRQ, 1);
	if (ret)
		return ret;

#if defined(REV_D)
	ret = no_os_irq_set_priority(stout_nvic_desc, GPIO2_IRQn, 1);
	if (ret)
		return ret;
#endif

	ret = no_os_irq_enable(stout_nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		return ret;

#if defined(REV_D)
	ret = no_os_irq_enable(stout_nvic_desc, GPIO2_IRQn);
	if (ret)
		return ret;
#endif

	/* Initialize UART */
	struct no_os_uart_desc *uart_desc;

#ifdef BT_ENABLED
	pal_get_terminal_uart(&uart_desc);
#else
	ret = no_os_uart_init(&uart_desc, &ade9113_uart_ip);
	if (ret)
		return ret;
#endif

	no_os_uart_stdio(uart_desc);

	// Launch basic example
	ret = state_machine();
	if (ret) {
		no_os_uart_remove(uart_desc);
	}

	return ret;
}
