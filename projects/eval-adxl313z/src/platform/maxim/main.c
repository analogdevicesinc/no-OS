/*******************************************************************************
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxl313z project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/*******************************************************************************
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	adxl313_user_init.comm_init.spi_init = sip;

	return example_main();
}
