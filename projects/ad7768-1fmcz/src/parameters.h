/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#define AD77681_EVB_SAMPLE_NO				8
#define AD77681_DMA_1_BASEADDR				XPAR_AXI_AD77681_DMA_BASEADDR
#define AD77681_SPI1_ENGINE_BASEADDR		XPAR_SPI_AD77681_SPI_AD77681_AXI_REGMAP_BASEADDR
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
