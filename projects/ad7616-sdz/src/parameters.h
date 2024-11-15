/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters definition for AD7616-SDZ.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7616_SPI_ENGINE_BASEADDR  XPAR_SPI_AD7616_SPI_AD7616_AXI_REGMAP_BASEADDR
#define AD7616_DMA_BASEADDR			XPAR_AXI_AD7616_DMA_BASEADDR
#define AD7616_RX_CLKGEN_BASEADDR   XPAR_SPI_CLKGEN_BASEADDR
#define AXI_PWMGEN_BASEADDR         XPAR_AXI_PWM_GEN_0_BASEADDR
#define SPI_AD7616_CS				0
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET					32 + 54
#define GPIO_ADC_CRCEN				GPIO_OFFSET + 0
#define GPIO_ADC_CHSEL0				GPIO_OFFSET + 1
#define GPIO_ADC_CHSEL1				GPIO_OFFSET + 2
#define GPIO_ADC_CHSEL2				GPIO_OFFSET + 3
#define GPIO_ADC_BURST				GPIO_OFFSET + 4
#define GPIO_ADC_SEQEN				GPIO_OFFSET + 5
#define GPIO_ADC_OS0				GPIO_OFFSET + 6
#define GPIO_ADC_OS1				GPIO_OFFSET + 7
#define GPIO_ADC_OS2				GPIO_OFFSET + 8
#define GPIO_ADC_HW_RNGSEL0			GPIO_OFFSET + 9
#define GPIO_ADC_HW_RNGSEL1			GPIO_OFFSET + 10
#define GPIO_ADC_RESET_N			GPIO_OFFSET + 11
#define ADC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0x800000

#endif /* PARAMETERS_H_ */
