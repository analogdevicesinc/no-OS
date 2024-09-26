/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <xparameters.h>

#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define RX_CORE_BASEADDR			XPAR_AXI_ADAQ8092_BASEADDR
#define RX_DMA_BASEADDR				XPAR_AXI_ADAQ8092_DMA_BASEADDR
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE                           115200
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				32 + 54

#define GPIO_PAR_SER_NR				GPIO_OFFSET
#define GPIO_PD1_NR				GPIO_OFFSET+1
#define GPIO_PD2_NR			    	GPIO_OFFSET+2
#define GPIO_1V8_NR			   	GPIO_OFFSET+3

#endif /* _PARAMETERS_H_ */
