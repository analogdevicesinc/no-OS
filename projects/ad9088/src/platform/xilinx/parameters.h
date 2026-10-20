/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to xilinx platform used by ad9088
 *           project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>
#include <xil_cache.h>
#include <xilinx_spi.h>
#include <xilinx_uart.h>
#include <xilinx_gpio.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define UART_DEVICE_ID			XPAR_XUARTPSV_0_DEVICE_ID
#define CLK_SPI_DEVICE_ID		XPAR_XSPIPS_0_DEVICE_ID
#define APOLLO_SPI_DEVICE_ID		XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_GPIO_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#elif defined(PLATFORM_MB)
#define UART_DEVICE_ID			XPAR_AXI_UART_DEVICE_ID
#define CLK_SPI_DEVICE_ID		XPAR_AXI_SPI_DEVICE_ID
#ifndef XPAR_SPI_1_DEVICE_ID
#define XPAR_SPI_1_DEVICE_ID		1
#endif
#define APOLLO_SPI_DEVICE_ID		XPAR_SPI_1_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_AXI_GPIO_DEVICE_ID
#define UART_IRQ_ID			XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#endif

extern struct xil_uart_init_param	platform_uart_extra_ip;
extern struct xil_spi_init_param	clk_spi_extra_ip;
extern struct xil_spi_init_param	apollo_spi_extra_ip;
extern struct xil_gpio_init_param	xil_gpio_param;

#define SPI_OPS_CLK			&xil_spi_ops
#define SPI_EXTRA_CLK			&clk_spi_extra_ip
#define SPI_CS_ADF4382			0
#define SPI_CS_HMC7044			1
#define SPI_CS_ADF4030			4

#define SPI_OPS_APOLLO			&xil_spi_ops
#define SPI_EXTRA_APOLLO		&apollo_spi_extra_ip
#define SPI_CS_APOLLO			0

#define GPIO_OPS			&xil_gpio_ops
#define GPIO_EXTRA			&xil_gpio_param
#define GPIO_OFFSET			32
#define GPIO_RESET_N			30

#define UART_OPS			&xil_uart_ops
#define UART_BAUDRATE			115200
#define UART_EXTRA			&platform_uart_extra_ip

#define RX_DMA_BASEADDR			XPAR_AXI_APOLLO_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_APOLLO_TX_DMA_BASEADDR

/*
 * Data offload IPs. The transmit one is only read, to size a transfer against
 * the actual BRAM; the receive one is reset and armed before every capture.
 */
#define TX_DATA_OFFLOAD_BASEADDR	\
	XPAR_APOLLO_TX_DATA_OFFLOAD_I_DATA_OFFLOAD_BASEADDR
#define RX_DATA_OFFLOAD_BASEADDR	\
	XPAR_APOLLO_RX_DATA_OFFLOAD_I_DATA_OFFLOAD_BASEADDR

/* JESD204 transport layer (TPL) cores */
#define RX_CORE_BASEADDR		\
	XPAR_RX_APOLLO_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR		\
	XPAR_TX_APOLLO_TPL_CORE_DAC_TPL_CORE_BASEADDR

/* Capture depth, in samples per converter */
#define ADC_BUFFER_SAMPLES		16384

#define RX_JESD_BASEADDR		XPAR_AXI_JESD204_RX_0_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_JESD204_TX_0_BASEADDR

#define RX_XCVR_BASEADDR		XPAR_AXI_APOLLO_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_APOLLO_TX_XCVR_BASEADDR

#endif /* __PARAMETERS_H__ */
