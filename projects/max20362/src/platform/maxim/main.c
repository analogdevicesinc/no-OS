/*******************************************************************************
 *   @file   main.c
 *   @brief  Main file for Maxim platform of MAX20362 project.
 *   @author Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main(void);

int main()
{
	return example_main();
}
