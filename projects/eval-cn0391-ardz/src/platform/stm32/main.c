/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main entry point for eval-cn0391-ardz firmware.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/**
 * @brief Application entry point.
 * @return 0 on success, negative error code otherwise.
 */
int main(void)
{
	stm32_init();

	return example_main();
}
