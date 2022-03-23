/***************************************************************************//**
 *   @file   iio_demo/src/app/parameters.h
 *   @brief  Parameters Definitions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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

#ifdef XILINX_PLATFORM
#include <xparameters.h>
#endif

#ifdef ADUCM_PLATFORM
#include "irq_extra.h"
#endif

#ifdef STM32_PLATFORM
#include "stm32_hal.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/


#ifdef XILINX_PLATFORM

#ifdef _XPARAMETERS_PS_H_
#define ADC_DDR_BASEADDR	(XPAR_DDR_MEM_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR	(XPAR_DDR_MEM_BASEADDR + 0xA000000)
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#endif

#else // _XPARAMETERS_PS_H_

#ifdef XPAR_DDR3_SDRAM_S_AXI_BASEADDR
#define ADC_DDR_BASEADDR	(XPAR_DDR3_SDRAM_S_AXI_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR	(XPAR_DDR3_SDRAM_S_AXI_BASEADDR + 0xA000000)
#else
#define ADC_DDR_BASEADDR	(XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000)
#define DAC_DDR_BASEADDR	(XPAR_AXI_DDR_CNTRL_BASEADDR + 0xA000000)
#endif

#define UART_DEVICE_ID	XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID	XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#endif // _XPARAMETERS_PS_H_

/* 400 * 8 * 2 = 6400 Default number of samples requested on a capture */
#define MAX_SIZE_BASE_ADDR	10000
#define UART_BAUDRATE	115200

#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM

#define UART_DEVICE_ID	0
#define INTC_DEVICE_ID	0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200

#endif // ADUCM_PLATFORM

#ifdef STM32_PLATFORM

#define UART_DEVICE_ID	5
extern UART_HandleTypeDef huart5;
#define IIO_APP_HUART	(&huart5)
#define UART_BAUDRATE	115200
#define UART_IRQ_ID		UART5_IRQn

#endif // STM32_PLATFORM

#ifdef USE_TCP_SOCKET
#define WIFI_SSID	"RouterSSID"
#define WIFI_PWD	"******"
#endif /* USE_TCP_SOCKET */

#endif // __PARAMETERS_H__
