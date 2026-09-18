/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Xilinx platform data for ADAR300X project.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct xil_uart_init_param adar300x_uart_extra = {
	.type = UART_PS,
#ifdef SDT
	.base_addr = UART_BASEADDR,
#endif
	.irq_id = UART_IRQ_ID,
};

struct xil_spi_init_param adar300x_spi_extra = {
	.type = SPI_PS,
#ifdef SDT
	.base_addr = SPI_BASEADDR,
#endif
	.flags = 0,
};

struct xil_gpio_init_param adar300x_gpio_extra = {
	.type = GPIO_PS,
#ifdef SDT
	.base_addr = GPIO_BASEADDR,
#endif
	.device_id = GPIO_DEVICE_ID,
};
