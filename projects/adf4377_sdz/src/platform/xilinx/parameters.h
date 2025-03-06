/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include <xparameters.h>

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_CS				0
#define SPI_EXTRA 			&xil_spi_extra
#define GPIO_EXTRA			&xil_gpio_init_param

#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID

#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR

#define UART_BAUDRATE	            	115200

#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID

#define GPIO_OFFSET			32 + 54
#define GPIO_MUXOUT                 	GPIO_OFFSET
#define GPIO_LKDET                  	GPIO_OFFSET + 1
#define GPIO_ENCLK2                 	GPIO_OFFSET + 2
#define GPIO_CE				GPIO_OFFSET + 3
#define GPIO_ENCLK1                 	GPIO_OFFSET + 4

extern struct xil_spi_init_param xil_spi_extra;
extern struct xil_gpio_init_param xil_gpio_init_param;

#endif /* _PARAMETERS_H_ */
