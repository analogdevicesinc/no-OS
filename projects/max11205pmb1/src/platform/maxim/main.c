/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of max11205pmb1 project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = 0;

	max11205_ip.spi_init = max11205_spi_ip;

	struct no_os_irq_ctrl_desc *max11205_nvic_desc;
	struct no_os_irq_init_param max11205_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize NVIC IRQ controller in orde to be able to enable GPIO IRQ interrupt */
	ret = no_os_irq_ctrl_init(&max11205_nvic_desc, &max11205_nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(max11205_nvic_desc, NVIC_GPIO_IRQ, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(max11205_nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		return ret;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *max11205_uart_desc;

	ret = no_os_uart_init(&max11205_uart_desc, &max11205_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(max11205_uart_desc);

	ret = basic_example_main();
#endif

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif
	return ret;

#if (BASIC_EXAMPLE + IIO_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif
}
