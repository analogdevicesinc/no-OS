/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Application parameters.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef APP_PARAMETERS_H_
#define APP_PARAMETERS_H_

#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define UART_BAUDRATE 115200

#ifdef XPS_BOARD_ZCU102
#define GPIO_OFFSET		78
#else
#define GPIO_OFFSET		0
#endif

#define PHY_CS			0

#ifdef QUAD_MXFE
#define ADF4371_CS		0
#define HMC7043_CS		4

#define PHY_RESET		(GPIO_OFFSET + 41)

#define ADRF5020_CTRL_GPIO	(GPIO_OFFSET + 34)
#define AD9081_GPIO_0_MUX	(GPIO_OFFSET + 44)

#define GPIO_2_DEVICE_ID	XPAR_AXI_GPIO_2_DEVICE_ID
#define SPI_2_DEVICE_ID		XPAR_AXI_SPI_2_DEVICE_ID

#else
#define PHY_RESET		(GPIO_OFFSET + 55)
#endif

#if defined(PLATFORM_MB)
#define GPIO_DEVICE_ID		XPAR_AXI_GPIO_DEVICE_ID
#define PHY_SPI_DEVICE_ID	XPAR_AXI_SPI_DEVICE_ID
#define CLK_SPI_DEVICE_ID	XPAR_AXI_SPI_DEVICE_ID
#define UART_DEVICE_ID		XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#define INTC_DEVICE_ID		XPAR_INTC_SINGLE_DEVICE_ID
#define DDR_CNTRL_BASEADDR	XPAR_AXI_DDR_CNTRL_BASEADDR
#define CLK_CS			1
#elif defined(PLATFORM_ZYNQMP)
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_DEVICE_ID		XPAR_PSU_GPIO_0_DEVICE_ID
#define PHY_SPI_DEVICE_ID	XPAR_PSU_SPI_0_DEVICE_ID
#define CLK_SPI_DEVICE_ID	XPAR_PSU_SPI_1_DEVICE_ID
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#define DDR_CNTRL_BASEADDR	XPAR_PSU_DDRC_0_BASEADDR
#define CLK_CS			0
#else
#error Unsupported platform.
#endif

#define RX_JESD_BASEADDR	XPAR_AXI_MXFE_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR	XPAR_AXI_MXFE_TX_JESD_TX_AXI_BASEADDR

#ifdef XPAR_AXI_MXFE_RX_XCVR_BASEADDR
#define RX_XCVR_BASEADDR	XPAR_AXI_MXFE_RX_XCVR_BASEADDR
#endif
#ifdef XPAR_AXI_MXFE_TX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR	XPAR_AXI_MXFE_TX_XCVR_BASEADDR
#endif

#ifdef XPAR_RX_MXFE_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define RX_CORE_BASEADDR	XPAR_RX_MXFE_TPL_CORE_ADC_TPL_CORE_BASEADDR
#else
#define RX_CORE_BASEADDR	XPAR_RX_MXFE_TPL_CORE_TPL_CORE_BASEADDR
#endif
#ifdef XPAR_TX_MXFE_TPL_CORE_DAC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR	XPAR_TX_MXFE_TPL_CORE_DAC_TPL_CORE_BASEADDR
#else
#define TX_CORE_BASEADDR	XPAR_TX_MXFE_TPL_CORE_TPL_CORE_BASEADDR
#endif

#define RX_DMA_BASEADDR		XPAR_AXI_MXFE_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR		XPAR_AXI_MXFE_TX_DMA_BASEADDR

#ifdef IIO_SUPPORT

#define MAX_DAC_BUF_SAMPLES 10000000 //1MB
#define MAX_ADC_BUF_SAMPLES 10000000 //1MB

#endif

#endif
