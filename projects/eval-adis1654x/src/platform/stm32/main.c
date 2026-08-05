/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-adis1654x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	adis1654x_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	adis1654x_ip.spi_init = &adis1654x_spi_ip;

	stm32_init();

	return example_main();
}
