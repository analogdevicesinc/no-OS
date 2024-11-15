/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adis1655x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

/**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;

	adis1655x_ip.spi_init = &adis1655x_spi_ip;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adis1655x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = basic_example_main();
	no_os_uart_remove(uart_desc);
#endif

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

	ret = iio_trigger_example_main();

remove_irq_ctrl:
	no_os_irq_ctrl_remove(nvic_desc);
remove_gpio:
	no_os_gpio_remove(adis_gpio_desc);
#endif

#if (BASIC_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}

