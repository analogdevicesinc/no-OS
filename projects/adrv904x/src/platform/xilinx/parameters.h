/***************************************************************************//**
 *   @file   adrv904x/src/platform/xilinx/parameters.h
 *   @brief  Platform dependent parameters.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xparameters.h"
#include "xilinx_gpio.h"
#include "app_config.h"
#include "xilinx_spi.h"


/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
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
#define UART_IRQ_ID			XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#define DDR_MEM_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR
#endif

#define RX_JESD_BASEADDR		XPAR_AXI_JESD204_RX_0_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_JESD204_TX_0_BASEADDR
//#define RX_OS_JESD_BASEADDR		XPAR_AXI_AD9371_RX_OS_JESD_RX_AXI_BASEADDR

#define RX_XCVR_BASEADDR		XPAR_AXI_ADRV904X_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_ADRV904X_TX_XCVR_BASEADDR
//#define RX_OS_XCVR_BASEADDR		XPAR_AXI_AD9371_RX_OS_XCVR_BASEADDR

#define RX_CORE_BASEADDR		XPAR_AD_IP_JESD204_TPL_ADC_0_BASEADDR
#define TX_CORE_BASEADDR		XPAR_AD_IP_JESD204_TPL_DAC_0_BASEADDR
//#define RX_OS_CORE_BASEADDR		XPAR_RX_OS_AD9371_TPL_CORE_ADC_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR			XPAR_AXI_ADRV904X_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_ADRV904X_TX_DMA_BASEADDR


#define RX_CLKGEN_BASEADDR		XPAR_AXI_ADRV904X_RX_CLKGEN_BASEADDR
#define TX_CLKGEN_BASEADDR		XPAR_AXI_ADRV904X_TX_CLKGEN_BASEADDR
#ifdef XPAR_AXI_ADRV904X_RX_OS_CLKGEN_BASEADDR
#define ORX_CLKGEN_BASEADDR		XPAR_AXI_ADRV904X_RX_OS_CLKGEN_BASEADDR
#endif

#define DAC_BUFFER_SAMPLES              8192
#define ADC_BUFFER_SAMPLES              32768
#define ADC_CHANNELS                    8

#define AD9528_CS			1
#define ADRV9040_CS			0

#define DAC_GPIO_PLDDR_BYPASS	        (GPIO_OFFSET + 70)
#define AD9528_RESET_B			(GPIO_OFFSET + 69)
#define AD9528_SYSREF_REQ		(GPIO_OFFSET + 68)
#define ADRV9040_RESET_B		(GPIO_OFFSET + 56)

extern struct xil_spi_init_param spi_extra;
extern struct xil_gpio_init_param xil_gpio_param;

#endif
