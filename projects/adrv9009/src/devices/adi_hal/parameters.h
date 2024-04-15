/***************************************************************************//**
 *   @file   adrv9009/src/devices/adi_hal/parameters.h
 *   @brief  Platform dependent parameters.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "app_config.h"
#ifdef ALTERA_PLATFORM
#include "system.h"
#else
#include "xparameters.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef ALTERA_PLATFORM
#define GPIO_OFFSET			0

#define ADRV_RESETB			GPIO_OFFSET + 52
#define ADRV_SYSREF_REQ			GPIO_OFFSET + 58
#define CLK_RESETB			GPIO_OFFSET + 59

#define RX_A10_FPLL_BASEADDR		ADRV9009_RX_JESD204_LINK_PLL_RECONFIG_BASE
#define TX_A10_FPLL_BASEADDR		ADRV9009_TX_JESD204_LINK_PLL_RECONFIG_BASE
#define RX_OS_A10_FPLL_BASEADDR		ADRV9009_RX_OS_JESD204_LINK_PLL_RECONFIG_BASE

#define RX_JESD_BASEADDR		ADRV9009_RX_JESD204_LINK_RECONFIG_BASE
#define TX_JESD_BASEADDR		ADRV9009_TX_JESD204_LINK_RECONFIG_BASE
#define RX_OS_JESD_BASEADDR		ADRV9009_RX_OS_JESD204_LINK_RECONFIG_BASE

#define RX_XCVR_BASEADDR		ADRV9009_RX_JESD204_LINK_MANAGEMENT_BASE
#define TX_XCVR_BASEADDR		ADRV9009_TX_JESD204_LINK_MANAGEMENT_BASE
#define RX_OS_XCVR_BASEADDR		ADRV9009_RX_OS_JESD204_LINK_MANAGEMENT_BASE

#define RX_ADXCFG_0_BASEADDR		AVL_ADXCFG_0_RCFG_S1_BASE
#define RX_ADXCFG_1_BASEADDR		AVL_ADXCFG_1_RCFG_S1_BASE
#define TX_ADXCFG_0_BASEADDR		AVL_ADXCFG_0_RCFG_S0_BASE
#define TX_ADXCFG_1_BASEADDR		AVL_ADXCFG_1_RCFG_S0_BASE
#define TX_ADXCFG_2_BASEADDR		AVL_ADXCFG_2_RCFG_S0_BASE
#define TX_ADXCFG_3_BASEADDR		AVL_ADXCFG_3_RCFG_S0_BASE
#define RX_OS_ADXCFG_0_BASEADDR		AVL_ADXCFG_2_RCFG_S1_BASE
#define RX_OS_ADXCFG_1_BASEADDR		AVL_ADXCFG_3_RCFG_S1_BASE

#define TX_PLL_BASEADDR			ADRV9009_TX_JESD204_LANE_PLL_RECONFIG_BASE

#define RX_CORE_BASEADDR		AXI_ADRV9009_BASE
#define TX_CORE_BASEADDR		AXI_ADRV9009_BASE + 0x4000
#define RX_OS_CORE_BASEADDR		AXI_ADRV9009_BASE + 0x8000

#define RX_DMA_BASEADDR			AXI_ADRV9009_RX_DMA_BASE
#define TX_DMA_BASEADDR			AXI_ADRV9009_TX_DMA_BASE

#define DDR_MEM_BASEADDR		SYS_DDR3_CNTRL_ARCH_BASE
#define ADC_DDR_BASEADDR		SYS_DDR3_CNTRL_ARCH_BASE + 0x800000
#define DAC_DDR_BASEADDR		SYS_DDR3_CNTRL_ARCH_BASE + 0xA000000

#define GPIO_BASEADDR			SYS_GPIO_OUT_BASE

#define SPI_BASEADDR			SYS_SPI_BASE
#else
#ifdef PLATFORM_ZYNQMP
#define GPIO_DEVICE_ID			XPAR_PSU_GPIO_0_DEVICE_ID
#else
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#endif

#if defined(ZU11EG) // ZU11EG
#define ADRV_CS				0 // Talise A
#define ADRV_B_CS			1 // Talise B
#define CLK_CS				2 // Clock chip placed on the som
#define CAR_CLK_CS			3 // Clock chip placed on the carrier

// Transceiver A
#define TRX_A_RESETB_GPIO		130
#define TRX_A_TEST_GPIO			131
#define TRX_A_RX1_ENABLE_GPIO		132
#define TRX_A_RX2_ENABLE_GPIO		133
#define TRX_A_TX1_ENABLE_GPIO		134
#define TRX_A_TX2_ENABLE_GPIO		135

// Transceiver B
#define TRX_B_RESETB_GPIO		156
#define TRX_B_TEST_GPIO 		157
#define TRX_B_RX1_ENABLE_GPIO		158
#define TRX_B_RX2_ENABLE_GPIO		159
#define TRX_B_TX1_ENABLE_GPIO		160
#define TRX_B_TX2_ENABLE_GPIO		161

#define SYSREF_REQ_GPIO			167
#define CLK_RESETB_GPIO			162
#define DAC_FIFO_BYPASS_GPIO		168
#define CAR_CLK_RESETB_GPIO		101

#elif defined(FMCOMMS8_ZCU102)

#define ADRV_CS				0 // Talise A
#define ADRV_B_CS			1 // Talise B
#define CLK_CS				2 // Clock chip placed on the som
#define CAR_CLK_CS			3 // Clock chip placed on the carrier

// Transceiver A
#define TRX_A_RESETB_GPIO		120
#define TRX_A_TEST_GPIO			131
#define TRX_A_RX1_ENABLE_GPIO		121
#define TRX_A_RX2_ENABLE_GPIO		122
#define TRX_A_TX1_ENABLE_GPIO		123
#define TRX_A_TX2_ENABLE_GPIO		124

// Transceiver B
#define TRX_B_RESETB_GPIO		135
#define TRX_B_TEST_GPIO 		157
#define TRX_B_RX1_ENABLE_GPIO		136
#define TRX_B_RX2_ENABLE_GPIO		137
#define TRX_B_TX1_ENABLE_GPIO		138
#define TRX_B_TX2_ENABLE_GPIO		139

#define SYSREF_REQ_GPIO			167
#define CLK_RESETB_GPIO			162
#define DAC_FIFO_BYPASS_GPIO		168
#define CAR_CLK_RESETB_GPIO		101

#else

#define CLK_CS				0
#define ADRV_CS				1

#ifdef XPS_BOARD_ZCU102 // ZCU102
#define TRX_A_RESETB_GPIO		130
#define SYSREF_REQ_GPIO			136
#define CLK_RESETB_GPIO			137
#define DAC_FIFO_BYPASS_GPIO		138
#else // ZC706
#define TRX_A_RESETB_GPIO		106
#define SYSREF_REQ_GPIO			112
#define CLK_RESETB_GPIO			113
#define DAC_FIFO_BYPASS_GPIO		114
#endif
#endif

#if defined(ZU11EG)
// axi_clkgen is not used with the adrv9009 som.
#elif defined(FMCOMMS8_ZCU102)
// axi_clkgen is not used with the fmcomms8.
#else
#define RX_CLKGEN_BASEADDR		XPAR_AXI_ADRV9009_RX_CLKGEN_BASEADDR
#define TX_CLKGEN_BASEADDR		XPAR_AXI_ADRV9009_TX_CLKGEN_BASEADDR
#define RX_OS_CLKGEN_BASEADDR		XPAR_AXI_ADRV9009_RX_OS_CLKGEN_BASEADDR
#endif

#if defined(ZU11EG)
#define RX_JESD_BASEADDR		XPAR_AXI_ADRV9009_SOM_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_ADRV9009_SOM_TX_JESD_TX_AXI_BASEADDR
#define RX_OS_JESD_BASEADDR		XPAR_AXI_ADRV9009_SOM_OBS_JESD_RX_AXI_BASEADDR
#elif defined(FMCOMMS8_ZCU102)
#define RX_JESD_BASEADDR		XPAR_AXI_ADRV9009_FMC_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_ADRV9009_FMC_TX_JESD_TX_AXI_BASEADDR
#define RX_OS_JESD_BASEADDR		XPAR_AXI_ADRV9009_FMC_OBS_JESD_RX_AXI_BASEADDR
#else
#define RX_JESD_BASEADDR		XPAR_AXI_ADRV9009_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_ADRV9009_TX_JESD_TX_AXI_BASEADDR
#define RX_OS_JESD_BASEADDR		XPAR_AXI_ADRV9009_RX_OS_JESD_RX_AXI_BASEADDR
#endif

#if defined(ZU11EG)
#define RX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_SOM_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_SOM_TX_XCVR_BASEADDR
#define RX_OS_XCVR_BASEADDR		XPAR_AXI_ADRV9009_SOM_OBS_XCVR_BASEADDR
#elif defined(FMCOMMS8_ZCU102)
#define RX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_FMC_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_FMC_TX_XCVR_BASEADDR
#define RX_OS_XCVR_BASEADDR		XPAR_AXI_ADRV9009_FMC_OBS_XCVR_BASEADDR
#else
#define RX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_TX_XCVR_BASEADDR
#define RX_OS_XCVR_BASEADDR		XPAR_AXI_ADRV9009_RX_OS_XCVR_BASEADDR
#endif

#ifdef XPAR_AXI_ADRV9009_CORE_BASEADDR
#define RX_CORE_BASEADDR		XPAR_AXI_ADRV9009_CORE_BASEADDR
#define TX_CORE_BASEADDR		XPAR_AXI_ADRV9009_CORE_BASEADDR + 0x4000
#define RX_OS_CORE_BASEADDR		XPAR_AXI_ADRV9009_CORE_BASEADDR + 0x8000
#else
#if defined(ZU11EG)
#define RX_CORE_BASEADDR		XPAR_RX_ADRV9009_SOM_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR		XPAR_TX_ADRV9009_SOM_TPL_CORE_DAC_TPL_CORE_BASEADDR
#define RX_OS_CORE_BASEADDR		XPAR_OBS_ADRV9009_SOM_TPL_CORE_ADC_TPL_CORE_BASEADDR
#elif defined(FMCOMMS8_ZCU102)
#define RX_CORE_BASEADDR		XPAR_RX_ADRV9009_FMC_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR		XPAR_TX_ADRV9009_FMC_TPL_CORE_DAC_TPL_CORE_BASEADDR
#define RX_OS_CORE_BASEADDR		XPAR_OBS_ADRV9009_FMC_TPL_CORE_ADC_TPL_CORE_BASEADDR
#else
#define RX_CORE_BASEADDR		XPAR_RX_ADRV9009_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR		XPAR_TX_ADRV9009_TPL_CORE_DAC_TPL_CORE_BASEADDR
#define RX_OS_CORE_BASEADDR		XPAR_RX_OS_ADRV9009_TPL_CORE_ADC_TPL_CORE_BASEADDR
#endif // #if defined(ZU11EG)
#endif // #ifdef XPAR_AXI_ADRV9009_CORE_BASEADDR

#if defined(ZU11EG)
#define RX_DMA_BASEADDR			XPAR_AXI_ADRV9009_SOM_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_ADRV9009_SOM_TX_DMA_BASEADDR
#define RX_OS_DMA_BASEADDR			XPAR_AXI_ADRV9009_SOM_OBS_DMA_BASEADDR
#elif defined(FMCOMMS8_ZCU102)
#define RX_DMA_BASEADDR			XPAR_AXI_ADRV9009_FMC_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_ADRV9009_FMC_TX_DMA_BASEADDR
#else
#define RX_DMA_BASEADDR			XPAR_AXI_ADRV9009_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_ADRV9009_TX_DMA_BASEADDR
#define RX_OS_DMA_BASEADDR		XPAR_AXI_ADRV9009_RX_OS_DMA_BASEADDR
#endif

#define DDR_MEM_BASEADDR		XPAR_DDR_MEM_BASEADDR
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000
#define DAC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0xA000000

#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_BAUDRATE                   921600
#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#endif
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif

#endif

