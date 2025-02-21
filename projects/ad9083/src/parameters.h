/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <xparameters.h>

#define GPIO_DEVICE_ID		XPAR_PSU_GPIO_0_DEVICE_ID
#define GPIO_OFFSET		78
#define AD9803_PWDN		(GPIO_OFFSET + 32)
#define AD9803_RSTB		(GPIO_OFFSET + 33)
#define AD9528_REFSEL		(GPIO_OFFSET + 34)

#define CLK_AD9258_CS		0x01
#define SPI_AD9083_CS		0x00
#define UART_DEVICE_ID          XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

#define RX_JESD_BASEADDR	XPAR_AXI_AD9083_RX_JESD_RX_AXI_BASEADDR
#define RX_XCVR_BASEADDR	XPAR_AXI_AD9083_RX_XCVR_BASEADDR
#define RX_CORE_BASEADDR	XPAR_RX_AD9083_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define RX_DMA_BASEADDR		XPAR_AXI_AD9083_RX_DMA_BASEADDR
#define ADC_DDR_BASEADDR	(XPAR_DDR_MEM_BASEADDR + 0x800000)
#define UART_BAUDRATE		115200

//#define SUBCLASS_1

#endif /* __PARAMETERS_H__ */
