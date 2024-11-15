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

#define SPI_DEVICE_ID				XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_GPIO_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_AXI_UART_DEVICE_ID
#define UART_BAUDRATE                           115200
#define UART_IRQ_ID                             XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#define ADC_MAX_SAMPLES				1000000
#define RX_0_CORE_BASEADDR			XPAR_AXI_AD9625_0_CORE_BASEADDR
#define RX_1_CORE_BASEADDR			XPAR_AXI_AD9625_1_CORE_BASEADDR

#define RX_DMA_BASEADDR				XPAR_AXI_AD9625_DMA_BASEADDR

#define RX_0_JESD_BASEADDR			XPAR_AXI_AD9625_0_JESD_RX_AXI_BASEADDR
#define RX_1_JESD_BASEADDR			XPAR_AXI_AD9625_1_JESD_RX_AXI_BASEADDR

#define RX_0_XCVR_BASEADDR			XPAR_AXI_AD9625_0_XCVR_BASEADDR
#define RX_0_XCVR_BASEADDR			XPAR_AXI_AD9625_0_XCVR_BASEADDR

#define GPIO_JESD204_SYSREF			32
#define GPIO_RST_0					34
#define GPIO_PWDN_0					35
#define GPIO_RST_1					38
#define GPIO_PWDN_1					39
#define GPIO_IRQ_0					40
#define GPIO_FD_0					41
#define GPIO_IRQ_1					42
#define GPIO_FD_1					43
#define GPIO_PWR_GOOD				44
#define GPIO_SYSREF_DELAY			56

#endif /* _PARAMETERS_H_ */
