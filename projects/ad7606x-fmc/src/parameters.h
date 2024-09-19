/***************************************************************************//**
* @file parameters.h
* @brief Parameters definition for AD7606X-FMC.
* @author Alexandru Ardelean (aardelean@baylibre.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD7606X_MODE_SPI_ENGINE			0
#define AD7606X_MODE_PARALLEL			1

#if defined(XPAR_AXI_SPI_ENGINE_0_BASEADDR)
#define AD7606X_CORE_BASEADDR			XPAR_AXI_SPI_ENGINE_0_BASEADDR
#define AD7606X_MODE				AD7606X_MODE_SPI_ENGINE
#define AD7606X_RX_CLKGEN_BASEADDR		XPAR_SPI_CLKGEN_BASEADDR
#elif defined(XPAR_AXI_AD7606X_BASEADDR)
#define AD7606X_CORE_BASEADDR			XPAR_AXI_AD7606X_BASEADDR
#define AD7606X_MODE				AD7606X_MODE_PARALLEL
#define AD7606X_RX_CLKGEN_BASEADDR		0
#endif

#define AD7606X_DMA_BASEADDR			XPAR_AXI_DMAC_0_BASEADDR
#define AXI_PWMGEN_BASEADDR			XPAR_AXI_PWM_GEN_0_BASEADDR
#define SPI_AD7606X_CS				0
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				(32 + 54)
#define GPIO_ADC_OS0				(GPIO_OFFSET + 0)
#define GPIO_ADC_OS1				(GPIO_OFFSET + 1)
#define GPIO_ADC_OS2				(GPIO_OFFSET + 2)
#define GPIO_ADC_RANGE				(GPIO_OFFSET + 3)
#define GPIO_ADC_STBY				(GPIO_OFFSET + 4)
#define GPIO_ADC_RESET				(GPIO_OFFSET + 5)
#define GPIO_ADC_FIRST_DATA			(GPIO_OFFSET + 6)
#define GPIO_ADC_SER_PAR			(GPIO_OFFSET + 7)
#define ADC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR + 0x800000)

#define AD7606X_TRIGGER_PULSE_WIDTH_NS		120

#endif /* PARAMETERS_H_ */
