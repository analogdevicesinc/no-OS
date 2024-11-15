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

#define UART_BAUDRATE                           115200
#ifdef PLATFORM_MB
#define SPI_DEVICE_ID				XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_GPIO_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID                 XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#define GPIO_OFFSET				0

#define ADC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_BASEADDR  + 0x800000)
#else
#define SPI_DEVICE_ID				XPAR_XSPIPS_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_XGPIOPS_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR

#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#define GPIO_OFFSET				54

#define ADC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR + 0x800000)
#endif

#define RX_CORE_BASEADDR			XPAR_AXI_AD6676_CORE_ADC_TPL_CORE_BASEADDR
#define RX_DMA_BASEADDR				XPAR_AXI_AD6676_DMA_BASEADDR
#define RX_JESD_BASEADDR			XPAR_AXI_AD6676_JESD_RX_AXI_BASEADDR
#define RX_XCVR_BASEADDR			XPAR_AXI_AD6676_XCVR_BASEADDR

#define GPIO_ADC_OEN			(GPIO_OFFSET + 41)
#define GPIO_ADC_SELA			(GPIO_OFFSET + 40)
#define GPIO_ADC_SELB			(GPIO_OFFSET + 39)
#define GPIO_ADC_S0				(GPIO_OFFSET + 38)
#define GPIO_ADC_S1				(GPIO_OFFSET + 37)
#define GPIO_ADC_RESETB			(GPIO_OFFSET + 36)
#define GPIO_ADC_AGC1			(GPIO_OFFSET + 35)
#define GPIO_ADC_AGC2			(GPIO_OFFSET + 34)
#define GPIO_ADC_AGC3			(GPIO_OFFSET + 33)
#define GPIO_ADC_AGC4			(GPIO_OFFSET + 32)

#define GPIO_JESD204_SYSREF		(GPIO_OFFSET + 48)

#endif /* _PARAMETERS_H_ */
