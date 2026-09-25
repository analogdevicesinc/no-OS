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
#include "max25603.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

int example_main()
{
	struct no_os_uart_desc *uart_desc;
	int _ret = no_os_uart_init(&uart_desc, &max25603_uart_ip);
	if (_ret)
		return _ret;
	no_os_uart_stdio(uart_desc);

	struct max25603_desc *max25603_desc;
	int ret;

	ret = max25603_init(&max25603_desc, &max25603_ip);
	if (ret)
		goto exit;

	ret = max25603_sel_comp(max25603_desc, MAX25603_COMP1);
	if (ret)
		goto remove_max25603;

	ret = max25603_set_enable(max25603_desc, MAX25603_EN2, 100000, 100);
	if (ret)
		goto remove_max25603;

	ret = max25603_set_beam(max25603_desc, MAX25603_LOW_BEAM, 50, 50);
	if (ret)
		goto remove_max25603;

	no_os_mdelay(5000);

	ret = max25603_set_beam(max25603_desc, MAX25603_DISABLE_BEAM, 0, 0);
	if (ret)
		goto remove_max25603;

	ret = max25603_set_enable(max25603_desc, MAX25603_DISABLE_EN, 0, 0);
	if (ret)
		goto remove_max25603;

remove_max25603:
	max25603_remove(max25603_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
