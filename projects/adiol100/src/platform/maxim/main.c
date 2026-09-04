/***************************************************************************//**
 *   @file   main.c
 *   @brief  ADIOL100 project main entry point (Maxim platform).
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "maxim_uart_stdio.h"
#include "no_os_uart.h"
#include "no_os_irq.h"

#ifdef CONFIG_ADIOL100_BASIC_EXAMPLE
extern int basic_example_main(void);
#endif

#ifdef CONFIG_ADIOL100_ILINK_EXAMPLE
extern int ilink_example_main(void);
#endif

int main(void)
{
	int ret;
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &adiol100_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

#ifdef CONFIG_ADIOL100_BASIC_EXAMPLE
	ret = basic_example_main();
#endif

#ifdef CONFIG_ADIOL100_ILINK_EXAMPLE
	struct no_os_irq_ctrl_desc *nvic_ctrl;

	ret = no_os_irq_ctrl_init(&nvic_ctrl, &nvic_ip);
	if (ret)
		goto uart_remove;

	ret = no_os_irq_enable(nvic_ctrl, NVIC_GPIO_IRQ);
	if (ret)
		goto nvic_remove;

	ret = ilink_example_main();

nvic_remove:
	no_os_irq_ctrl_remove(nvic_ctrl);
#endif

uart_remove:
	no_os_uart_remove(uart);

	return ret;
}
