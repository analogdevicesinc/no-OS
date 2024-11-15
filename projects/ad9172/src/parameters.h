/***************************************************************************//**
 *   @file   ad9172/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Cristian Pop (cristian.pop@analog.com)
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

#ifdef PLATFORM_ZYNQMP
#define SPI_DEVICE_ID				XPAR_PSU_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_PSU_GPIO_0_DEVICE_ID
#else
#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#endif
#define SPI_HMC7044_CS				0x00
#define SPI_AD9172_CS				0x01

#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE                           921600

#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#define TX_JESD_BASEADDR			XPAR_DAC_JESD204_LINK_TX_AXI_BASEADDR
#define TX_XCVR_BASEADDR			XPAR_DAC_JESD204_XCVR_BASEADDR
#define TX_CORE_BASEADDR			XPAR_DAC_JESD204_TRANSPORT_DAC_TPL_CORE_BASEADDR
#define TX_DMA_BASEADDR				XPAR_DAC_DMA_BASEADDR
#ifdef PLATFORM_ZYNQMP
#define DDR_MEM_BASEADDR                        (XPAR_PSU_DDR_0_S_AXI_BASEADDR + 0xA000000)
#else
#define DDR_MEM_BASEADDR 			(XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0xA000000)
#endif

#endif /* __PARAMETERS_H__ */
