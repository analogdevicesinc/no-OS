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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

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
#define INTC_DEVICE_ID		XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#endif /* _XPARAMETERS_PS_H_ */

#define UART_BAUDRATE		115200
#define UART_EXTRA		&hello_world_uart_extra_ip
#define UART_OPS		&xil_uart_ops

extern struct xil_uart_init_param hello_world_uart_extra_ip;

#endif /* __PARAMETERS_H__ */