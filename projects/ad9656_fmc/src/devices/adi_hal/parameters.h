/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author DHotolea (dan.hotoleanu@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "xparameters.h"

#define SPI_DEVICE_ID				XPAR_XSPIPS_0_DEVICE_ID

#define ADC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR + 0x800000)

#define RX_CORE_BASEADDR			XPAR_RX_AD9656_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define RX_DMA_BASEADDR				XPAR_AXI_AD9656_RX_DMA_BASEADDR
#define RX_JESD_BASEADDR			XPAR_AXI_AD9656_RX_JESD_RX_AXI_BASEADDR
#define RX_XCVR_BASEADDR			XPAR_AXI_AD9656_XCVR_BASEADDR
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_0_INTR
#define UART_BAUDRATE                           115200
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#endif /* _PARAMETERS_H_ */
