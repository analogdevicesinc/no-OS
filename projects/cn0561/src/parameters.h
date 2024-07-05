/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define CN0561_DMA_BASEADDR		XPAR_AXI_CN0561_DMA_BASEADDR
#define CN0561_SPI_ENGINE_BASEADDR	XPAR_SPI_CN0561_SPI_CN0561_AXI_REGMAP_BASEADDR
#define CN0561_SPI_ENG_REF_CLK_FREQ_HZ	100000000
#define CN0561_SPI_CS			0
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET			54
#define GPIO_RESETN			GPIO_OFFSET + 32
#define GPIO_PDN			GPIO_OFFSET + 33
#define GPIO_MODE			GPIO_OFFSET + 34
#define GPIO_PINBSPI		GPIO_OFFSET + 35
#define GPIO_0				GPIO_OFFSET + 36
#define GPIO_1				GPIO_OFFSET + 37
#define GPIO_2				GPIO_OFFSET + 38
#define GPIO_4				GPIO_OFFSET + 39
#define GPIO_5				GPIO_OFFSET + 40
#define GPIO_6				GPIO_OFFSET + 41
#define GPIO_7				GPIO_OFFSET + 42
#define CN0561_FMC_CH_NO		4
#define CN0561_FMC_SAMPLE_NO	256

#define ADC_BUFFER_SIZE			1000000

//#define CN0561_CORAZ7S_CARRIER
//#define CN0561_ZED_CARRIER
//#define CN0561_REG_DUMP

#define CORA_Z7S_DATA_CLK_FREQ_HZ   24000000
#define ZED_DATA_CLK_FREQ_HZ        50000000

#ifdef IIO_SUPPORT
#define UART_BAUDRATE			115200
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_0_INTR
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif // IIO_SUPPORT

#endif /* __PARAMETERS_H__ */
