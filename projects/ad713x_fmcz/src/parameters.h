/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define AD7134_DMA_BASEADDR		XPAR_AXI_AD7134_DMA_BASEADDR
#define AD7134_SPI_ENGINE_BASEADDR	XPAR_DUAL_AD7134_DUAL_AD7134_AXI_REGMAP_BASEADDR
#define AD713x_SPI_ENG_REF_CLK_FREQ_HZ	100000000
#define AD7134_1_SPI_CS			0
#define AD7134_2_SPI_CS			1
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET			54
#define GPIO_RESETN_1			GPIO_OFFSET + 32
#define GPIO_RESETN_2			GPIO_OFFSET + 33
#define GPIO_PDN_1			GPIO_OFFSET + 34
#define GPIO_PDN_2			GPIO_OFFSET + 35
#define GPIO_MODE_1			GPIO_OFFSET + 36
#define GPIO_MODE_2			GPIO_OFFSET + 37
#define GPIO_0				GPIO_OFFSET + 38
#define GPIO_1				GPIO_OFFSET + 39
#define GPIO_2				GPIO_OFFSET + 40
#define GPIO_3				GPIO_OFFSET + 41
#define GPIO_4				GPIO_OFFSET + 42
#define GPIO_5				GPIO_OFFSET + 43
#define GPIO_6				GPIO_OFFSET + 44
#define GPIO_7				GPIO_OFFSET + 45
#define GPIO_DCLKIO_1			GPIO_OFFSET + 46
#define GPIO_DCLKIO_2			GPIO_OFFSET + 47
#define GPIO_PINBSPI			GPIO_OFFSET + 48
#define GPIO_DCLKMODE			GPIO_OFFSET + 49
#define GPIO_CS_SYNC			GPIO_OFFSET + 50
#define GPIO_CS_SYNC_1			GPIO_OFFSET + 51
#define AD7134_FMC_CH_NO		8
#define AD7134_FMC_SAMPLE_NO		1024

#define ADC_BUFFER_SIZE			1000000

#ifdef IIO_SUPPORT
#define UART_BAUDRATE			115200
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif // IIO_SUPPORT

#endif /* __PARAMETERS_H__ */
