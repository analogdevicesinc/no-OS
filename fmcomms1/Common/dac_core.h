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

#define ADI_REG_VERSION		0x0000 /*Version and Scratch Registers */
#define ADI_VERSION_NUM(x)	(((x) & 0xffffffff) << 0) /* RO, Version number. */
#define VERSION_IS(x,y,z)	((x) << 16 | (y) << 8 | (z))
#define ADI_REG_ID			0x0004 /*Version and Scratch Registers */
#define ADI_ID(x)			(((x) & 0xffffffff) << 0) /* RO, Instance identifier number. */
#define ADI_REG_SCRATCH		0x0008 /*Version and Scratch Registers */
#define ADI_SCRATCH(x)		(((x) & 0xffffffff) << 0) /* RW, Scratch register. */

#define PCORE_VERSION_MAJOR(version)	(version >> 16)

/* DAC COMMON */
#define ADI_REG_RSTN		0x0040
#define ADI_RSTN			(1 << 0)
#define ADI_MMCM_RSTN		(1 << 1)

#define ADI_REG_CNTRL_1		0x0044
#define ADI_ENABLE			(1 << 0)

#define ADI_REG_CNTRL_2		0x0048
#define ADI_PAR_TYPE		(1 << 7)
#define ADI_PAR_ENB			(1 << 6)
#define ADI_R1_MODE			(1 << 5)
#define ADI_DATA_FORMAT		(1 << 4)
#define ADI_DATA_SEL(x)		(((x) & 0xF) << 0)
#define ADI_TO_DATA_SEL(x)	(((x) >> 0) & 0xF)

enum {
	DATA_SEL_DDS,
	DATA_SEL_SED,
	DATA_SEL_DMA,
};

#define ADI_REG_RATECNTRL 	0x004C
#define ADI_RATE(x)			(((x) & 0xFF) << 0)
#define ADI_TO_RATE(x)		(((x) >> 0) & 0xFF)

#define ADI_REG_FRAME		0x0050
#define ADI_FRAME			(1 << 0)

#define ADI_REG_CLK_FREQ	0x0054
#define ADI_CLK_FREQ(x)		(((x) & 0xFFFFFFFF) << 0)
#define ADI_TO_CLK_FREQ(x)	(((x) >> 0) & 0xFFFFFFFF)

#define ADI_REG_CLK_RATIO	0x0058
#define ADI_CLK_RATIO(x)	(((x) & 0xFFFFFFFF) << 0)
#define ADI_TO_CLK_RATIO(x)	(((x) >> 0) & 0xFFFFFFFF)

#define ADI_REG_STATUS		0x005C
#define ADI_STATUS			(1 << 0)

#define ADI_REG_DRP_CNTRL		0x0070
#define ADI_DRP_SEL				(1 << 29)
#define ADI_DRP_RWN				(1 << 28)
#define ADI_DRP_ADDRESS(x)		(((x) & 0xFFF) << 16)
#define ADI_TO_DRP_ADDRESS(x) 	(((x) >> 16) & 0xFFF)
#define ADI_DRP_WDATA(x)		(((x) & 0xFFFF) << 0)
#define ADI_TO_DRP_WDATA(x)		(((x) >> 0) & 0xFFFF)

#define ADI_REG_DRP_STATUS		0x0074
#define ADI_DRP_LOCKED			(1 << 17)
#define ADI_DRP_STATUS			(1 << 16)
#define ADI_DRP_RDATA(x)		(((x) & 0xFFFF) << 0)
#define ADI_TO_DRP_RDATA(x) 	(((x) >> 0) & 0xFFFF)

#define ADI_REG_VDMA_FRMCNT		0x0084
#define ADI_VDMA_FRMCNT(x)		(((x) & 0xFFFFFFFF) << 0)
#define ADI_TO_VDMA_FRMCNT(x)	(((x) >> 0) & 0xFFFFFFFF)

#define ADI_REG_VDMA_STATUS		0x0088
#define ADI_VDMA_OVF			(1 << 1)
#define ADI_VDMA_UNF			(1 << 0)

#define ADI_REG_USR_CNTRL_1		0x00A0
#define ADI_USR_CHANMAX(x)		(((x) & 0xFF) << 0)
#define ADI_TO_USR_CHANMAX(x)	(((x) >> 0) & 0xFF)


/* DAC CHANNEL */
#define ADI_REG_CHAN_CNTRL_1_IIOCHAN(x)	(0x0400 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define ADI_DDS_SCALE(x)				(((x) & 0xFFFF) << 0)
#define ADI_TO_DDS_SCALE(x)				(((x) >> 0) & 0xFFFF)

#define ADI_REG_CHAN_CNTRL_2_IIOCHAN(x)	(0x0404 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define ADI_DDS_INIT(x)					(((x) & 0xFFFF) << 16)
#define ADI_TO_DDS_INIT(x)				(((x) >> 16) & 0xFFFF)
#define ADI_DDS_INCR(x)					(((x) & 0xFFFF) << 0)
#define ADI_TO_DDS_INCR(x)				(((x) >> 0) & 0xFFFF)

#define ADI_REG_CHAN_CNTRL_1(c)	(0x0400 + (c) * 0x40)
#define ADI_DDS_SCALE_1(x)		(((x) & 0xF) << 0)
#define ADI_TO_DDS_SCALE_1(x)	(((x) >> 0) & 0xF)

#define ADI_REG_CHAN_CNTRL_2(c)	(0x0404 + (c) * 0x40)
#define ADI_DDS_INIT_1(x)		(((x) & 0x1FFFF) << 15)
#define ADI_TO_DDS_INIT_1(x)	(((x) >> 15) & 0x1FFFF)
#define ADI_DDS_INCR_1(x)		(((x) & 0xFFFF) << 0)
#define ADI_TO_DDS_INCR_1(x)	(((x) >> 0) & 0xFFFF)

#define ADI_REG_CHAN_CNTRL_3(c)	(0x0408 + (c) * 0x40)
#define ADI_DDS_SCALE_2(x)		(((x) & 0xF) << 0)
#define ADI_TO_DDS_SCALE_2(x)	(((x) >> 0) & 0xF)

#define ADI_REG_CHAN_CNTRL_4(c)	(0x040C + (c) * 0x40)
#define ADI_DDS_INIT_2(x)		(((x) & 0x1FFFF) << 15)
#define ADI_TO_DDS_INIT_2(x)	(((x) >> 15) & 0x1FFFF)
#define ADI_DDS_INCR_2(x)		(((x) & 0xFFFF) << 0)
#define ADI_TO_DDS_INCR_2(x)	(((x) >> 0) & 0xFFFF)

#define ADI_REG_CHAN_CNTRL_5(c) (0x0410 + (c) * 0x40)
#define ADI_DDS_PATT_2(x)		(((x) & 0xFFFF) << 16)
#define ADI_TO_DDS_PATT_2(x)	(((x) >> 16) & 0xFFFF)
#define ADI_DDS_PATT_1(x)		(((x) & 0xFFFF) << 0)
#define ADI_TO_DDS_PATT_1(x)	(((x) >> 0) & 0xFFFF)

#define ADI_REG_CHAN_CNTRL_7(c) (0x0418 + (c) * 0x40) /* v8.0 */
#define ADI_DAC_DDS_SEL(x) (((x) & 0xF) << 0)
#define ADI_TO_DAC_DDS_SEL(x) (((x) >> 0) & 0xF)

#define ADI_REG_USR_CNTRL_3(c)				(0x0420 + (c) * 0x40)
#define ADI_USR_DATATYPE_BE					(1 << 25)
#define ADI_USR_DATATYPE_SIGNED				(1 << 24)
#define ADI_USR_DATATYPE_SHIFT(x)			(((x) & 0xFF) << 16)
#define ADI_TO_USR_DATATYPE_SHIFT(x)		(((x) >> 16) & 0xFF)
#define ADI_USR_DATATYPE_TOTAL_BITS(x)		(((x) & 0xFF) << 8)
#define ADI_TO_USR_DATATYPE_TOTAL_BITS(x)	(((x) >> 8) & 0xFF)
#define ADI_USR_DATATYPE_BITS(x)			(((x) & 0xFF) << 0)
#define ADI_TO_USR_DATATYPE_BITS(x)			(((x) >> 0) & 0xFF)

#define ADI_REG_USR_CNTRL_4(c)			(0x0424 + (c) * 0x40)
#define ADI_USR_INTERPOLATION_M(x)		(((x) & 0x1FFFF) << 15)
#define ADI_TO_USR_INTERPOLATION_M(x)	(((x) >> 15) & 0x1FFFF)
#define ADI_USR_INTERPOLATION_N(x)		(((x) & 0xFFFF) << 0)
#define ADI_TO_USR_INTERPOLATION_N(x)	(((x) >> 0) & 0xFFFF)

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
