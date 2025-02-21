/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Cristian Pop (cristian.pop@analog.com)
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <xparameters.h>

#define AXI_PWMGEN_BASEADDR			XPAR_SPI_TRIGGER_GEN_BASEADDR
#define ADAQ7980_SPI_ENG_REF_CLK_FREQ_HZ	XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ
#define ADAQ7980_DMA_BASEADDR			XPAR_AXI_ADAQ7980_DMA_BASEADDR
#define ADAQ7980_SPI_ENGINE_BASEADDR		XPAR_SPI_ADAQ7980_ADC_SPI_ADAQ7980_ADC_AXI_REGMAP_BASEADDR
#define SPI_ADAQ7980_CS				0
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				32 + 54
#define GPIO_0					GPIO_OFFSET + 0
#define GPIO_1					GPIO_OFFSET + 1
#define GPIO_2					GPIO_OFFSET + 2
#define GPIO_3					GPIO_OFFSET + 3
#define GPIO_4					GPIO_OFFSET + 4
#define GPIO_5					GPIO_OFFSET + 5
#define GPIO_6					GPIO_OFFSET + 6
#define GPIO_7					GPIO_OFFSET + 7
#define GPIO_REF_PUB				GPIO_OFFSET + 8
#define GPIO_RBUF_PUB				GPIO_OFFSET + 9
#define ADC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0x800000

#endif /* __PARAMETERS_H__ */
