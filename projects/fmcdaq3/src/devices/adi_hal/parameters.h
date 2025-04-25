/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "app_config.h"
#ifdef ALTERA_PLATFORM
#include "system.h"
#else
#include "xparameters.h"
#endif

#define UART_BAUDRATE                           115200
#ifndef ALTERA_PLATFORM
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
#endif

#define RX_CORE_BASEADDR			XPAR_AXI_AD9680_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR			XPAR_AXI_AD9152_TPL_CORE_DAC_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR				XPAR_AXI_AD9680_DMA_BASEADDR
#define TX_DMA_BASEADDR				XPAR_AXI_AD9152_DMA_BASEADDR

#define RX_JESD_BASEADDR			XPAR_AXI_AD9680_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR			XPAR_AXI_AD9152_JESD_TX_AXI_BASEADDR

#define RX_XCVR_BASEADDR			XPAR_AXI_AD9680_XCVR_BASEADDR
#define TX_XCVR_BASEADDR			XPAR_AXI_AD9152_XCVR_BASEADDR
#else
#define SPI_DEVICE_ID				0
#define GPIO_DEVICE_ID				0

#define GPIO_OFFSET				0

#define SPI_BASEADDR				SYS_SPI_BASE
#define GPIO_BASEADDR				SYS_GPIO_OUT_BASE

#define RX_CORE_BASEADDR			AXI_AD9680_CORE_BASE
#define TX_CORE_BASEADDR			AXI_AD9152_CORE_BASE

#define RX_JESD_BASEADDR			AD9680_JESD204_LINK_RECONFIG_BASE
#define TX_JESD_BASEADDR			AD9152_JESD204_LINK_RECONFIG_BASE

#define RX_XCVR_BASEADDR			AD9680_JESD204_LINK_MANAGEMENT_BASE
#define TX_XCVR_BASEADDR			AD9152_JESD204_LINK_MANAGEMENT_BASE

#define RX_A10_FPLL_BASEADDR			AD9680_JESD204_LINK_PLL_RECONFIG_BASE
#define TX_A10_FPLL_BASEADDR			AD9152_JESD204_LINK_PLL_RECONFIG_BASE

#define TX_PLL_BASEADDR				AD9152_JESD204_LANE_PLL_RECONFIG_BASE
#define RX_PLL_BASEADDR				AD9680_JESD204_LINK_PLL_RECONFIG_BASE

#define RX_ADXCFG_0_BASEADDR			AVL_ADXCFG_0_RCFG_S1_BASE
#define RX_ADXCFG_1_BASEADDR			AVL_ADXCFG_1_RCFG_S1_BASE
#define RX_ADXCFG_2_BASEADDR			AVL_ADXCFG_2_RCFG_S1_BASE
#define RX_ADXCFG_3_BASEADDR			AVL_ADXCFG_3_RCFG_S1_BASE
#define TX_ADXCFG_0_BASEADDR			AVL_ADXCFG_0_RCFG_S0_BASE
#define TX_ADXCFG_1_BASEADDR			AVL_ADXCFG_1_RCFG_S0_BASE
#define TX_ADXCFG_2_BASEADDR			AVL_ADXCFG_2_RCFG_S0_BASE
#define TX_ADXCFG_3_BASEADDR			AVL_ADXCFG_3_RCFG_S0_BASE
#endif

#define BUFFER_SAMPLES				32768

#define GPIO_CLKD_STATUS_0      (GPIO_OFFSET + 32)
#define GPIO_CLKD_STATUS_1      (GPIO_OFFSET + 33)
#define GPIO_DAC_IRQ           	(GPIO_OFFSET + 34)
#define GPIO_ADC_FDA            (GPIO_OFFSET + 35)
#define GPIO_ADC_FDB            (GPIO_OFFSET + 36)
#define GPIO_DAC_TXEN           (GPIO_OFFSET + 37)
#define GPIO_ADC_PD             (GPIO_OFFSET + 38)
#define GPIO_TRIG               (GPIO_OFFSET + 39)

enum ad9523_channels {
	DAC_DEVICE_CLK,
	DAC_DEVICE_SYSREF,
	DAC_FPGA_CLK,
	DAC_FPGA_SYSREF,
	ADC_DEVICE_CLK,
	ADC_DEVICE_SYSREF,
	ADC_FPGA_CLK,
	ADC_FPGA_SYSREF,
};

#endif /* _PARAMETERS_H_ */
