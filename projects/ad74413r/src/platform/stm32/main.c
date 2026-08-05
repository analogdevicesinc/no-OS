/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of ad74413r project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"
#include <stdio.h>
#include <stdlib.h>

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	ad74413r_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	ad74413r_ip.comm_param = ad74413r_spi_ip;

	stm32_init();

	return example_main();
}
