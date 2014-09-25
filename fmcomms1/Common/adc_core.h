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

/* ADC COMMON */
#define ADC_REG_RSTN			0x0040
#define ADC_RSTN				(1 << 0)
#define ADC_MMCM_RSTN 			(1 << 1)

/* ADC CHANNEL */
#define ADC_REG_CHAN_CNTRL(c)	(0x0400 + (c) * 0x40)
#define ADC_PN_SEL				(1 << 10)
#define ADC_IQCOR_ENB			(1 << 9)
#define ADC_DCFILT_ENB			(1 << 8)
#define ADC_FORMAT_SIGNEXT		(1 << 6)
#define ADC_FORMAT_TYPE			(1 << 5)
#define ADC_FORMAT_ENABLE		(1 << 4)
#define ADC_PN23_TYPE			(1 << 1)
#define ADC_ENABLE				(1 << 0)

#define ADC_REG_CHAN_STATUS(c)	(0x0404 + (c) * 0x40)
#define ADC_PN_ERR				(1 << 2)
#define ADC_PN_OOS				(1 << 1)
#define ADC_OVER_RANGE			(1 << 0)

#define ADC_REG_CHAN_CNTRL_2(c)	(0x0414 + (c) * 0x40)
#define ADC_IQCOR_COEFF_1(x)	(((x) & 0xFFFF) << 16)
#define ADC_TO_IQCOR_COEFF_1(x)	(((x) >> 16) & 0xFFFF)
#define ADC_IQCOR_COEFF_2(x)	(((x) & 0xFFFF) << 0)
#define ADC_TO_IQCOR_COEFF_2(x)	(((x) >> 0) & 0xFFFF)

#define ADI_REG_CHAN_CNTRL_3(c)	(0x0418 + (c) * 0x40) /* v8.0 */
#define ADI_ADC_PN_SEL(x)		(((x) & 0xF) << 16)

enum adc_pn_sel {
	ADC_PN9 = 0,
	ADC_PN23A = 1,
	ADC_PN7 = 4,
	ADC_PN15 = 5,
	ADC_PN23 = 6,
	ADC_PN31 = 7,
	ADC_PN_CUSTOM = 9,
	ADC_PN_END = 10,
};

#define PCORE_VERSION_MAJOR(version) (version >> 16)

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

/** Initializes the ADC Core */
void ADC_Core_Init(uint32_t fmcPort);
/** Reads ADC Core Register. */
void ADC_Core_Read(uint32_t regAddr, uint32_t *data); 
/** Writes ADC Core Register. */
void ADC_Core_Write(uint32_t regAddr, uint32_t data); 

#endif /* __ADC_CORE_H__ */
