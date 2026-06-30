/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters for K project (Xilinx).
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "xparameters.h"
#include "xilinx_uart.h"
#include "xilinx_gpio.h"
#include "xilinx_i2c.h"
#include "app_config.h"

/******************************************************************************/
/*****  GPIO/I2C device IDs (from Xilinx BSP)                              ***/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#ifdef XPS_BOARD_ZCU102
#define GPIO_OFFSET			78
#define GPIO_DEVICE_ID			XPAR_PSU_GPIO_0_DEVICE_ID
/*
 * Vitis 2025+ SDT BSPs no longer emit XPAR_PSU_I2C_1_DEVICE_ID; the PS I2C
 * driver is looked up by base address in SDT mode. Si5391 is on I2C1.
 */
#define I2C_DEVICE_ID			1
#define I2C_BASEADDR			XPAR_XIICPS_1_BASEADDR
#else
#define GPIO_OFFSET			54
#endif
#define DDR_MEM_BASEADDR		XPAR_DDR_MEM_BASEADDR
#else
#define GPIO_OFFSET			0
#define GPIO_DEVICE_ID			XPAR_GPIO_0_DEVICE_ID
#define I2C_DEVICE_ID			XPAR_AXI_IIC_0_DEVICE_ID
#define DDR_MEM_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR
#endif

/******************************************************************************/
/***  AXI IP base addresses                                                  ***/
/******************************************************************************/
#define RX_JESD_BASEADDR		XPAR_AXI_JESD204_RX_0_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_JESD204_TX_0_BASEADDR

#define RX_XCVR_BASEADDR		XPAR_AXI_RX_ADXCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_TX_ADXCVR_BASEADDR

#define RX_CORE_BASEADDR		XPAR_RX_ANALYZER_TPL_RX_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR		XPAR_TX_GENERATOR_TPL_TX_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR			XPAR_AXI_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR			XPAR_AXI_TX_DMA_BASEADDR
#define RX_CMD_DMA_BASEADDR		XPAR_AXI_RX_CMD_DMA_BASEADDR
#define TX_CMD_DMA_BASEADDR		XPAR_AXI_TX_CMD_DMA_BASEADDR

#define TX_DATA_OFFLOAD_BASEADDR	XPAR_TX_DATA_OFFLOAD_I_DATA_OFFLOAD_BASEADDR
#define RX_DATA_OFFLOAD_BASEADDR	XPAR_RX_DATA_OFFLOAD_I_DATA_OFFLOAD_BASEADDR

/******************************************************************************/
/***  DMA buffer addresses in low DDR (must fit in 32-bit axi_dma_transfer) ***/
/******************************************************************************/
#define TX_DDR_BASEADDR			(DDR_MEM_BASEADDR + 0x1000000)
#define TX_CMD_DDR_BASEADDR		(DDR_MEM_BASEADDR + 0x1100000)
#define RX_DDR_BASEADDR			(DDR_MEM_BASEADDR + 0x1200000)
#define RX_CMD_DDR_BASEADDR		(DDR_MEM_BASEADDR + 0x1300000)
#define ADC_DDR_BASEADDR		(DDR_MEM_BASEADDR + 0x1400000)

/******************************************************************************/
/***  Buffer sizes                                                          ***/
/******************************************************************************/
#define DAC_BUFFER_SAMPLES		65536
#define ADC_BUFFER_SAMPLES		32768
#define ADC_CHANNELS			8

/******************************************************************************/
/***  GPIO offsets                                                          ***/
/******************************************************************************/
#define AD9081K_RESET_B			(GPIO_OFFSET + 55)	/* GPIO 133 = rstb */

/******************************************************************************/
/***  UART                                                                  ***/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#else
#define UART_DEVICE_ID			XPAR_AXI_UART_DEVICE_ID
#ifdef XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#define UART_IRQ_ID			XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#else
#define UART_IRQ_ID			0
#endif
#endif

#define UART_EXTRA		&xilinx_lwip_uart_extra_ip
#define UART_OPS		&xil_uart_ops

/******************************************************************************/
/***  GEM instance selection                                                ***/
/******************************************************************************/
#ifndef GEM_INSTANCE
#define GEM_INSTANCE	0
#endif

/*
 * The XEmacPs driver is looked up by an "identifier" which is a device ID on
 * legacy BSPs and a base address on Vitis 2025+ SDT BSPs (-DSDT). SDT BSPs no
 * longer emit XPAR_XEMACPS_n_DEVICE_ID, so prefer the base address there and
 * fall back to the device ID on older BSPs.
 */
#if defined(SDT)
#if GEM_INSTANCE == 0 && defined(XPAR_XEMACPS_0_BASEADDR)
#define GEM_DEVICE_ID	XPAR_XEMACPS_0_BASEADDR
#elif GEM_INSTANCE == 1 && defined(XPAR_XEMACPS_1_BASEADDR)
#define GEM_DEVICE_ID	XPAR_XEMACPS_1_BASEADDR
#elif GEM_INSTANCE == 2 && defined(XPAR_XEMACPS_2_BASEADDR)
#define GEM_DEVICE_ID	XPAR_XEMACPS_2_BASEADDR
#elif GEM_INSTANCE == 3 && defined(XPAR_XEMACPS_3_BASEADDR)
#define GEM_DEVICE_ID	XPAR_XEMACPS_3_BASEADDR
#else
#error "GEM_INSTANCE base address not available - check your .xsa / BSP configuration"
#endif
#else /* legacy (non-SDT) BSP */
#if GEM_INSTANCE == 0 && defined(XPAR_XEMACPS_0_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#elif GEM_INSTANCE == 1 && defined(XPAR_XEMACPS_1_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_1_DEVICE_ID
#elif GEM_INSTANCE == 2 && defined(XPAR_XEMACPS_2_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_2_DEVICE_ID
#elif GEM_INSTANCE == 3 && defined(XPAR_XEMACPS_3_DEVICE_ID)
#define GEM_DEVICE_ID	XPAR_XEMACPS_3_DEVICE_ID
#else
#error "GEM_INSTANCE not available - check your .xsa / BSP configuration"
#endif
#endif

extern struct xil_i2c_init_param i2c_extra;
extern struct xil_gpio_init_param xil_gpio_param;
extern struct xil_uart_init_param xilinx_lwip_uart_extra_ip;

#endif /* _PARAMETERS_H_ */
