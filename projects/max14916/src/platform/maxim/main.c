/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of max14916 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "parameters.h"
#include "no_os_error.h"

extern int example_main();

int main()
{
	return example_main();
}
