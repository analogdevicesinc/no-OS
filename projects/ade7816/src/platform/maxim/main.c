/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ade7816 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "basic_example.h"

int main()
{
	int ret = -EINVAL;

	struct no_os_uart_desc *uart_desc;

	/* NVIC Interrupt Controller specific for Maxim platform. */
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_desc_param = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_uart_init(&uart_desc, &ade7816_uart_ip);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_desc_param);
	if (ret)
		goto ade7816_uart_remove;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto ade7816_nvic_remove;

	no_os_uart_stdio(uart_desc);

	ret = basic_example_main();

ade7816_nvic_remove:
	no_os_irq_ctrl_remove(nvic_desc);
ade7816_uart_remove:
	no_os_uart_remove(uart_desc);

	return ret;
}
