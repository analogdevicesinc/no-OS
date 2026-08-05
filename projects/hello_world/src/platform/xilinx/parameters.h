/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Xilinx platform parameters for hello_world project.
 *
 *           This file selects the correct UART peripheral identifiers based on
 *           the target hardware. All values come from the BSP-generated
 *           xparameters.h, so the same source builds correctly for:
 *
 *             - Any Zynq-7000 board (Cora Z7, ZED, ZC706, ...): PS UART 1
 *             - ZCU102 (ZynqMP): PS UART 0
 *             - MicroBlaze (PL-only): AXI UART Lite
 *
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <xparameters.h>
#include <xil_cache.h>
#include <xilinx_uart.h>

/*
 * _XPARAMETERS_PS_H_ is defined whenever the design contains an ARM
 * Processing System (PS) -- i.e. Zynq-7000 or ZynqMP.  When it is NOT
 * defined we are running on a MicroBlaze soft-core inside the PL and must
 * use an AXI UART Lite peripheral instead.
 */
#ifdef _XPARAMETERS_PS_H_

#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID

/*
 * The ZCU102 routes its JTAG/USB-UART adapter to UART0; all other Zynq-7000
 * and ZynqMP boards (Cora Z7, ZED, ZC706, ...) use UART1.
 */
#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#endif

#else /* MicroBlaze / PL-only */

#define UART_DEVICE_ID		XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#endif /* _XPARAMETERS_PS_H_ */

#define UART_BAUDRATE		115200
#define UART_EXTRA		&hello_world_uart_extra_ip
#define UART_OPS		&xil_uart_ops

extern struct xil_uart_init_param hello_world_uart_extra_ip;

#endif /* __PARAMETERS_H__ */
