/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Xilinx platform parameter definitions for hello_world project.
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

struct xil_spi_init_param hello_world_spi_extra_ip = {
#ifdef XPAR_XSPI_NUM_INSTANCES
	.type = SPI_PL,
#ifdef SDT
	.base_addr = XPAR_XSPI_0_BASEADDR,
#endif
#else
	.type = SPI_PS,
#ifdef SDT
	.base_addr = XPAR_XSPIPS_0_BASEADDR,
#endif
#endif
	.flags = 0
};

struct xil_gpio_init_param hello_world_gpio_extra_ip = {
#ifdef XPAR_XGPIO_NUM_INSTANCES
	.type = GPIO_PL,
#ifdef SDT
	.base_addr = XPAR_XGPIO_0_BASEADDR,
#endif
#else
	.type = GPIO_PS,
#ifdef SDT
	.base_addr = XPAR_XGPIOPS_0_BASEADDR,
#endif
#endif
	.device_id = GPIO_DEVICE_ID
};
