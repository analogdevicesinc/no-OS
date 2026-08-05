/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of XILINX platform data used by eval-adxl367z project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct xil_spi_init_param spi_extra = {
	.type = SPI_PS,
	.flags = 0U,
#ifdef SDT
	.base_addr = XPAR_XSPIPS_0_BASEADDR,
#endif
};

#ifdef IIO_SUPPORT
struct xil_uart_init_param uart_extra_ip = {
	.type = UART_PS,
	.irq_id = UART_IRQ_ID,
#ifdef SDT
	.base_addr = XPAR_XUARTPS_0_BASEADDR,
#endif
};
#endif
