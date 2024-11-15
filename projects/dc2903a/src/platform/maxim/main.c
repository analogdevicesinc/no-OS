/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of DC2903A project.
 *   @author JSanBuenaventura (jose.sanbuenaventura@analog.com)
 *   @author MSosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"
#include "basic_example.h"

/***************************************************************************//**
 * @brief Main function execution for maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	ltc2672_ip.spi_init = ltc2672_spi_ip;

	struct no_os_uart_desc *ltc2672_uart_desc;

	ret = no_os_uart_init(&ltc2672_uart_desc, &ltc2672_uart_ip);
	if (ret) {
		goto error;
	}

	no_os_uart_stdio(ltc2672_uart_desc);

	return basic_example_main();

	return ret;

#if (BASIC_EXAMPLE == 0)
#error Enable BASIC_EXAMPLE by using y value in Makefile.
#endif

error:
	no_os_uart_remove(ltc2672_uart_desc);
	return ret;
}
