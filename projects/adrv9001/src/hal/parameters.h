/***************************************************************************//**
 *   @file   adrv9001/src/hal/parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADRV9001_NUM_SUBCHANNELS	2 /* I - in-phase and Q - quadrature channels */
#define ADRV9001_NUM_CHAIN_CHANNELS	2 /* channels per RX/TX chain */
#define ADRV9001_NUM_CHANNELS		(ADRV9001_NUM_CHAIN_CHANNELS * ADRV9001_NUM_SUBCHANNELS)

#if defined(PLATFORM_MB)
#define GPIO_OFFSET			54
#else
#define GPIO_OFFSET			78
#endif

/* GPIO */
#define GPIO_DEVICE_ID			XPAR_PSU_GPIO_0_DEVICE_ID
#define GPIO_RESET			(46 + GPIO_OFFSET)
#define GPIO_SSI_SYNC			(54 + GPIO_OFFSET)

/* SPI */
#define SPI_CS				0
#define SPI_DEVICE_ID			XPAR_PSU_SPI_0_DEVICE_ID

/* UART */
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#endif

/* Interrupt */
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif

/* DMA Controller */
#define RX1_DMA_BASEADDR		XPAR_AXI_ADRV9001_RX1_DMA_BASEADDR
#define RX2_DMA_BASEADDR		XPAR_AXI_ADRV9001_RX2_DMA_BASEADDR
#define TX1_DMA_BASEADDR		XPAR_AXI_ADRV9001_TX1_DMA_BASEADDR
#define TX2_DMA_BASEADDR		XPAR_AXI_ADRV9001_TX2_DMA_BASEADDR

/* Memory */
#define DDR_MEM_BASEADDR		XPAR_DDR_MEM_BASEADDR
#define ADC1_DDR_BASEADDR		(DDR_MEM_BASEADDR + 0x800000)
#define ADC2_DDR_BASEADDR		(DDR_MEM_BASEADDR + 0x900000)
#define DAC1_DDR_BASEADDR		(DDR_MEM_BASEADDR + 0xA000000)
#define DAC2_DDR_BASEADDR		(DDR_MEM_BASEADDR + 0xA100000)

/* AXI ADC/DAC */
#define RX1_ADC_BASEADDR		XPAR_AXI_ADRV9001_BASEADDR
#define RX2_ADC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x1000)
#define TX1_DAC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x2000)
#define TX2_DAC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x4000)