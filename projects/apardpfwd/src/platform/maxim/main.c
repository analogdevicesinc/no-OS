/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for the apardpfwd project.
 *   @author Aron Kis (kis.aron@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	return example_main();
}
