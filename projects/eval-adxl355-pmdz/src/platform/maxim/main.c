/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxl355-pmdz project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "no_os_error.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef IIO_TRIGGER_EXAMPLE
	struct no_os_gpio_desc *adxl355_gpio_desc;
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize DATA READY pin */
	ret = no_os_gpio_get_optional(&adxl355_gpio_desc, &adxl355_gpio_drdy_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(adxl355_gpio_desc);
	if (ret)
		return ret;

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		return ret;

	ret = iio_trigger_example_main();
#endif

#ifdef DUMMY_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adxl355_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = dummy_example_main();
#endif

#if (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}

