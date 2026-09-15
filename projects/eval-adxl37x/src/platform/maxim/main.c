/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxl37x project.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_error.h"
#include "no_os_uart.h"
#include "maxim_uart_stdio.h"

extern int example_main();

int main()
{

	return example_main();
}
