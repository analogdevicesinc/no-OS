/***************************************************************************//**
 *   @file   ad7091.h
 *   @brief  Header file of ADC core driver for AD7091.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
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
 *
******************************************************************************/
#ifndef _AD7091_H_
#define _AD7091_H_

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/

/* Register Map - Offset from the Base Address */
// General registers
#define AD7091_REG_VERSION             0x0000
#define AD7091_REG_ID                  0x0004
#define AD7091_REG_SCRATCH             0x0008

// ADC Common                                              Type
#define AD7091_REG_RSTN                0x0040         //       RW
#define AD7091_REG_CNTRL               0x0044         //       RW
#define AD7091_REG_CLK_FREQ            0x0054         //       RO
#define AD7091_REG_CLK_RATIO           0x0058         //       RO
#define AD7091_REG_STATUS              0x005C         //       RO
#define AD7091_REG_DELAY_CNTRL         0x0060         //       RW
#define AD7091_REG_DELAY_STATUS        0x0064         //       RO
#define AD7091_REG_DRP_CNTRL           0x0070         //       RW
#define AD7091_REG_DRP_STATUS          0x0074         //       RO
#define AD7091_REG_DMA_CNTRL           0x0080         //       RW
#define AD7091_REG_DMA_COUNT           0x0084         //       RW
#define AD7091_REG_DMA_STATUS          0x0088         //       RW1C
#define AD7091_REG_DMA_BUSWIDTH        0x008C         //       RO
#define AD7091_REG_USR_CNTRL_1         0x00A0         //       RW
#define AD7091_REG_USR_CNTRL_2         0x00C0         //       RO

// Definition of register AD7091_REG_RSTN
#define ADC_MMCM_RSTN               (1 << 0x01)
#define ADC_RSTN                    (1 << 0x00)

// Definition of register AD7091_REG_CNTRL
#define ADC_R1_MODE                 (1 << 0x02)
#define ADC_DDR_EDGESEL             (1 << 0x01)
#define ADC_PIN_MODE                (1 << 0x00)

// Definition of register AD7091_REG_STATUS
#define ADC_PN_ERR                  (1 << 0x03)
#define ADC_PN_OOS                  (1 << 0x02)
#define ADC_OVER_RANGE              (1 << 0x01)
#define ADC_STATUS                  (1 << 0x00)

// Definition of register AD7091_REG_DELAY_CNTRL
#define ADC_DELAY_SEL               (1 << 0x11)
#define ADC_DELAY_RWN               (1 << 0x10)
#define ADC_DELAY_ADDRESS           (1 << 0x08)
#define ADC_DELAY_WDATA             (1 << 0x00)

#define ADC_DELAY_ADDRESS_MASK      0x00FF
#define ADC_DELAY_WDATA_MASK        0x001F

// Definition of register AD7091_REG_DELAY_STATUS
#define ADC_DELAY_LOCKED            (1 << 0x09)
#define ADC_DELAY_STATUS            (1 << 0x08)
#define ADC_DELAY_RDATA             (1 << 0x00)

#define ADC_DELAY_RDATA_MASK        0x001F

// Definition of register AD7091_REG_DRP_CNTRL
#define ADC_DRP_RWN                 (1 << 0x1C)
#define ADC_DRP_ADDRESS             (1 << 0x10)
#define ADC_DRP_WDATA               (1 << 0x00)

#define ADC_DRP_ADDRESS_MASK        0x0FFF
#define ADC_DRP_DATA_MASK           0xFFFF

// Definition of register AD7091_REG_DRP_STATUS
#define ADC_DRP_STATUS              (1 << 0x10)
#define ADC_DRP_RDATA               (1 << 0x00)

// Definition of register AD7091_REG_DMA_CNTRL
#define ADC_DMA_STREAM              (1 << 0x01)
#define ADC_DMA_START               (1 << 0x00)

// Definition of register AD7091_REG_STATUS
#define ADC_DMA_OVF                 (1 << 0x02)
#define ADC_DMA_UNF                 (1 << 0x01)
#define ADC_DMA_STATUS              (1 << 0x00)

// Definition of register AD7091_REG_USER_CNTRL_1
#define ADC_USR_CHANMAX             (1 << 0x00)
#define ADC_USR_CHANMAX_MASK        0x00FF

// Definition of register AD7091_REG_USER_CNTRL_2
#define ADC_DP_DISABLE              (1 << 0x00)

#define BUS_WIDTH                   4

/*****************************************************************************/
/************************* Functions Declarations ****************************/
/*****************************************************************************/
// Initialization
void AD7091_Init(unsigned int baseAddress);

// Functions for register access
void AD7091_SetRegister(unsigned int address, unsigned int data);
unsigned int AD7091_GetRegister(unsigned int address);

// Initiate data acquisition
unsigned int AD7091_GetData(unsigned int size);

#endif  /* _AD7091_H_ */
