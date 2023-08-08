/***************************************************************************//**
 *   @file   ad9208/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#define SPI_HMC7044_CS		0x00

#define SPI_0_DEVICE_ID		XPAR_SPI_0_DEVICE_ID
#define SPI_AD9208_0_CS		0x01
#define RX_0_JESD_BASEADDR 	XPAR_AXI_AD9208_0_JESD_RX_AXI_BASEADDR
#define RX_0_XCVR_BASEADDR 	XPAR_AXI_AD9208_0_XCVR_BASEADDR
#define RX_0_CORE_BASEADDR	XPAR_RX_AD9208_0_TPL_CORE_ADC_TPL_CORE_BASEADDR

#define SPI_DEVICE_ID		XPAR_AXI_SPI_DEVICE_ID
#define SPI_AD9208_CS		0x02

#define GPIO_DEVICE_ID		XPAR_GPIO_0_DEVICE_ID

#define RX_1_JESD_BASEADDR 	XPAR_AXI_AD9208_1_JESD_RX_AXI_BASEADDR
#define RX_1_XCVR_BASEADDR 	XPAR_AXI_AD9208_1_XCVR_BASEADDR
#define RX_1_CORE_BASEADDR	XPAR_RX_AD9208_1_TPL_CORE_ADC_TPL_CORE_BASEADDR

#define RX_DMA_BASEADDR		XPAR_AXI_AD9208_DMA_BASEADDR
#define ADC_DDR_BASEADDR	(XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR + 0x800000)
#define ADC_DDR_HIGHADDR	XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR

/* iio_app parameters */
#define UART_DEVICE_ID          XPAR_AXI_UART_DEVICE_ID
#define UART_BAUDRATE           115200
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#define INTC_DEVICE_ID          XPAR_INTC_SINGLE_DEVICE_ID

#endif /* __PARAMETERS_H__ */
