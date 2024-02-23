/***************************************************************************//**
 *   @file   ad9361/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#ifdef XILINX_PLATFORM
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define UART_BAUDRATE 921600
#define SYSID_BASEADDR			XPAR_AXI_SYSID_0_BASEADDR
#else
#define UART_BAUDRATE 115200
#endif
#ifdef XPAR_AXI_AD9361_0_BASEADDR
#define AD9361_RX_0_BASEADDR		XPAR_AXI_AD9361_0_BASEADDR
#define AD9361_TX_0_BASEADDR		XPAR_AXI_AD9361_0_BASEADDR + 0x4000
#else
#define AD9361_RX_0_BASEADDR		XPAR_AXI_AD9361_BASEADDR
#define AD9361_TX_0_BASEADDR		XPAR_AXI_AD9361_BASEADDR + 0x4000
#endif
#ifdef XPAR_AXI_AD9361_1_BASEADDR
#define AD9361_RX_1_BASEADDR		XPAR_AXI_AD9361_1_BASEADDR
#define AD9361_TX_1_BASEADDR		XPAR_AXI_AD9361_1_BASEADDR + 0x4000
#else
#ifdef XPAR_AXI_AD9361_0_BASEADDR
#define AD9361_RX_1_BASEADDR		XPAR_AXI_AD9361_0_BASEADDR
#define AD9361_TX_1_BASEADDR		XPAR_AXI_AD9361_0_BASEADDR + 0x4000
#else
#define AD9361_RX_1_BASEADDR		XPAR_AXI_AD9361_BASEADDR
#define AD9361_TX_1_BASEADDR		XPAR_AXI_AD9361_BASEADDR + 0x4000
#endif
#endif
#ifdef XPAR_AXI_DMAC_0_BASEADDR
#define CF_AD9361_RX_DMA_BASEADDR	XPAR_AXI_DMAC_0_BASEADDR
#else
#define CF_AD9361_RX_DMA_BASEADDR	XPAR_AXI_AD9361_ADC_DMA_BASEADDR
#endif
#ifdef XPAR_AXI_DMAC_1_BASEADDR
#define CF_AD9361_TX_DMA_BASEADDR	XPAR_AXI_DMAC_1_BASEADDR
#else
#define CF_AD9361_TX_DMA_BASEADDR	XPAR_AXI_AD9361_DAC_DMA_BASEADDR
#endif
#ifdef _XPARAMETERS_PS_H_
#define ADC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0x800000
#define DAC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0xA000000

#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#ifdef XPS_BOARD_ZCU102
#define GPIO_DEVICE_ID				XPAR_PSU_GPIO_0_DEVICE_ID
#ifdef FMCOMMS5
#define GPIO_RESET_PIN				130
#define GPIO_SYNC_PIN				129
#define GPIO_RESET_PIN_2			143
#define GPIO_CAL_SW1_PIN			137
#define GPIO_CAL_SW2_PIN			138
#else
#define GPIO_RESET_PIN				124
#define GPIO_SYNC_PIN				123
#define GPIO_RESET_PIN_2			113
#define GPIO_CAL_SW1_PIN			107
#define GPIO_CAL_SW2_PIN			108
#endif
#define GPIO_ENABLE_PIN				125
#define GPIO_TXNRX_PIN        		126
#define SPI_DEVICE_ID				XPAR_PSU_SPI_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_0_INTR
#else
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_RESET_PIN				100
#define GPIO_SYNC_PIN				99
#define GPIO_RESET_PIN_2			113
#define GPIO_CAL_SW1_PIN			107
#define GPIO_CAL_SW2_PIN			108
#define GPIO_ENABLE_PIN				101
#define GPIO_TXNRX_PIN        		102
#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#endif
#define GPIO_RESET_PIN_ZC702		84
#define GPIO_RESET_PIN_ZC706		83
#define GPIO_RESET_PIN_ZED			100
#define GPIO_CTL0_PIN				94
#define GPIO_CTL1_PIN				95
#define GPIO_CTL2_PIN				96
#define GPIO_CTL3_PIN				97

#else
#ifdef XPAR_DDR3_SDRAM_S_AXI_BASEADDR
#define ADC_DDR_BASEADDR			XPAR_DDR3_SDRAM_S_AXI_BASEADDR + 0x800000
#define DAC_DDR_BASEADDR			XPAR_DDR3_SDRAM_S_AXI_BASEADDR + 0xA000000
#else
#if defined(XPAR_AXI_DDR_CNTRL_BASEADDR)
#define ADC_DDR_BASEADDR			XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#define DAC_DDR_BASEADDR			XPAR_AXI_DDR_CNTRL_BASEADDR + 0xA000000
#else
#define ADC_DDR_BASEADDR			XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR + 0x800000
#define DAC_DDR_BASEADDR			XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR + 0xA000000
#endif
#endif

#define UART_DEVICE_ID                          XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID				XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID                             XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#define GPIO_DEVICE_ID				0
#define GPIO_RESET_PIN				46
#ifdef XPAR_AXI_SPI_0_DEVICE_ID
#define SPI_DEVICE_ID				XPAR_AXI_SPI_0_DEVICE_ID
#else
#define SPI_DEVICE_ID				XPAR_SPI_0_DEVICE_ID
#endif
#endif

#define SPI_CS                  0
#define SPI_CS_2                1

#define RX_CORE_BASEADDR	AD9361_RX_0_BASEADDR
#define TX_CORE_BASEADDR	AD9361_TX_0_BASEADDR
#endif

#define DAC_BUFFER_SAMPLES 1024
#define ADC_BUFFER_SAMPLES 16384
#define ADC_CHANNELS 4

#if defined LINUX_PLATFORM || defined GENERIC_PLATFORM
#define RX_CORE_BASEADDR			0
#define TX_CORE_BASEADDR			1
#define CF_AD9361_RX_DMA_BASEADDR	2
#define CF_AD9361_TX_DMA_BASEADDR	3

#define SPI_DEVICE_ID		0
#define SPI_CS			0

#define MAX_SIZE_BASE_ADDR 0x1000

#define DAC_DDR_BASEADDR	((uintptr_t)out_buff)
#define ADC_DDR_BASEADDR	((uintptr_t)in_buff)

#define GPIO_RESET_PIN	1006
#endif

/* Workaround for correcting the erroneous generation of defines
 * for DMA IRQs from the *.xsa file. This is a HDL known issue. */
#ifdef PLATFORM_ZYNQ
#define AD9361_DAC_DMA_IRQ_INTR		88
#define AD9361_ADC_DMA_IRQ_INTR		89
#else
#define AD9361_DAC_DMA_IRQ_INTR		XPAR_FABRIC_AXI_AD9361_DAC_DMA_IRQ_INTR
#define AD9361_ADC_DMA_IRQ_INTR		XPAR_FABRIC_AXI_AD9361_ADC_DMA_IRQ_INTR
#endif

#endif // __PARAMETERS_H__
