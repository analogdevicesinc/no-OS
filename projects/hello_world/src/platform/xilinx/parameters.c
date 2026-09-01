/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Xilinx platform parameter definitions for hello_world project.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

/*
 * Extra initialisation parameters for the Xilinx UART driver.
 *
 * XPAR_XUARTLITE_NUM_INSTANCES is only defined when the design contains an
 * AXI UART Lite (MicroBlaze / PL-only designs).  On Zynq/ZynqMP the PS UART
 * is used instead.
 */
struct xil_uart_init_param hello_world_uart_extra_ip = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#ifdef SDT
	.base_addr = XPAR_XUARTLITE_0_BASEADDR,
#endif
#else
	.type = UART_PS,
#ifdef SDT
	.base_addr = XPAR_XUARTPS_0_BASEADDR,
#endif
	.irq_id = UART_IRQ_ID,
#endif
};
