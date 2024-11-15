/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

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
