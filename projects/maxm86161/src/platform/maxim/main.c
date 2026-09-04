/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main Function for the MAXM86161 driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "parameters.h"

extern int example_main(void);

int main(void)
{
	int ret;
	struct no_os_uart_desc *uart_desc;
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

	ret = no_os_uart_init(&uart_desc, &maxm86161_uart_ip);
	if (ret)
		goto remove_irq;

	no_os_uart_stdio(uart_desc);

	ret = example_main();

remove_uart:
	no_os_uart_remove(uart_desc);
remove_irq:
	no_os_irq_ctrl_remove(nvic_desc);
error:
	return ret;
}
