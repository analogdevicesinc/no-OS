/***************************************************************************//**
 *   @file   cf_ad9467.h
 *   @brief  Header file of cf_ad9467 Driver.
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/
#ifndef __CF_AD9467_H__
#define __CF_AD9467_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xbasic_types.h"
#include "xparameters.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
	#define CF_BASEADDR   XPAR_AXI_ADC_1C_0_BASEADDR
	#define DDR_BASEADDR  XPAR_DDR_MEM_BASEADDR + 128*1024*1024
	#define DMA_BASEADDR  XPAR_AXI_DMA_0_BASEADDR
	#define UART_BASEADDR XPS_UART1_BASEADDR
	#define SPI_BASEADDR  XPAR_AXI_SPI_0_BASEADDR
#else
	#define CF_BASEADDR   XPAR_AXI_ADC_1C_0_BASEADDR
	#define DDR_BASEADDR  XPAR_DDR3_SDRAM_S_AXI_BASEADDR
	#define DMA_BASEADDR  XPAR_AXI_DMA_0_BASEADDR
	#define UART_BASEADDR XPS_UART1_BASEADDR
	#define SPI_BASEADDR  XPAR_AXI_SPI_0_BASEADDR
#endif

/* CF register map. */
#define CF_REG_VERSION			0x00
#define CF_REG_CAPTURE_CTRL		0x0C
#define CF_REG_ADC_STATUS		0x10
#define CF_REG_DATA_MONITOR		0x14
#define CF_REG_DATA_MODE		0x18
#define CF_REG_DELAY_CTRL		0x1C
#define CF_REG_DELAY_STATUS		0x20
#define CF_REG_PN_TYPE			0x24
#define CF_REG_DATA_SELECT		0x28

/* CF_REG_CAPTURE_CTRL bit definition. */
#define CF_CAPTURE_CTRL_CAPTURE_START(x)	(((x) & 0x1) << 16)
#define CF_CAPTURE_CTRL_CAPTURE_COUNT(x)	((((x) - 1) & 0xFFFF) << 0)

/* CF_REG_ADC_STATUS bit definition. */
#define CF_ADC_STATUS_UNDERFLOW		(1 << 2) // (Write 1 to clear)
#define CF_ADC_STATUS_OVERFLOW		(1 << 1) // (Write 1 to clear)
#define CF_ADC_STATUS_BUSY			(1 << 0) // (Read Only)

/* CF_REG_DATA_MONITOR bit definition. */
#define CF_DATA_MONITOR_PN_ERR		(1 << 2) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_SYNC		(1 << 1) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_OVER_RNG	(1 << 0) // (Write 1 to clear)

/* CF_REG_DATA_MODE bit definition. */
#define CF_DATA_MODE_BITS(x)		(((x) & 0x3) << 0)

/* CF_REG_DELAY_CTRL bit definition. */
#define CF_DELAY_CTRL_SEL(x)		(((x) & 0x1) << 17)
#define CF_DELAY_CTRL_RW_ENABLE(x)  (((x) & 0x1) << 16)
#define CF_DELAY_CTRL_WR_ADDR(x)	(((x) & 0xF) << 8)
#define CF_DELAY_CTRL_WR_DATA(x)	(((x) & 0x1F) << 0)

/* CF_REG_DELAY_STATUS bit definition. */
#define CF_DELAY_STATUS_LOCKED		(1 << 8)	// (Read Only)
#define CF_DELAY_STATUS_RD_DATA		(0x1F << 0) // (Read Only)

/* CF_REG_PN_TYPE bit definition. */
#define CF_PN_TYPE_BIT(x)			(((x) & 0x1) << 0)

/* CF_REG_DATA_SELECT bit definition. */
#define CF_DATA_SELECT_BIT(x)		(((x) & 0x1) << 0)

/******************************************************************************/
/************************** Types Declarations ********************************/
/******************************************************************************/
typedef enum _TestModes
{
	TEST_DISABLE = 0,
	MIDSCALE,
	POS_FULLSCALE,
	NEG_FULLSCALE,
	CHECKERBOARD,
	PN_23_SEQUENCE,
	PN_9_SEQUENCE,
	ONE_ZERO_TOGGLE

}typeTestModes;

typedef enum _OutputModes
{
	OFFSET_BINARY = 0,
	TWOS_COMPLEMENT,
	GRAY_CODE,

}typeOutputModes;

/*******************************************************************************/
/************************ Functions Declarations *******************************/
/*******************************************************************************/
/*! Initializes the ADC FPGA core. FPGA core. */
void adc_setup(u32 dco_delay);
/*! Configures the AD9467 device to run in the selected test mode. */
void adc_test(u32 mode, u32 format);
/*! Captures a specified number of samples from the ADC. */
void adc_capture(u32 qwcnt, u32 sa);

#endif /* __CF_AD9467_H__ */
