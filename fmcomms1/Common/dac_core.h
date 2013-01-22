/**************************************************************************//**
*   @file   dac_core.h
*   @brief  DAC core header file.
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

#ifndef __DAC_CORE_H__
#define __DAC_CORE_H__

/*****************************************************************************/
/******************* Include Files *******************************************/
/*****************************************************************************/
#include <stdint.h>

/*****************************************************************************/
/******************* DAC Core Registers Definitions *******************************/
/*****************************************************************************/
#define CF_AXI_DDS_VERSION_ID		0x00
#define CF_AXI_DDS_CTRL				0x04
#define CF_AXI_DDS_1A_OUTPUT_CTRL	0x08
#define CF_AXI_DDS_1B_OUTPUT_CTRL	0x0C
#define CF_AXI_DDS_2A_OUTPUT_CTRL	0x10
#define CF_AXI_DDS_2B_OUTPUT_CTRL	0x14
#define CF_AXI_DDS_INTERPOL_CTRL	0x18
#define CF_AXI_DDS_SCALE			0x20
#define CF_AXI_DDS_FRAME			0x24
#define CF_AXI_DDS_DMA_STAT			0x28
#define CF_AXI_DDS_PAT_DATA1		0x40
#define CF_AXI_DDS_PAT_DATA2		0x44
#define CF_AXI_DDS_PCORE_IDENT		0x48

/* CF_AXI_DDS_CTRL */
#define CF_AXI_DDS_CTRL_PATTERN_EN		(1 << 4)
#define CF_AXI_DDS_CTRL_INTERPOLATE		(1 << 3)
#define CF_AXI_DDS_CTRL_DDS_SEL			(1 << 2)
#define CF_AXI_DDS_CTRL_DATA_EN			(1 << 1)
#define CF_AXI_DDS_CTRL_DDS_CLK_EN_V1	(1 << 8)
#define CF_AXI_DDS_CTRL_DDS_CLK_EN_V2	(1 << 0)

/* CF_AXI_DDS_FRAME */
#define CF_AXI_DDS_FRAME_SYNC		0x1

/* CF_AXI_DDS_PCORE_IDENT */
#define CF_AXI_DDS_PCORE_IDENT_SLAVE	0x1

/* CF_AXI_DDS_DMA_STAT W1C */
#define CF_AXI_DDS_DMA_STAT_OVF		(1 << 0)
#define CF_AXI_DDS_DMA_STAT_UNF		(1 << 1)

#define AXIDDS_MAX_DMA_SIZE		(4 * 1024 * 1024) /* Randomly picked */

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

/** Initializes the DAC Core */
void DAC_Core_Init(uint32_t fmcPort);
/** Reads DAC Core Register. */
void DAC_Core_Read(uint32_t regAddr, uint32_t *data);
/** Writes DAC Core Register. */
void DAC_Core_Write(uint32_t regAddr, uint32_t data);

#endif /* __DAC_CORE_H__ */
