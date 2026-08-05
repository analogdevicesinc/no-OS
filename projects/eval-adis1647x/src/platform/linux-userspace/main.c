/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Linux platform of eval-adis1647x project.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/**
 * @brief Main function execution for Linux platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main(void)
{
	adis1647x_ip.spi_init = &adis1647x_spi_ip;

	return example_main();
}
