/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_uart.h"
#include "max14914.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

int example_main()
{
	struct no_os_uart_desc *uart_desc;
	int _ret = no_os_uart_init(&uart_desc, &max14914_uart_ip);
	if (_ret)
		return _ret;
	no_os_uart_stdio(uart_desc);

	struct max14914_desc *desc;
	int ret;

	ret = max14914_init(&desc, &max14914_ip);
	if (ret)
		goto exit;

	ret = max14914_set_state(desc, MAX14914_DIGITAL_INPUT_IEC_TYPE2);
	if (ret)
		goto free_max14914;

	pr_info("MAX14914 is successfully set as an IEC Type 2 Digital Input.\n");

free_max14914:
	max14914_remove(desc);
exit:
	if (ret)
		pr_info("Error!\n");

	return ret;
}
