/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <xparameters.h>

#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define RX_CORE_BASEADDR			XPAR_AXI_ADAQ8092_BASEADDR
#define RX_DMA_BASEADDR				XPAR_AXI_ADAQ8092_DMA_BASEADDR
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE                           115200
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				32 + 54

#define GPIO_PAR_SER_NR				GPIO_OFFSET
#define GPIO_PD1_NR				GPIO_OFFSET+1
#define GPIO_PD2_NR			    	GPIO_OFFSET+2
#define GPIO_1V8_NR			   	GPIO_OFFSET+3

#endif /* _PARAMETERS_H_ */
