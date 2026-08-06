/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters for ADRV903X project (Xilinx).
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders. This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
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
#include "app_config.h"
#include "xilinx_spi.h"

/******************************************************************************/
/*****  GPIO/SPI identifiers                                               ***/
/******************************************************************************/
#ifdef PLATFORM_VERSAL
  /* Versal Tetra: PL GPIO via AXI GPIO, PS SPI1 */
  #define GPIO_OFFSET			0
  #define GPIO_DEVICE_ID		XPAR_XGPIO_0_BASEADDR
  #define SPI_DEVICE_ID			1
  #define UART_DEVICE_ID		XPAR_XUARTPSV_0_BASEADDR
  #define UART_IRQ_ID			XPAR_PSV_SBSAUART_1_INTR
  #define DDR_MEM_BASEADDR		0x0
#elif defined(_XPARAMETERS_PS_H_)
  #ifdef XPS_BOARD_ZCU102
    /* ZCU102: PS GPIO, PS SPI0 */
    #define GPIO_OFFSET			78
    #define GPIO_DEVICE_ID		XPAR_PSU_GPIO_0_DEVICE_ID
    #define SPI_DEVICE_ID		XPAR_PSU_SPI_0_DEVICE_ID
    #define UART_DEVICE_ID		XPAR_PSU_UART_0_DEVICE_ID
    #define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
  #else
    #define GPIO_OFFSET			54
  #endif
  #define DDR_MEM_BASEADDR		XPAR_DDR_MEM_BASEADDR
#else
  /* MicroBlaze / PL-only */
  #define GPIO_OFFSET			0
  #define GPIO_DEVICE_ID		XPAR_GPIO_0_DEVICE_ID
  #define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
  #define UART_IRQ_ID			XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
  #define DDR_MEM_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR
#endif

/******************************************************************************/
/*** AXI IP base addresses                                                  ***/
/******************************************************************************/
/* JESD204 and TPL — same XPAR names on both platforms */
#define RX_JESD_BASEADDR		XPAR_AXI_JESD204_RX_0_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_JESD204_TX_0_BASEADDR
#define RX_CORE_BASEADDR		XPAR_AD_IP_JESD204_TPL_ADC_0_BASEADDR
#define TX_CORE_BASEADDR		XPAR_AD_IP_JESD204_TPL_DAC_0_BASEADDR

#ifdef PLATFORM_VERSAL
  #define RX_DMA_BASEADDR		XPAR_AXI_JESD_RX_DMA_BASEADDR
  #define TX_DMA_BASEADDR		XPAR_AXI_JESD_TX_DMA_BASEADDR
  #define TX_DATA_OFFLOAD_BASEADDR	0xBC440000UL
  #define RX_DATA_OFFLOAD_BASEADDR	0xBC450000UL
#else
  #define RX_DMA_BASEADDR		XPAR_AXI_ADRV903X_RX_DMA_BASEADDR
  #define TX_DMA_BASEADDR		XPAR_AXI_ADRV903X_TX_DMA_BASEADDR
  #define TX_DATA_OFFLOAD_BASEADDR	0x9C440000UL
  #define RX_DATA_OFFLOAD_BASEADDR	0x9C450000UL
  #define RX_XCVR_BASEADDR		XPAR_AXI_ADRV903X_RX_XCVR_BASEADDR
  #define TX_XCVR_BASEADDR		XPAR_AXI_ADRV903X_TX_XCVR_BASEADDR
  #define RX_CLKGEN_BASEADDR		XPAR_AXI_ADRV903X_RX_CLKGEN_BASEADDR
  #define TX_CLKGEN_BASEADDR		XPAR_AXI_ADRV903X_TX_CLKGEN_BASEADDR
#endif

/******************************************************************************/
/***  Buffer sizes for DMA/IIO examples                                     ***/
/******************************************************************************/
#define DAC_BUFFER_SAMPLES		65536
#define ADC_BUFFER_SAMPLES		32768
#define ADC_CHANNELS			16

/******************************************************************************/
/***  SPI chip selects and GPIO offsets                                      ***/
/******************************************************************************/
#ifdef PLATFORM_VERSAL
  /* Versal Tetra GPIO pins (from DTS axi_gpio offsets) */
  #define ADRV903X_CS			0
  #define HMC7044_CS			1
  #define HMC7044_SPI_SPEED_HZ		1000000u
  #define ADRV903X_RESET_B		(GPIO_OFFSET + 20)
  #define HMC7044_RESET_B		(GPIO_OFFSET + 21)
  /* PS MIO GPIO */
  #define PS_GPIO_DEVICE_ID		XPAR_XGPIOPS_1_BASEADDR
  #define LINK_STATUS_GPIO_MIO		11
  /* GT reset GPIOs */
  #define RX_GT_RESET_DONE		(GPIO_OFFSET + 32)
  #define TX_GT_RESET_DONE		(GPIO_OFFSET + 33)
  #define RX_GT_PLL_RESET		(GPIO_OFFSET + 36)
  #define TX_GT_PLL_RESET		(GPIO_OFFSET + 37)
  #define RX_GT_DP_RESET		(GPIO_OFFSET + 38)
  #define TX_GT_DP_RESET		(GPIO_OFFSET + 39)
#else
  #define AD9528_CS			1
  #define ADRV903X_CS			0
  #define DAC_GPIO_PLDDR_BYPASS		(GPIO_OFFSET + 70)
  #define AD9528_RESET_B		(GPIO_OFFSET + 69)
  #define AD9528_SYSREF_REQ		(GPIO_OFFSET + 68)
  #define ADRV903X_RESET_B		(GPIO_OFFSET + 56)
#endif

extern struct xil_spi_init_param spi_extra;
extern struct xil_gpio_init_param xil_gpio_param;

#endif /* _PARAMETERS_H_ */
