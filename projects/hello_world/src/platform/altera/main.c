/***************************************************************************//**
 *   @file   altera/main.c
 *   @brief  Nios V hello_world example.
 *   @author Aurel Miron (aurel.miron@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "parameters.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/**
 * @brief Main function.
 * @return int - always runs forever, never returns.
 */
int main(void)
{
	uint32_t count = 0;

	pr_info("hello_world: hello from Nios V\n");

	while (1) {
		count++;

		pr_info("Hello World #%" PRIu32 "\n", count);

		/*
		 * no_os_mdelay() is the no-OS millisecond delay function.
		 * It uses the platform timer underneath, so the exact
		 * implementation differs per target but the API is identical.
		 */
		no_os_mdelay(1000);
	}

	return 0;
}
