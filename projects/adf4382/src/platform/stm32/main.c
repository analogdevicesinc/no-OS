/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of ADF4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
	adf4382_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;

	stm32_init();

	return example_main();
}
