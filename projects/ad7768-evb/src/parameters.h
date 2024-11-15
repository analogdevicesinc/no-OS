/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <xparameters.h>

#define UART_BAUDRATE                           115200
#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_AD7768_CS				0
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET					32 + 54
#define AD7768_DMA_BASEADDR			XPAR_AD7768_DMA_2_BASEADDR
#define ADC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0x800000
#define AD7768_ADC_BASEADDR			XPAR_AXI_AD7768_ADC_BASEADDR
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_RESET_N				GPIO_OFFSET + 0
#define AD7768_HEADER_SIZE			8
#define BITS_IN_BYTE				8
#define AD7768_EVB_REFERENCE_VOLT		4.096f

#endif /* _PARAMETERS_H_ */
