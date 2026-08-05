/*******************************************************************************
 *   @file   main.c
 *   @brief  Main file for linux platform of max31827 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#include "common_data.h"

extern int example_main();

/******************************************************************************
 * @brief Main function execution for FTD2XX platform.
 *
 * @return ret - Result of the enabled examples execution.
 *****************************************************************************/
int main()
{
	return example_main();
}
