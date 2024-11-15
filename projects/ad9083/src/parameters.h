/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

#define GPIO_DEVICE_ID		XPAR_PSU_GPIO_0_DEVICE_ID
#define GPIO_OFFSET		78
#define AD9803_PWDN		(GPIO_OFFSET + 32)
#define AD9803_RSTB		(GPIO_OFFSET + 33)
#define AD9528_REFSEL		(GPIO_OFFSET + 34)

#define CLK_AD9258_CS		0x01
#define SPI_AD9083_CS		0x00
#define UART_DEVICE_ID          XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

#define RX_JESD_BASEADDR	XPAR_AXI_AD9083_RX_JESD_RX_AXI_BASEADDR
#define RX_XCVR_BASEADDR	XPAR_AXI_AD9083_RX_XCVR_BASEADDR
#define RX_CORE_BASEADDR	XPAR_RX_AD9083_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define RX_DMA_BASEADDR		XPAR_AXI_AD9083_RX_DMA_BASEADDR
#define ADC_DDR_BASEADDR	(XPAR_DDR_MEM_BASEADDR + 0x800000)
#define UART_BAUDRATE		115200

//#define SUBCLASS_1

#endif /* __PARAMETERS_H__ */
