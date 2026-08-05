/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for pico platform of eval-adis1646x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/**
 * @brief Main function execution for pico platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;
	adis1646x_ip.spi_init = &adis1646x_spi_ip;

	return example_main();
}
