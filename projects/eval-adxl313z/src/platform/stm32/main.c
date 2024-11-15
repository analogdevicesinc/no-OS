/*******************************************************************************
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-adxl313z project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <errno.h>
#include "platform_includes.h"
#include "common_data.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/*******************************************************************************
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -ENODEV;

	xsip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	adxl313_user_init.comm_init.spi_init = sip;

	stm32_init();

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret)
		goto error;
#endif

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);
	ret = basic_example_main();
	if (ret)
		goto error;
#endif

#if (IIO_EXAMPLE + BASIC_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
	return ret;
}
