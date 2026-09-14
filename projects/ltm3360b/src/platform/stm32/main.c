/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of LTM3360B project.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

extern int example_main();
extern int stm32_init();

int main()
{
	stm32_init();
	return example_main();
}
