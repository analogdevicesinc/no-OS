/***************************************************************************//**
 *   @file   parameters.c
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
#define GPIO_OFFSET		0

#define ADRV_RESETB		GPIO_OFFSET + 52
#define ADRV_SYSREF_REQ	GPIO_OFFSET + 58
#define CLK_RESETB		GPIO_OFFSET + 59

#define RX_A10_FPLL_BASEADDR	ADRV9009_RX_JESD204_LINK_PLL_RECONFIG_BASE
#define TX_A10_FPLL_BASEADDR	ADRV9009_TX_JESD204_LINK_PLL_RECONFIG_BASE
#define RX_OS_A10_FPLL_BASEADDR	ADRV9009_RX_OS_JESD204_LINK_PLL_RECONFIG_BASE

#define RX_JESD_BASEADDR		ADRV9009_RX_JESD204_LINK_RECONFIG_BASE
#define TX_JESD_BASEADDR		ADRV9009_TX_JESD204_LINK_RECONFIG_BASE
#define RX_OS_JESD_BASEADDR		ADRV9009_RX_OS_JESD204_LINK_RECONFIG_BASE

#define RX_XCVR_BASEADDR		ADRV9009_RX_JESD204_LINK_MANAGEMENT_BASE
#define TX_XCVR_BASEADDR		ADRV9009_TX_JESD204_LINK_MANAGEMENT_BASE
#define RX_OS_XCVR_BASEADDR		ADRV9009_RX_OS_JESD204_LINK_MANAGEMENT_BASE

#define RX_ADXCFG_0_BASEADDR	AVL_ADXCFG_0_RCFG_S1_BASE
#define RX_ADXCFG_1_BASEADDR	AVL_ADXCFG_1_RCFG_S1_BASE
#define TX_ADXCFG_0_BASEADDR	AVL_ADXCFG_0_RCFG_S0_BASE
#define TX_ADXCFG_1_BASEADDR	AVL_ADXCFG_1_RCFG_S0_BASE
#define TX_ADXCFG_2_BASEADDR	AVL_ADXCFG_2_RCFG_S0_BASE
#define TX_ADXCFG_3_BASEADDR	AVL_ADXCFG_3_RCFG_S0_BASE
#define RX_OS_ADXCFG_0_BASEADDR	AVL_ADXCFG_2_RCFG_S1_BASE
#define RX_OS_ADXCFG_1_BASEADDR	AVL_ADXCFG_3_RCFG_S1_BASE

#define TX_PLL_BASEADDR			ADRV9009_TX_JESD204_LANE_PLL_RECONFIG_BASE

#define RX_CORE_BASEADDR		AXI_ADRV9009_BASE
#define TX_CORE_BASEADDR		AXI_ADRV9009_BASE + 0x4000
#define RX_OS_CORE_BASEADDR		AXI_ADRV9009_BASE + 0x8000

#define RX_DMA_BASEADDR			AXI_ADRV9009_RX_DMA_BASE
#define TX_DMA_BASEADDR			AXI_ADRV9009_TX_DMA_BASE

#define DDR_MEM_BASEADDR		SYS_DDR3_CNTRL_ARCH_BASE

#define GPIO_BASEADDR			SYS_GPIO_OUT_BASE

#define SPI_BASEADDR			SYS_SPI_BASE
#else
#ifdef PLATFORM_ZYNQMP
#define GPIO_DEVICE_ID				XPAR_PSU_GPIO_0_DEVICE_ID
#else
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#endif
#ifdef XPS_BOARD_ZCU102
#define GPIO_OFFSET		78
#else
#define GPIO_OFFSET		54
#endif

#define ADRV_RESETB		GPIO_OFFSET + 52
#define ADRV_SYSREF_REQ	GPIO_OFFSET + 58
#define CLK_RESETB		GPIO_OFFSET + 59
#define DAC_FIFO_BYPASS	GPIO_OFFSET + 60

#define RX_CLKGEN_BASEADDR		XPAR_AXI_ADRV9009_RX_CLKGEN_BASEADDR
#define TX_CLKGEN_BASEADDR		XPAR_AXI_ADRV9009_TX_CLKGEN_BASEADDR
#define RX_OS_CLKGEN_BASEADDR	XPAR_AXI_ADRV9009_RX_OS_CLKGEN_BASEADDR

#define RX_JESD_BASEADDR		XPAR_AXI_ADRV9009_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_ADRV9009_TX_JESD_TX_AXI_BASEADDR
#define RX_OS_JESD_BASEADDR		XPAR_AXI_ADRV9009_RX_OS_JESD_RX_AXI_BASEADDR

#define RX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_ADRV9009_TX_XCVR_BASEADDR
#define RX_OS_XCVR_BASEADDR		XPAR_AXI_ADRV9009_RX_OS_XCVR_BASEADDR

#ifdef XPAR_AXI_ADRV9009_CORE_BASEADDR
#define RX_CORE_BASEADDR		XPAR_AXI_ADRV9009_CORE_BASEADDR
#define TX_CORE_BASEADDR		XPAR_AXI_ADRV9009_CORE_BASEADDR + 0x4000
#define RX_OS_CORE_BASEADDR		XPAR_AXI_ADRV9009_CORE_BASEADDR + 0x8000
#else
#define RX_CORE_BASEADDR		XPAR_RX_ADRV9009_TPL_CORE_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR		XPAR_TX_ADRV9009_TPL_CORE_TPL_CORE_BASEADDR
#define RX_OS_CORE_BASEADDR		XPAR_RX_OS_ADRV9009_TPL_CORE_TPL_CORE_BASEADDR
#endif

#define RX_DMA_BASEADDR			XPAR_AXI_ADRV9009_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_ADRV9009_TX_DMA_BASEADDR

#define DDR_MEM_BASEADDR		XPAR_DDR_MEM_BASEADDR
#endif

#define CLK_CS			0
#define ADRV_CS			1

#endif
