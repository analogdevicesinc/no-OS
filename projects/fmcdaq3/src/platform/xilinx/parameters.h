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

#include <xparameters.h>
#include <xil_cache.h>
#include <xilinx_spi.h>
#include <xilinx_gpio.h>
#include <xil_printf.h>
#include "axi_adxcvr.h"

#ifdef IIO_SUPPORT
#include "xilinx_uart.h"
#endif

#define UART_BAUDRATE                           115200
#ifdef PLATFORM_MB
#define SPI_DEVICE_ID				XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_GPIO_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_AXI_UART_DEVICE_ID

#define GPIO_OFFSET				0

#define ADC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_BASEADDR + 0x900000)
#else
#define SPI_DEVICE_ID				XPAR_XSPIPS_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_XGPIOPS_0_DEVICE_ID
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#endif

#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#ifdef PLATFORM_ZYNQMP
#define GPIO_OFFSET				78
#else
#define GPIO_OFFSET				54
#endif

#define ADC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR + 0x900000)
#endif

#define RX_CORE_BASEADDR			XPAR_AXI_AD9680_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR			XPAR_AXI_AD9152_TPL_CORE_DAC_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR				XPAR_AXI_AD9680_DMA_BASEADDR
#define TX_DMA_BASEADDR				XPAR_AXI_AD9152_DMA_BASEADDR

#define RX_JESD_BASEADDR			XPAR_AXI_AD9680_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR			XPAR_AXI_AD9152_JESD_TX_AXI_BASEADDR

#define RX_XCVR_BASEADDR			XPAR_AXI_AD9680_XCVR_BASEADDR
#define TX_XCVR_BASEADDR			XPAR_AXI_AD9152_XCVR_BASEADDR

#define GPIO_CLKD_STATUS_0      		(GPIO_OFFSET + 32)
#define GPIO_CLKD_STATUS_1      		(GPIO_OFFSET + 33)
#define GPIO_DAC_IRQ           			(GPIO_OFFSET + 34)
#define GPIO_ADC_FDA            		(GPIO_OFFSET + 35)
#define GPIO_ADC_FDB            		(GPIO_OFFSET + 36)
#define GPIO_DAC_TXEN           		(GPIO_OFFSET + 37)
#define GPIO_ADC_PD             		(GPIO_OFFSET + 38)
#define GPIO_TRIG               		(GPIO_OFFSET + 39)

extern struct xil_uart_init_param platform_uart_init_par;
extern struct xil_spi_init_param xil_spi_param;
extern struct xil_gpio_init_param xil_gpio_param ;

#define SPI_OPS					&xil_spi_ops
#define SPI_EXTRA				&xil_spi_param
#define UART_OPS				&xil_uart_ops
#define UART_EXTRA				&platform_uart_init_par
#define GPIO_OPS				&xil_gpio_ops
#define GPIO_EXTRA				&xil_gpio_param

enum ad9528_channels {
	DAC_DEVICE_CLK,
	DAC_DEVICE_SYSREF,
	DAC_FPGA_CLK,
	DAC_FPGA_SYSREF,
	ADC_DEVICE_CLK,
	ADC_DEVICE_SYSREF,
	ADC_FPGA_CLK,
	ADC_FPGA_SYSREF,
};

extern struct adxcvr_init ad9152_xcvr_init_param;
extern struct adxcvr_init ad9680_xcvr_init_param;

#endif /* __PARAMETERS_H__ */
