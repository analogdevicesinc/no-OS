/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#define UART_IRQ_ID                 		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#define GPIO_OFFSET				0

#ifdef XPAR_AXI_DDR_CNTRL_BASEADDR
#define ADC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_BASEADDR + 0x900000)
#else
#define ADC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR			(XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR + 0x900000)
#endif
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

#define RX_CORE_BASEADDR			XPAR_AXI_AD9680_TPL_ADC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR			XPAR_AXI_AD9144_TPL_DAC_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR				XPAR_AXI_AD9680_DMA_BASEADDR
#define TX_DMA_BASEADDR				XPAR_AXI_AD9144_DMA_BASEADDR

#define RX_JESD_BASEADDR			XPAR_AXI_AD9680_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR			XPAR_AXI_AD9144_JESD_TX_AXI_BASEADDR

#define RX_XCVR_BASEADDR			XPAR_AXI_AD9680_XCVR_BASEADDR
#define TX_XCVR_BASEADDR			XPAR_AXI_AD9144_XCVR_BASEADDR
#else
#define SPI_DEVICE_ID				0
#define GPIO_DEVICE_ID				0

#define GPIO_OFFSET				0

#define SPI_BASEADDR				SYS_SPI_BASE
#define GPIO_BASEADDR				SYS_GPIO_OUT_BASE

#define ADC_DDR_BASEADDR			(SYS_DDR3_CNTRL_ARCH_BASE + 0x800000)
#define DAC_DDR_BASEADDR			(SYS_DDR3_CNTRL_ARCH_BASE + 0x900000)

#define RX_CORE_BASEADDR			AXI_AD9680_CORE_BASE
#define TX_CORE_BASEADDR			AXI_AD9144_CORE_BASE + 0x4000

#define RX_DMA_BASEADDR				AXI_AD9680_DMA_BASE
#define TX_DMA_BASEADDR				AXI_AD9144_DMA_BASE

#define RX_JESD_BASEADDR			AD9680_JESD204_LINK_RECONFIG_BASE
#define TX_JESD_BASEADDR			AD9144_JESD204_LINK_RECONFIG_BASE

#define RX_XCVR_BASEADDR			AD9680_JESD204_LINK_MANAGEMENT_BASE
#define TX_XCVR_BASEADDR			AD9144_JESD204_LINK_MANAGEMENT_BASE

#define RX_A10_FPLL_BASEADDR			AD9680_JESD204_LINK_PLL_RECONFIG_BASE
#define TX_A10_FPLL_BASEADDR			AD9144_JESD204_LINK_PLL_RECONFIG_BASE

#define TX_PLL_BASEADDR				AD9144_JESD204_LANE_PLL_RECONFIG_BASE
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

#define GPIO_TRIG				(GPIO_OFFSET + 43)
#define GPIO_ADC_PD				(GPIO_OFFSET + 42)
#define GPIO_DAC_TXEN				(GPIO_OFFSET + 41)
#define GPIO_DAC_RESET				(GPIO_OFFSET + 40)
#define GPIO_CLKD_SYNC				(GPIO_OFFSET + 38)
#define GPIO_ADC_FDB				(GPIO_OFFSET + 36)
#define GPIO_ADC_FDA				(GPIO_OFFSET + 35)
#define GPIO_DAC_IRQ				(GPIO_OFFSET + 34)
#define GPIO_CLKD_STATUS_1			(GPIO_OFFSET + 33)
#define GPIO_CLKD_STATUS_0			(GPIO_OFFSET + 32)

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
