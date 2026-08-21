/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adgm3121 project.
 *   @author Ciobanu Radu (Radu-rares.Ciobanu@analog.com)
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
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	return example_main();
}
