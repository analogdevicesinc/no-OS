/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adis16607 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_error.h"

extern int example_main();
/**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;

	adis16607_ip.spi_init = &adis16607_spi_ip;

#ifdef IIO_TRIGGER_EXAMPLE
	struct no_os_gpio_desc *adis_gpio_desc;
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize DATA READY pin */
	ret = no_os_gpio_get_optional(&adis_gpio_desc, &adis_gpio_drdy_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(adis_gpio_desc);
	if (ret)
		goto remove_gpio;

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		goto remove_gpio;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto remove_irq_ctrl;

	ret = example_main();

remove_irq_ctrl:
	no_os_irq_ctrl_remove(nvic_desc);
remove_gpio:
	no_os_gpio_remove(adis_gpio_desc);
#else
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adis16607_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = example_main();
	no_os_uart_remove(uart_desc);
#endif

	return ret;
}

