/***************************************************************************//**
* @file dac_core.h
* @brief Header file of DAC Core Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#ifndef DAC_CORE_H_
#define DAC_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DAC_REG_RSTN			0x040
#define DAC_MMCM_RSTN			(1 << 1)
#define DAC_RSTN				(1 << 0)

#define DAC_REG_CNTRL_1			0x0044
#define DAC_ENABLE				(1 << 0) /* v7.0 */
#define DAC_SYNC				(1 << 0) /* v8.0 */

#define DAC_REG_CLK_FREQ		0x0054
#define DAC_CLK_FREQ(x)			(((x) & 0xFFFFFFFF) << 0)
#define DAC_TO_CLK_FREQ(x)		(((x) >> 0) & 0xFFFFFFFF)

#define DAC_REG_CLK_RATIO		0x0058
#define DAC_CLK_RATIO(x)		(((x) & 0xFFFFFFFF) << 0)
#define DAC_TO_CLK_RATIO(x)		(((x) >> 0) & 0xFFFFFFFF)

#define DAC_REG_CHAN_CNTRL_1_IIOCHAN(x)	(0x0400 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define DAC_DDS_SCALE(x)				(((x) & 0xFFFF) << 0)
#define DAC_TO_DDS_SCALE(x)				(((x) >> 0) & 0xFFFF)

#define DAC_REG_CHAN_CNTRL_2_IIOCHAN(x)	(0x0404 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define DAC_DDS_INIT(x)					(((x) & 0xFFFF) << 16)
#define DAC_TO_DDS_INIT(x)				(((x) >> 16) & 0xFFFF)
#define DAC_DDS_INCR(x)					(((x) & 0xFFFF) << 0)
#define DAC_TO_DDS_INCR(x)				(((x) >> 0) & 0xFFFF)

#define DAC_REG_CHAN_CNTRL_7(c)			(0x0418 + (c) * 0x40) /* v8.0 */
#define DAC_DAC_DDS_SEL(x)				(((x) & 0xF) << 0)
#define DAC_TO_DAC_DDS_SEL(x)			(((x) >> 0) & 0xF)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t dac_read(uint32_t reg_addr, uint32_t *reg_data);
int32_t dac_write(uint32_t reg_addr, uint32_t reg_data);
int32_t dac_setup(uint32_t baseaddr);

#endif
