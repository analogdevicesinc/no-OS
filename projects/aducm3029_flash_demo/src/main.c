/***************************************************************************//**
 *   @file   aducm3029_flash_demo/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include <sys/platform.h>

#include "adi_initialize.h"
#include "no_os_flash.h"
#include "no_os_irq.h"
#include "platform_init.h"
#include "no_os_uart.h"
#include "aducm3029_uart.h"
#include "aducm3029_uart_stdio.h"

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(int argc, char *argv[])
{
	/**
	 * Initialize managed drivers and/or services that have been added to
	 * the project.
	 * @return zero on success
	 */
	adi_initComponents();

	int32_t ret;
	uint32_t flash_val = 0;
	struct no_os_flash_dev *flash_dut;
	struct no_os_flash_init_param flash_init_par = {
		.id = 0
	};
	struct no_os_uart_desc *uart_dut;
	struct no_os_uart_init_param uart_init_par = {
		.baud_rate = 115200,
		.device_id = 0,
		.parity = NO_OS_UART_PAR_NO,
		.size = NO_OS_UART_CS_8,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &aducm_uart_ops,
	};
	struct no_os_irq_ctrl_desc *irq_dut;
	struct no_os_irq_init_param irq_init = {
		.irq_ctrl_id = 0,
		.extra = NULL
	};

	ret = platform_init();
	if(ret < 0)
		return ret;

	ret = no_os_irq_ctrl_init(&irq_dut, &irq_init);
	if(ret < 0)
		return ret;

	ret = no_os_irq_global_enable(irq_dut);
	if (ret < 0)
		return ret;

	ret = no_os_uart_init(&uart_dut, &uart_init_par);
	if (ret < 0)
		return ret;
	no_os_uart_stdio(uart_dut);

	ret = no_os_flash_init(&flash_dut, &flash_init_par);
	if (ret < 0)
		return ret;

	printf("UART online.\n");

	no_os_flash_read(flash_dut, 0x3E000, &flash_val, 4);
	printf("Address 0x3E000: %lX\n", flash_val);
	no_os_flash_read(flash_dut, 0x3E004, &flash_val, 4);
	printf("Address 0x3E004: %lX\n", flash_val);

	flash_val = 0xAABBCCDD;

	printf("Write 0xAABBCCDD to 0x3E000\n");
	ret = no_os_flash_write(flash_dut, 0x3E000, &flash_val, 1);
	if (ret < 0)
		return ret;

	no_os_flash_read(flash_dut, 0x3E000, &flash_val, 1);
	printf("Address 0x3E000: %lX\n", flash_val);
	no_os_flash_read(flash_dut, 0x3E004, &flash_val, 1);
	printf("Address 0x3E004: %lX\n", flash_val);

	flash_val = 0xCCDDAABB;

	printf("Write 0xCCDDAABB to 0x3E004\n");
	ret = no_os_flash_write(flash_dut, 0x3E004, &flash_val, 1);
	if (ret < 0)
		return ret;

	no_os_flash_read(flash_dut, 0x3E000, &flash_val, 1);
	printf("Address 0x3E000: %lX\n", flash_val);
	no_os_flash_read(flash_dut, 0x3E004, &flash_val, 1);
	printf("Address 0x3E004: %lX\n", flash_val);

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

