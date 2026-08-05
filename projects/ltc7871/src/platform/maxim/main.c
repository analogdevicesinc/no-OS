/*****************************************************************************
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ltc7871 project.
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

int main()
{
	int ret = -EINVAL;
	ret = example_main();
	return ret;
}
