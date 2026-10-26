/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of xilinx platform data used by ad9088 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

struct xil_uart_init_param platform_uart_extra_ip = {
#ifdef PLATFORM_MB
	.type = UART_PL,
#else
	.type = UART_PS,
#endif
	.irq_id = UART_IRQ_ID,
#ifdef SDT
	.base_addr = XPAR_XUARTLITE_0_BASEADDR,
#endif
};

struct xil_spi_init_param clk_spi_extra_ip = {
	.flags = 0,
#ifdef PLATFORM_MB
	.type = SPI_PL,
#else
	.type = SPI_PS,
#endif
#ifdef SDT
#ifdef PLATFORM_MB
	.base_addr = XPAR_XSPI_0_BASEADDR,
#else
	.base_addr = XPAR_XSPIPS_0_BASEADDR,
#endif
#endif
};

struct xil_spi_init_param apollo_spi_extra_ip = {
	.type = SPI_PL,
#ifdef SDT
	.base_addr = XPAR_XSPI_1_BASEADDR,
#endif
};

struct xil_gpio_init_param xil_gpio_param = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PL,
#ifdef SDT
	.base_addr = XPAR_XGPIO_0_BASEADDR,
#endif
};
