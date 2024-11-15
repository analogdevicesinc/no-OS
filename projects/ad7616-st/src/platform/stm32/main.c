/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-ad7616 project.
 *   @author Esteban Blanc (eblanc@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
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

struct no_os_uart_desc *uart_desc;

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	ad7616_ip.spi_param = &ad7616_spi_ip;
	ad7616_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK2Freq;

	ad7616_ip.gpio_convst_param->extra = &ad7616_gpio_convst_extra_ip;

	stm32_init();

#ifdef IIO_EXAMPLE
	return iio_example_main();
#else
#error At least one example has to be selected using y value in Makefile.
#endif

	return 0;
}
