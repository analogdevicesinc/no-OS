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

#include "app_config.h"
#include "xparameters.h"

#ifdef PLATFORM_MB
#define SPI_DEVICE_ID				XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_GPIO_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID				XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#define GPIO_OFFSET				0

#define ADC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000)
#else
#define SPI_DEVICE_ID				XPAR_XSPIPS_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_XGPIOPS_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR

#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#ifdef PLATFORM_ZYNQ
#define GPIO_OFFSET				54
#endif

#define ADC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR + 0x800000)
#endif

#define UART_BAUDRATE	            		115200

#define RX_CORE_BASEADDR			XPAR_AXI_AD9250_CORE_ADC_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR			XPAR_AXI_AD9250_DMA_BASEADDR

#define RX_JESD_BASEADDR			XPAR_AXI_AD9250_JESD_RX_AXI_BASEADDR

#define RX_XCVR_BASEADDR			XPAR_AXI_AD9250_XCVR_BASEADDR

#define GPIO_JESD204_SYSREF			(GPIO_OFFSET + 32)

#endif /* _PARAMETERS_H_ */
