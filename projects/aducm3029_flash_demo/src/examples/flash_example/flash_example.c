/***************************************************************************//**
 *   @file   flash_example.c
 *   @brief  Implementation of flash example for aducm3029_flash_demo project.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include "common_data.h"
#include "no_os_flash.h"
#include "no_os_irq.h"
#include "no_os_uart.h"

/***************************************************************************//**
 * @brief Flash example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int example_main()
{
	int32_t ret;
	uint32_t flash_val = 0;
	struct no_os_flash_dev *flash_dut;
	struct no_os_uart_desc *uart_dut;
	struct no_os_irq_ctrl_desc *irq_dut;

	ret = no_os_irq_ctrl_init(&irq_dut, &flash_demo_irq_ip);
	if (ret < 0)
		return ret;

	ret = no_os_irq_global_enable(irq_dut);
	if (ret < 0)
		return ret;

	ret = no_os_uart_init(&uart_dut, &flash_demo_uart_ip);
	if (ret < 0)
		return ret;
	no_os_uart_stdio(uart_dut);

	ret = no_os_flash_init(&flash_dut, &flash_demo_flash_ip);
	if (ret < 0)
		return ret;

	printf("UART online.\n");

	no_os_flash_read(flash_dut, 0x3E000, &flash_val, 4);
	printf("Address 0x3E000: %lX\n", (unsigned long)flash_val);
	no_os_flash_read(flash_dut, 0x3E004, &flash_val, 4);
	printf("Address 0x3E004: %lX\n", (unsigned long)flash_val);

	flash_val = 0xAABBCCDD;

	printf("Write 0xAABBCCDD to 0x3E000\n");
	ret = no_os_flash_write(flash_dut, 0x3E000, &flash_val, 1);
	if (ret < 0)
		return ret;

	no_os_flash_read(flash_dut, 0x3E000, &flash_val, 1);
	printf("Address 0x3E000: %lX\n", (unsigned long)flash_val);
	no_os_flash_read(flash_dut, 0x3E004, &flash_val, 1);
	printf("Address 0x3E004: %lX\n", (unsigned long)flash_val);

	flash_val = 0xCCDDAABB;

	printf("Write 0xCCDDAABB to 0x3E004\n");
	ret = no_os_flash_write(flash_dut, 0x3E004, &flash_val, 1);
	if (ret < 0)
		return ret;

	no_os_flash_read(flash_dut, 0x3E000, &flash_val, 1);
	printf("Address 0x3E000: %lX\n", (unsigned long)flash_val);
	no_os_flash_read(flash_dut, 0x3E004, &flash_val, 1);
	printf("Address 0x3E004: %lX\n", (unsigned long)flash_val);

	ret = no_os_irq_global_disable(irq_dut);
	if (ret < 0)
		return ret;
	ret = no_os_irq_ctrl_remove(irq_dut);
	if (ret < 0)
		return ret;
	ret = no_os_uart_remove(uart_dut);
	if (ret < 0)
		return ret;
	ret = no_os_flash_remove(flash_dut);
	if (ret < 0)
		return ret;

	return 0;
}
