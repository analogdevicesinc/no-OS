/***************************************************************************//**
 *   @file   ad9467/src/devices/adi_hal/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "app_config.h"
#include "xparameters.h"

#ifdef PLATFORM_MB
#define SPI_DEVICE_ID 				XPAR_AXI_SPI_DEVICE_ID
#define ADC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000)
#define UART_DEVICE_ID				XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID				XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#else
#define SPI_DEVICE_ID 				XPAR_XSPIPS_0_DEVICE_ID
#define ADC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR  + 0x800000)
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR

#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif

#define UART_BAUDRATE				115200
#define RX_CORE_BASEADDR 			XPAR_AXI_AD9467_BASEADDR
#define RX_DMA_BASEADDR  			XPAR_AXI_AD9467_DMA_BASEADDR

#endif /* SRC_PARAMETERS_H_ */
