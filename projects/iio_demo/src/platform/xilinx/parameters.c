/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of xilinx platform data used by iio_demo project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct xil_uart_init_param iio_demo_uart_extra_ip = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#ifdef SDT
	.base_addr = XPAR_XUARTLITE_0_BASEADDR,
#endif
#else
	.type = UART_PS,
	.irq_id = UART_IRQ_ID,
#ifdef SDT
	.base_addr = XPAR_XUARTPS_0_BASEADDR,
#endif
#endif
};
