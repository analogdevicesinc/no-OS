/***************************************************************************//**
 *   @file   cf_ad9739a.h
 *   @brief  Header file of cf_ad9739a Driver.
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
#ifndef __CF_AD9739A_H__
#define __CF_AD9739A_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "xuartlite_l.h"
#include "xparameters.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
#define CF_BASEADDR   XPAR_AXI_DAC_1C_2P_0_BASEADDR
#define DDR_BASEADDR  XPAR_DDR3_SDRAM_S_AXI_BASEADDR
#define UART_BASEADDR XPAR_RS232_UART_1_BASEADDR
#define SPI_BASEADDR  XPAR_AXI_SPI_0_BASEADDR
#define LCD_BASEADDR  XPAR_AXI_GPIO_0_BASEADDR
#define VDMA_BASEADDR XPAR_AXI_VDMA_0_BASEADDR

/* CF register map. */
#define CF_REG_VERSION    0x00
#define CF_REG_DDS_CTRL   0x04
#define CF_REG_DDS_STAT   0x24

/***************************************************************************//**
* This register controls the dds outputs. The tone may be generated either from
* Xilinx core or the DDR. Note that DDR throughput may not be sufficient for dac
* at full speed.
*******************************************************************************/
/* CF_REG_DDS_CTRL bit definition. */
#define CF_DDS_CTRL_SELECT(x)    (((x) & 0x1) << 18)  // DDS DDR (1) or Xilinx (0) select
#define CF_DDS_CTRL_INTERPOL(x)  (((x) & 0x1) << 17)  // DDS DDR interpolation enable (1) or disable (0)
#define CF_DDS_CTRL_ENABLE(x)    (((x) & 0x1) << 16)  // DDS enable (0->1)
#define CF_DDS_CTRL_INCR(x)      ((x) & 0xFFFF)       // DDS phase increment (2pi = 0xffffw)

/***************************************************************************//**
* This register indicates the status of DDR-DDS and are applicable only if dds
* select is set to 0x1.
*******************************************************************************/
/* CF_REG_DDS_STAT bit definition. */
#define CF_DDS_STAT_VDMA_UNDERFLOW    (1 << 1)  // VDMA Underflow (W1C) - Write 1 to Clear
#define CF_DDS_STAT_VDMA_OVERFLOW     (1 << 0)  // VDMA Overflow (W1C)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Creates a delay of milliseconds. */
void delay_ms(uint32_t ms_count);

/*! Configures the DDS core. */
void dds_setup(uint32_t sin_clk, uint32_t dac_clk);

/*! Configures the DMA core. */
void dma_setup();

/*! Waits for an input from user. */
uint32_t user_exit(void);

#endif /* __CF_AD9739A_H__ */
