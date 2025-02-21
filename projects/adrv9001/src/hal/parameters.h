/***************************************************************************//**
 *   @file   adrv9001/src/hal/parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Darius Berghe (darius.berghe@analog.com)
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

#include "xparameters.h"

#ifndef ADRV9002_RX2TX2
#define IIO_DEV_COUNT 		2
#define IIO_DEV_CHANNELS	1
#else
#define IIO_DEV_COUNT 		1
#define IIO_DEV_CHANNELS	2
#endif

#define ADRV9001_I_Q_CHANNELS			(IIO_DEV_CHANNELS * 2)

#define DAC_BUFFER_SAMPLES 1024
#define ADC_BUFFER_SAMPLES 16384

#ifdef XPS_BOARD_ZCU102
#define GPIO_OFFSET			78
#else
#define GPIO_OFFSET			54
#endif

/* GPIO */
#define GPIO_RESET			(46 + GPIO_OFFSET)
#define GPIO_SSI_SYNC			(54 + GPIO_OFFSET)

/* SPI */
#define SPI_CS				0

#ifdef XPS_BOARD_ZCU102
#define SPI_DEVICE_ID			XPAR_PSU_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PSU_GPIO_0_DEVICE_ID
#else
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#endif

/* UART */
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_BAUDRATE			921600
#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#endif

/* Interrupt */
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID

/* DMA Controller */
#define RX1_DMA_BASEADDR		XPAR_AXI_ADRV9001_RX1_DMA_BASEADDR
#define RX2_DMA_BASEADDR		XPAR_AXI_ADRV9001_RX2_DMA_BASEADDR
#define TX1_DMA_BASEADDR		XPAR_AXI_ADRV9001_TX1_DMA_BASEADDR
#define TX2_DMA_BASEADDR		XPAR_AXI_ADRV9001_TX2_DMA_BASEADDR

/* AXI ADC/DAC */
#define RX1_ADC_BASEADDR		XPAR_AXI_ADRV9001_BASEADDR
#define RX2_ADC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x1000)
#define TX1_DAC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x2000)
#define TX2_DAC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x4000)

#endif
