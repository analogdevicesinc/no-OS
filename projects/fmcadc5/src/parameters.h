/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "app_config.h"
#include "xparameters.h"

#define SPI_DEVICE_ID				XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_GPIO_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_AXI_UART_DEVICE_ID
#define UART_BAUDRATE                           115200
#define UART_IRQ_ID                             XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#define ADC_MAX_SAMPLES				1000000
#define RX_0_CORE_BASEADDR			XPAR_AXI_AD9625_0_CORE_BASEADDR
#define RX_1_CORE_BASEADDR			XPAR_AXI_AD9625_1_CORE_BASEADDR

#define RX_DMA_BASEADDR				XPAR_AXI_AD9625_DMA_BASEADDR

#define RX_0_JESD_BASEADDR			XPAR_AXI_AD9625_0_JESD_RX_AXI_BASEADDR
#define RX_1_JESD_BASEADDR			XPAR_AXI_AD9625_1_JESD_RX_AXI_BASEADDR

#define RX_0_XCVR_BASEADDR			XPAR_AXI_AD9625_0_XCVR_BASEADDR
#define RX_0_XCVR_BASEADDR			XPAR_AXI_AD9625_0_XCVR_BASEADDR

#define GPIO_JESD204_SYSREF			32
#define GPIO_RST_0					34
#define GPIO_PWDN_0					35
#define GPIO_RST_1					38
#define GPIO_PWDN_1					39
#define GPIO_IRQ_0					40
#define GPIO_FD_0					41
#define GPIO_IRQ_1					42
#define GPIO_FD_1					43
#define GPIO_PWR_GOOD				44
#define GPIO_SYSREF_DELAY			56

#endif /* _PARAMETERS_H_ */
