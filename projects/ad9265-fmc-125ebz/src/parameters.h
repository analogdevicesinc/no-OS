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

#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define RX_CORE_BASEADDR			XPAR_AXI_AD9265_BASEADDR
#define RX_DMA_BASEADDR				XPAR_AXI_AD9265_DMA_BASEADDR
#define ADC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0x800000
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE                           115200
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#endif /* _PARAMETERS_H_ */
