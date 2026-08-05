/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of DC2903A project.
 *   @author JSanBuenaventura (jose.sanbuenaventura@analog.com)
 *   @author MSosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	return example_main();
}
