/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Application parameters.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef APP_PARAMETERS_H_
#define APP_PARAMETERS_H_

#include <xparameters.h>

#define UART_BAUDRATE 115200

/*This flag is for configuring leader/follower boards in Multi-Chip Sync
 * MXFE_SYNC_LEADER 0 - Follower
 * MXFE_SYNC_LEADER 1 - Leader
 * If not in MCS_CONTINUOUS_SYSREF, it can be ignored */
#define MXFE_SYNC_LEADER 0

#ifdef XPS_BOARD_ZCU102
#define GPIO_OFFSET		78
#define PHY_SYNC		(GPIO_OFFSET + 31)
#if (MXFE_SYNC_LEADER == 1)
#define LF_GPIO_IN              (GPIO_OFFSET + 29)
#define LF_GPIO_OUT              (GPIO_OFFSET + 30)
#else
#define LF_GPIO_IN              (GPIO_OFFSET + 30)
#define LF_GPIO_OUT              (GPIO_OFFSET + 29)
#endif
#else
#ifdef PLATFORM_ZYNQ
#define GPIO_OFFSET		54
#define PHY_SYNC		(GPIO_OFFSET + 12)
#if (MXFE_SYNC_LEADER == 1)
#define LF_GPIO_IN              (GPIO_OFFSET + 11)
#define LF_GPIO_OUT              (GPIO_OFFSET + 13)
#else
#define LF_GPIO_IN              (GPIO_OFFSET + 13)
#define LF_GPIO_OUT              (GPIO_OFFSET + 11)
#endif
#else
#define GPIO_OFFSET		0
#endif
#endif

#define PHY_CS			0

#ifdef QUAD_MXFE
#define ADF4371_CS		0
#define HMC7043_CS		4

#define PHY_RESET		(GPIO_OFFSET + 41)

#define ADRF5020_CTRL_GPIO	(GPIO_OFFSET + 34)
#define MS_SYNC_ENABLE_GPIO	(GPIO_OFFSET + 44)

#define GPIO_2_DEVICE_ID	XPAR_AXI_GPIO_2_DEVICE_ID
#define SPI_2_DEVICE_ID		XPAR_AXI_SPI_2_DEVICE_ID

#else
#define PHY_RESET		(GPIO_OFFSET + 55)
#endif

#if defined(PLATFORM_MB)
#define GPIO_DEVICE_ID		XPAR_AXI_GPIO_DEVICE_ID
#define PHY_SPI_DEVICE_ID	XPAR_AXI_SPI_DEVICE_ID
#define CLK_SPI_DEVICE_ID	XPAR_AXI_SPI_DEVICE_ID
#define UART_DEVICE_ID		XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#define DDR_CNTRL_BASEADDR	XPAR_AXI_DDR_CNTRL_BASEADDR
#define CLK_CS			1
#elif defined(PLATFORM_ZYNQMP)
#define GPIO_DEVICE_ID		XPAR_PSU_GPIO_0_DEVICE_ID
#define PHY_SPI_DEVICE_ID	XPAR_PSU_SPI_0_DEVICE_ID
#define CLK_SPI_DEVICE_ID	XPAR_PSU_SPI_1_DEVICE_ID
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#define DDR_CNTRL_BASEADDR	XPAR_PSU_DDRC_0_BASEADDR
#define CLK_CS			0
#elif defined(PLATFORM_ZYNQ)
#define GPIO_DEVICE_ID		XPAR_PS7_GPIO_0_DEVICE_ID
#define PHY_SPI_DEVICE_ID	XPAR_PS7_SPI_0_DEVICE_ID
#define CLK_SPI_DEVICE_ID	XPAR_PS7_SPI_1_DEVICE_ID
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#define CLK_CS			0
#else
#error Unsupported platform.
#endif

#define RX_JESD_BASEADDR	XPAR_AXI_MXFE_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR	XPAR_AXI_MXFE_TX_JESD_TX_AXI_BASEADDR

#ifdef XPAR_AXI_MXFE_RX_XCVR_BASEADDR
#define RX_XCVR_BASEADDR	XPAR_AXI_MXFE_RX_XCVR_BASEADDR
#endif
#ifdef XPAR_AXI_MXFE_TX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR	XPAR_AXI_MXFE_TX_XCVR_BASEADDR
#endif

#ifdef XPAR_RX_MXFE_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define RX_CORE_BASEADDR	XPAR_RX_MXFE_TPL_CORE_ADC_TPL_CORE_BASEADDR
#else
#define RX_CORE_BASEADDR	XPAR_RX_MXFE_TPL_CORE_TPL_CORE_BASEADDR
#endif
#ifdef XPAR_TX_MXFE_TPL_CORE_DAC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR	XPAR_TX_MXFE_TPL_CORE_DAC_TPL_CORE_BASEADDR
#else
#define TX_CORE_BASEADDR	XPAR_TX_MXFE_TPL_CORE_TPL_CORE_BASEADDR
#endif

#define RX_DMA_BASEADDR		XPAR_AXI_MXFE_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR		XPAR_AXI_MXFE_TX_DMA_BASEADDR

#ifdef IIO_SUPPORT

#define MAX_DAC_BUF_SAMPLES 10000000 //1MB
#define MAX_ADC_BUF_SAMPLES 10000000 //1MB

#endif

#endif
