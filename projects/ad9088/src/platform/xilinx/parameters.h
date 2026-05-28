/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to xilinx platform used by iio_demo
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#define CLK_SPI_DEVICE_ID		XPAR_XSPIPS_0_DEVICE_ID
#define APOLLO_SPI_DEVICE_ID		XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_GPIO_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#elif defined(PLATFORM_MB)
#define UART_DEVICE_ID			XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID			XPAR_INTC_SINGLE_DEVICE_ID
#define CLK_SPI_DEVICE_ID		XPAR_AXI_SPI_DEVICE_ID
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

#define SPI_OPS_APOLLO			&xil_spi_ops
#define SPI_EXTRA_APOLLO		&apollo_spi_extra_ip
#define SPI_CS_APOLLO			0

#define GPIO_OPS			&xil_gpio_ops
#define GPIO_EXTRA			&xil_gpio_param
#define GPIO_OFFSET			0
#define GPIO_RESET_N			30

#define UART_OPS			&xil_uart_ops
#define UART_BAUDRATE			115200
#define UART_EXTRA			&platform_uart_extra_ip

#define RX_DMA_BASEADDR			XPAR_AXI_APOLLO_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_APOLLO_TX_DMA_BASEADDR

#define RX_JESD_BASEADDR		XPAR_AXI_JESD204_RX_0_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_JESD204_TX_0_BASEADDR

#endif /* __PARAMETERS_H__ */
