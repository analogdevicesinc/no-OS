/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of the ADIN1110 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#include "frame_rx_tx_example.h"

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adin1110_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	return frame_rx_tx_example_main();

#ifndef FRAME_RX_TX
#error No example was selected.
#endif
}
