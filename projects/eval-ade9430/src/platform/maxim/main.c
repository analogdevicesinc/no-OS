/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for the Maxim platform of EVAL-ADE9430 project.
 *   @author RaduE (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_error.h"

extern int example_main(void);

/**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled example execution.
 */
int main(void)
{
	return example_main();
}
