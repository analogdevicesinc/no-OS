/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ad9545 project.
 *   @author Jonathan Santos (jonathan.santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "common_data.h"
#include "platform_includes.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#endif

#if (BASIC_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
