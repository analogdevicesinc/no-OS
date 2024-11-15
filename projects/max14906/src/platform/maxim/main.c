/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

int main()
{
	int ret = -EINVAL;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	/* NVIC Interrupt Controller specific for Maxim platform. */
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_desc_param = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_uart_init(&uart_desc, &max14906_uart_ip);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_desc_param);
	if (ret)
		goto max14906_uart_remove;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto max14906_irq_ctrl_remove;

	no_os_uart_stdio(uart_desc);

	ret = basic_example_main();

max14906_irq_ctrl_remove:
	no_os_irq_ctrl_remove(nvic_desc);
max14906_uart_remove:
	no_os_uart_remove(uart_desc);

#endif

#if (BASIC_EXAMPLE + IIO_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
