/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Xilinx platform-specific definitions for the xilinx_lwip project.
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "xilinx_uart.h"

/* PS (Zynq-7000 / ZynqMP) peripheral IDs */
#ifdef _XPARAMETERS_PS_H_
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#else
/* Fallback for MicroBlaze / PL UART */
#define UART_DEVICE_ID		XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#endif

#define UART_EXTRA		&xilinx_lwip_uart_extra_ip
#define UART_OPS		&xil_uart_ops

/* Buffer depth for virtual IIO demo devices */
#define SAMPLES_PER_CHANNEL_PLATFORM	2000
#define UART_BAUDRATE			        115200

/*
 * GEM instance selection. Override at build time with -DGEM_INSTANCE=1
 */
#ifndef GEM_INSTANCE
#define GEM_INSTANCE	0
#endif

#if GEM_INSTANCE == 0 && defined(XPAR_XEMACPS_0_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#elif GEM_INSTANCE == 1 && defined(XPAR_XEMACPS_1_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_1_DEVICE_ID
#elif GEM_INSTANCE == 2 && defined(XPAR_XEMACPS_2_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_2_DEVICE_ID
#elif GEM_INSTANCE == 3 && defined(XPAR_XEMACPS_3_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_3_DEVICE_ID
#else
#error "GEM_INSTANCE not available - check your .xsa / BSP configuration"
#endif

extern struct xil_uart_init_param xilinx_lwip_uart_extra_ip;

#endif /* __PARAMETERS_H__ */
