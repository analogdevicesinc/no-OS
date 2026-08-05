/*******************************************************************************
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-adxl313z project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/*******************************************************************************
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	xsip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	adxl313_user_init.comm_init.spi_init = sip;

	stm32_init();

	return example_main();
}
