/***************************************************************************//**
 *   @file   ad9208/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

#define SPI_HMC7044_CS		0x00

#define SPI_0_DEVICE_ID		XPAR_SPI_0_DEVICE_ID
#define SPI_AD9208_0_CS		0x01
#define RX_0_JESD_BASEADDR 	XPAR_AXI_AD9208_0_JESD_RX_AXI_BASEADDR
#define RX_0_XCVR_BASEADDR 	XPAR_AXI_AD9208_0_XCVR_BASEADDR
#define RX_0_CORE_BASEADDR	XPAR_RX_AD9208_0_TPL_CORE_ADC_TPL_CORE_BASEADDR

#define SPI_DEVICE_ID		XPAR_AXI_SPI_DEVICE_ID
#define SPI_AD9208_CS		0x02

#define GPIO_DEVICE_ID		XPAR_GPIO_0_DEVICE_ID

#define RX_1_JESD_BASEADDR 	XPAR_AXI_AD9208_1_JESD_RX_AXI_BASEADDR
#define RX_1_XCVR_BASEADDR 	XPAR_AXI_AD9208_1_XCVR_BASEADDR
#define RX_1_CORE_BASEADDR	XPAR_RX_AD9208_1_TPL_CORE_ADC_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR		XPAR_AXI_AD9208_DMA_BASEADDR
#define ADC_DDR_BASEADDR	(XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR + 0x800000)
#define ADC_DDR_HIGHADDR	XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR

/* iio_app parameters */
#define UART_DEVICE_ID          XPAR_AXI_UART_DEVICE_ID
#define UART_BAUDRATE           115200
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#define INTC_DEVICE_ID          XPAR_INTC_SINGLE_DEVICE_ID

#endif /* __PARAMETERS_H__ */
