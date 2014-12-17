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
#include <stdint.h>
#include "xbasic_types.h"
#include "xparameters.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
    #define CF_BASEADDR   XPAR_AXI_AD9467_BASEADDR
    #define DDR_BASEADDR  XPAR_DDR_MEM_BASEADDR + 128*1024*1024
    #define DMA_BASEADDR  XPAR_AXI_DMA_0_BASEADDR
    #define UART_BASEADDR XPS_UART1_BASEADDR
    #define SPI_BASEADDR  XPAR_AXI_SPI_0_BASEADDR
#else
    #define CF_BASEADDR   XPAR_AXI_AD9467_BASEADDR
    #define DDR_BASEADDR  XPAR_AXI_DDR_CNTRL_BASEADDR
    #define DMA_BASEADDR  XPAR_AXI_DMA_0_BASEADDR
    #define UART_BASEADDR XPS_UART1_BASEADDR
    #define SPI_BASEADDR  XPAR_AXI_SPI_0_BASEADDR
#endif

/* ADC registers */
#define CF_REG_DELAY_CTRL			0x60
#define CF_REG_DELAY_STATUS			0x20
#define CF_REG_DATA_MONITOR			0x404

/* CF_REG_DELAY_CTRL bit definition. */
#define CF_DELAY_CTRL_SEL(x)        (((x) & 0x1) << 17)
#define CF_DELAY_CTRL_RW_ENABLE(x)  (((x) & 0x1) << 16)
#define CF_DELAY_CTRL_WR_ADDR(x)    (((x) & 0xF) << 8)
#define CF_DELAY_CTRL_WR_DATA(x)    (((x) & 0x1F) << 0)

/* CF_REG_DELAY_STATUS bit definition. */
#define CF_DELAY_STATUS_LOCKED      (1 << 9)    // (Read Only)
#define CF_DELAY_STATUS_STATUS      (1 << 8)    // (Read Only)
#define CF_DELAY_STATUS_RD_DATA     (0x1F << 0) // (Read Only)

/* CF_REG_DATA_MONITOR bit definition. */
#define CF_DATA_MONITOR_PN_ERR         (1 << 2) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_SYNC        (1 << 1) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_OVER_RNG    (1 << 0) // (Write 1 to clear)

#define ADC_REG_RSTN			0x040
#define ADC_MMCM_RSTN			(1 << 1)
#define ADC_RSTN				(1 << 0)

#define ADC_REG_CHAN_CNTRL(c)	(0x0400 + (c) * 0x40)
#define ADC_PN_SEL				(1 << 10)	/* !v8.0 */
#define ADC_IQCOR_ENB			(1 << 9)
#define ADC_DCFILT_ENB			(1 << 8)
#define ADC_FORMAT_SIGNEXT		(1 << 6)
#define ADC_FORMAT_TYPE			(1 << 5)
#define ADC_FORMAT_ENABLE		(1 << 4)
#define ADC_PN23_TYPE			(1 << 1)	/* !v8.0 */
#define ADC_ENABLE				(1 << 0)

#define ADC_REG_CHAN_CNTRL_3(c)	(0x0418 + (c) * 0x40) /* v8.0 */
#define ADC_ADC_PN_SEL(x)		(((x) & 0xF) << 16)
#define ADC_TO_ADC_PN_SEL(x)	(((x) >> 16) & 0xF)
#define ADC_ADC_DATA_SEL(x)		(((x) & 0xF) << 0)
#define ADC_TO_ADC_DATA_SEL(x)	(((x) >> 0) & 0xF)

/* DMAC registers */
#define AXI_DMAC_REG_IRQ_MASK			0x80
#define AXI_DMAC_REG_IRQ_PENDING		0x84
#define AXI_DMAC_REG_IRQ_SOURCE			0x88
#define AXI_DMAC_REG_CTRL				0x400
#define AXI_DMAC_REG_TRANSFER_ID		0x404
#define AXI_DMAC_REG_START_TRANSFER		0x408
#define AXI_DMAC_REG_FLAGS				0x40c
#define AXI_DMAC_REG_DEST_ADDRESS		0x410
#define AXI_DMAC_REG_SRC_ADDRESS		0x414
#define AXI_DMAC_REG_X_LENGTH			0x418
#define AXI_DMAC_REG_Y_LENGTH			0x41c
#define AXI_DMAC_REG_DEST_STRIDE		0x420
#define AXI_DMAC_REG_SRC_STRIDE			0x424
#define AXI_DMAC_REG_TRANSFER_DONE		0x428
#define AXI_DMAC_REG_ACTIVE_TRANSFER_ID 0x42c
#define AXI_DMAC_REG_STATUS				0x430
#define AXI_DMAC_REG_CURRENT_DEST_ADDR	0x434
#define AXI_DMAC_REG_CURRENT_SRC_ADDR	0x438
#define AXI_DMAC_REG_DBG0				0x43c
#define AXI_DMAC_REG_DBG1				0x440

#define AXI_DMAC_CTRL_ENABLE			(1 << 0)
#define AXI_DMAC_CTRL_PAUSE				(1 << 1)

#define AXI_DMAC_IRQ_SOT				(1 << 0)
#define AXI_DMAC_IRQ_EOT				(1 << 1)

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

enum adc_pnsel {
	ADC_PN9 = 0,
	ADC_PN23A = 1,
	ADC_PN7 = 4,
	ADC_PN15 = 5,
	ADC_PN23 = 6,
	ADC_PN31 = 7,
	ADC_PN_CUSTOM = 9,
	ADC_PN_END = 10,
};

/*******************************************************************************/
/************************ Functions Declarations *******************************/
/*******************************************************************************/
/*! Initializes the ADC FPGA core. */
void adc_setup(uint32_t dco_delay);
/*! Configures the AD9467 device to run in the selected test mode. */
void adc_test(uint32_t mode, uint32_t format);
/*! Captures a specified number of samples from the ADC. */
void adc_capture(uint32_t qwcnt, uint32_t sa);

#endif /* __CF_AD9467_H__ */
