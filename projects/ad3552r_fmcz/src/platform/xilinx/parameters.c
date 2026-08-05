/***************************************************************************//**
*   @file   parameters.c
*   @brief  Implementation of Main Function.
*   @author Angelo Dureghello (adureghello@baylibre.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <parameters.h>

struct xil_gpio_init_param xil_gpio_param = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS,
#ifdef SDT
	.base_addr = XPAR_XGPIOPS_0_BASEADDR,
#endif
};

struct xil_spi_init_param xil_spi_param = {
	.type = SPI_PS,
	.flags = 0,
#ifdef SDT
	.base_addr = XPAR_XSPIPS_0_BASEADDR,
#endif
};

#ifdef IIO_SUPPORT
struct xil_uart_init_param platform_uart_param = {
	.type = UART_PS,
	.irq_id = UART_IRQ_ID,
#ifdef SDT
	.base_addr = XPAR_XUARTPS_0_BASEADDR,
#endif
};
#endif
