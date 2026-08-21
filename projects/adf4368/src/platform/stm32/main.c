/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of ADF4368 project.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/**
 * @brief Main function execution for STM32 platform.
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	adf4368_spi_extra.get_input_clock = HAL_RCC_GetPCLK1Freq;

	stm32_init();

	return example_main();
}
