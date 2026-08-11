/***************************************************************************//**
 *   @file   adrv902x/src/platform/xilinx/parameters.h
 *   @brief  Platform dependent parameters.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "xparameters.h"
#include "xilinx_gpio.h"
#include "app_config.h"
#include "xilinx_spi.h"


#ifdef _XPARAMETERS_PS_H_
#ifdef XPS_BOARD_ZCU102
#define GPIO_OFFSET			78

#define GPIO_DEVICE_ID			XPAR_PSU_GPIO_0_DEVICE_ID
#define SPI_DEVICE_ID			XPAR_PSU_SPI_0_DEVICE_ID
#define UART_DEVICE_ID			XPAR_PSU_UART_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#else
#define GPIO_OFFSET			54
#endif

#define DDR_MEM_BASEADDR		XPAR_DDR_MEM_BASEADDR
#else
#define GPIO_OFFSET			0

#define GPIO_DEVICE_ID			XPAR_GPIO_0_DEVICE_ID
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define UART_DEVICE_ID			XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID			XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#define DDR_MEM_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR
#endif

#define RX_JESD_BASEADDR		XPAR_AXI_JESD204_RX_0_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_JESD204_TX_0_BASEADDR
#ifdef XPAR_AXI_JESD204_RX_1_BASEADDR
#define ORX_JESD_BASEADDR		XPAR_AXI_JESD204_RX_1_BASEADDR
#endif

#define RX_XCVR_BASEADDR		XPAR_AXI_ADRV9026_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_ADRV9026_TX_XCVR_BASEADDR
#ifdef XPAR_AXI_ADRV9026_RX_OS_XCVR_BASEADDR
#define ORX_XCVR_BASEADDR		XPAR_AXI_ADRV9026_RX_OS_XCVR_BASEADDR
#endif

#define RX_CORE_BASEADDR		XPAR_AD_IP_JESD204_TPL_ADC_0_BASEADDR
#define TX_CORE_BASEADDR		XPAR_AD_IP_JESD204_TPL_DAC_0_BASEADDR
#ifdef XPAR_AD_IP_JESD204_TPL_ADC_1_BASEADDR
#define ORX_CORE_BASEADDR		XPAR_AD_IP_JESD204_TPL_ADC_1_BASEADDR
#endif

#define RX_DMA_BASEADDR		        XPAR_AXI_ADRV9026_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR		        XPAR_AXI_ADRV9026_TX_DMA_BASEADDR
#ifdef XPAR_AXI_ADRV9026_RX_OS_DMA_BASEADDR
#define ORX_DMA_BASEADDR		        XPAR_AXI_ADRV9026_RX_OS_DMA_BASEADDR
#endif

#define RX_CLKGEN_BASEADDR		XPAR_AXI_ADRV9026_RX_CLKGEN_BASEADDR
#define TX_CLKGEN_BASEADDR		XPAR_AXI_ADRV9026_TX_CLKGEN_BASEADDR
#ifdef XPAR_AXI_ADRV9026_RX_OS_CLKGEN_BASEADDR
#define ORX_CLKGEN_BASEADDR		XPAR_AXI_ADRV9026_RX_OS_CLKGEN_BASEADDR
#endif

#define DAC_BUFFER_SAMPLES              8192
#define ADC_BUFFER_SAMPLES              16384

#define AD9528_CS			1
#define ADRV9025_CS			0

#ifdef XPS_BOARD_ZCU102
#define DAC_GPIO_PLDDR_BYPASS          (GPIO_OFFSET + 69)
#define AD9528_RESET_B                 (GPIO_OFFSET + 68)
#define AD9528_SYSREF_REQ              (GPIO_OFFSET + 58)
#define ADRV9025_RESET_B               (GPIO_OFFSET + 57)
#else
#define DAC_GPIO_PLDDR_BYPASS	        (GPIO_OFFSET + 63)
#define AD9528_RESET_B			(GPIO_OFFSET + 62)
#define AD9528_SYSREF_REQ		(GPIO_OFFSET + 61)
#define ADRV9025_RESET_B		(GPIO_OFFSET + 51)
#endif

extern struct xil_spi_init_param spi_extra;
extern struct xil_gpio_init_param xil_gpio_param;

#endif
