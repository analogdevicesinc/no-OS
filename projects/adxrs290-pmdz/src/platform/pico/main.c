/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for pico platform of adxrs290 project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"
#include "pico/stdlib.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for pico platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	adxrs290_ip.spi_init = adxrs290_spi_ip;

	return example_main();
}
