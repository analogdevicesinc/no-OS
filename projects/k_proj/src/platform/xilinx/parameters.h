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
#define I2C_DEVICE_ID			XPAR_PSU_I2C_1_DEVICE_ID
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

extern struct xil_i2c_init_param i2c_extra;
extern struct xil_gpio_init_param xil_gpio_param;

#endif /* _PARAMETERS_H_ */
