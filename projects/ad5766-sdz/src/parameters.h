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
#define AD5766_SPI_ENG_REF_CLK_FREQ_HZ	XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ
#define AD5766_DMA_BASEADDR		XPAR_AXI_AD5766_DAC_DMA_BASEADDR
#define AD5766_CORE_BASEADDR		XPAR_SPI_AXI_AD5766_BASEADDR
#define AD5766_SPI_ENGINE_BASEADDR	XPAR_SPI_AXI_BASEADDR
#define AD5766_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0xA000000
#define SPI_AD5766_CS			0
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET			54 + 32
#define GPIO_RESET			GPIO_OFFSET + 0

#endif /* __PARAMETERS_H__ */
