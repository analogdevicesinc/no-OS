/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD77681_EVB_SAMPLE_NO				8
#define AD77681_DMA_1_BASEADDR				XPAR_AXI_AD77681_DMA_BASEADDR
#define AD77681_SPI1_ENGINE_BASEADDR		XPAR_SPI_ADC_AXI_REGMAP_BASEADDR
#define AD77681_SPI_CS						0
#define AD77681_SPI_ENG_REF_CLK_FREQ_HZ		80000000

#define GPIO_DEVICE_ID						XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET							32

#define GPIO_0_0							GPIO_OFFSET + 6 // 38
#define GPIO_0_1							GPIO_OFFSET + 5 // 37
#define GPIO_0_2							GPIO_OFFSET + 4 // 36
#define GPIO_0_3							GPIO_OFFSET + 3 // 35
#define GPIO_0_SYNC_IN						GPIO_OFFSET + 2 // 34
#define GPIO_0_SYNC_OUT						GPIO_OFFSET + 1 // 33
#define GPIO_0_RESET						GPIO_OFFSET + 0 // 32

#endif /* PARAMETERS_H_ */
