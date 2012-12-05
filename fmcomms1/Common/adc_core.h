/**************************************************************************//**
*   @file   adc_core.h
*   @brief  ADC core header file.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

#ifndef __ADC_CORE_H__
#define __ADC_CORE_H__

/*****************************************************************************/
/******************* Include Files *******************************************/
/*****************************************************************************/
#include <stdint.h>

/*****************************************************************************/
/******************* ADC Core Registers Definitions *******************************/
/*****************************************************************************/
#define ADC_CORE_VERSION		0x00
#define ADC_CORE_DMA_CHAN_SEL	0x04
#define ADC_CORE_DMA_CTRL		0x0C
#define ADC_CORE_DMA_STAT		0x10
#define ADC_CORE_ADC_STAT		0x14
#define ADC_CORE_PN_ERR_CTRL	0x24
#define ADC_CORE_ADC_CTRL		0x2C
#define ADC_CORE_IDENT		    0x30
#define ADC_CORE_CA_OFFS_SCALE	0x40
#define ADC_CORE_CB_OFFS_SCALE	0x44

/* ADC_CORE_DMA_CHAN_SEL */
#define ADC_CORE_DMA_CHAN_SEL0	(1 << 0)
#define ADC_CORE_DMA_CHAN_SEL1	(1 << 1)

/* ADC_CORE_DMA_CTRL */
#define ADC_CORE_DMA_CAP_EN		(1 << 16)
#define ADC_CORE_DMA_CNT(x)		(((x) & 0xFFFF) << 0)

/* ADC_CORE_DMA_STAT */
#define ADC_CORE_DMA_STAT_BUSY		(1 << 0) /* W1C */
#define ADC_CORE_DMA_STAT_OVF		(1 << 1) /* W1C */
#define ADC_CORE_DMA_STAT_UNF		(1 << 2) /* W1C */

/* ADC_CORE_ADC_STAT */
#define ADC_CORE_ADC_STAT_OVR0	    (1 << 0) /* W1C */
#define ADC_CORE_ADC_STAT_OVR1	    (1 << 1) /* W1C */
#define ADC_CORE_ADC_STAT_PN_OOS0	(1 << 2) /* W1C */
#define ADC_CORE_ADC_STAT_PN_OOS1	(1 << 3) /* W1C */
#define ADC_CORE_ADC_STAT_PN_ERR0	(1 << 4) /* W1C */
#define ADC_CORE_ADC_STAT_PN_ERR1	(1 << 5) /* W1C */
#define ADC_CORE_ADC_STAT_MASK	    0x3F

/* ADC_CORE_ADC_STAT */
#define ADC_CORE_ADC_STAT_OVR	    (1 << 0) /* W1C */
#define ADC_CORE_ADC_STAT_PN_OOS	(1 << 1) /* W1C */
#define ADC_CORE_ADC_STAT_PN_ERR	(1 << 2) /* W1C */
#define ADC_CORE_ADC_1C_STAT_MASK	0x7

/* ADC_CORE_PN_ERR_CTRL */
#define ADC_CORE_PN23_1_EN		(1 << 1)
#define ADC_CORE_PN23_0_EN		(1 << 0)
#define ADC_CORE_PN9_1_EN	    (0 << 1)
#define ADC_CORE_PN9_0_EN	    (0 << 0)
#define ADC_CORE_PN23_EN	    (ADC_CORE_PN23_0_EN | ADC_CORE_PN23_1_EN)
#define ADC_CORE_PN9_EN			(ADC_CORE_PN9_0_EN | ADC_CORE_PN9_1_EN)

/* ADC_CORE_ADC_CTRL */
#define ADC_CORE_INPUT_FMT_OFFSET_BIN	(1 << 3)
#define ADC_CORE_INPUT_FMT_TWOS_COMPL	(0 << 3)
#define ADC_CORE_SCALE_OFFSET_EN		(1 << 2)
#define ADC_CORE_SIGNEXTEND		(1 << 1)
#define ADC_CORE_STATUS_EN		(1 << 0)

/* ADC_CORE_IDENT */
#define ADC_CORE_IDENT_SLAVE	0x1

/* ADC_CORE_C[A|B]_OFFS_SCALE */
#define ADC_CORE_OFFSET(x)		(((x) & 0xFFFF) << 16)
#define ADC_CORE_SCALE(x)			((x) & 0xFFFF)


/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

/** Reads ADC Core Register. */
void ADC_Core_Read(uint32_t regAddr, uint32_t *data); 
/** Writes ADC Core Register. */
void ADC_Core_Write(uint32_t regAddr, uint32_t data); 

#endif /* __ADC_CORE_H__ */
