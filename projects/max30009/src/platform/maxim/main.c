/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function for Maxim platform
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "parameters.h"

int example_main();

int main(void)
{
	int ret;

	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = IRQ_OPS,
	};

	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		goto error;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto remove_irq;

	struct no_os_uart_desc *uart_desc;
	ret = no_os_uart_init(&uart_desc, &max30009_uart_ip);
	if (ret)
		goto remove_irq;

	no_os_uart_stdio(uart_desc);

	/* Add delay to ensure UART is fully initialized after power-on reset */
	no_os_mdelay(100);

	ret = example_main();
	if (ret)
		goto remove_uart;

	return 0;

remove_uart:
	no_os_uart_remove(uart_desc);
remove_irq:
	no_os_irq_ctrl_remove(nvic_desc);
error:
	return ret;
}
