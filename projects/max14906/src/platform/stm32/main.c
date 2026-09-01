/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "no_os_error.h"
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
	max14906_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	stm32_init();

	return example_main();
}
