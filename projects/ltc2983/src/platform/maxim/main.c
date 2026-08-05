/********************************************************************************
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ltc2983 project.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/*******************************************************************************
 * @brief Main function execution for LTC2983 platform.
 *
 * @return ret - Result of the enabled examples execution.
 *******************************************************************************/
int main()
{
	return example_main();
}
