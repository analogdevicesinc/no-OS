/***************************************************************************//**
 *   @file   ad7124-8pmdz/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#ifdef XILINX_PLATFORM
#include <xparameters.h>
#endif

#ifdef ADUCM_PLATFORM
#include "aducm3029_irq.h"
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

#define UART_OPS        &xil_uart_ops

/* 400 * 8 * 2 = 6400‬ Default number of samples requested on a capture */
#define MAX_SIZE_BASE_ADDR	10000
#define UART_BAUDRATE	115200

#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM

#define UART_DEVICE_ID	0
#define INTC_DEVICE_ID	0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200
#define UART_OPS        &aducm_uart_ops

#endif //ADUCM_PLATFORM

#define WIFI_SSID	"RouterSSID"
#define WIFI_PWD	"******"

#endif // __PARAMETERS_H__
