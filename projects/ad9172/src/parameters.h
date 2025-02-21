/***************************************************************************//**
 *   @file   ad9172/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifdef PLATFORM_ZYNQMP
#define SPI_DEVICE_ID				XPAR_PSU_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_PSU_GPIO_0_DEVICE_ID
#else
#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#endif
#define SPI_HMC7044_CS				0x00
#define SPI_AD9172_CS				0x01

#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE                           921600

#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#define TX_JESD_BASEADDR			XPAR_DAC_JESD204_LINK_TX_AXI_BASEADDR
#define TX_XCVR_BASEADDR			XPAR_DAC_JESD204_XCVR_BASEADDR
#define TX_CORE_BASEADDR			XPAR_DAC_JESD204_TRANSPORT_DAC_TPL_CORE_BASEADDR
#define TX_DMA_BASEADDR				XPAR_DAC_DMA_BASEADDR
#ifdef PLATFORM_ZYNQMP
#define DDR_MEM_BASEADDR                        (XPAR_PSU_DDR_0_S_AXI_BASEADDR + 0xA000000)
#else
#define DDR_MEM_BASEADDR 			(XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0xA000000)
#endif

#endif /* __PARAMETERS_H__ */
