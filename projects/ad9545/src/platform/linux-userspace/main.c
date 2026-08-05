/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Linux platform of ad9545 project.
 *   @author Jonathan Santos (jonathan.santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "common_data.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for Linux platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	return example_main();
}
