/***************************************************************************//**
 *   @file   axi_dac_core.c
 *   @brief  Driver for the Analog Devices AXI-DAC-CORE module.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "platform_drivers.h"
#include "util.h"
#include "xil_cache.h"
#include "axi_dac_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_DAC_REG_RSTN				0x40
#define AXI_DAC_MMCM_RSTN				BIT(1)
#define AXI_DAC_RSTN					BIT(0)

#define AXI_DAC_REG_SYNC_CONTROL		0x44
#define AXI_DAC_SYNC					BIT(0)

#define AXI_DAC_REG_RATECNTRL			0x4C
#define AXI_DAC_RATE(x)					(((x) & 0xFF) << 0)
#define AXI_DAC_TO_RATE(x)				(((x) >> 0) & 0xFF)

#define AXI_DAC_REG_CLK_FREQ			0x54
#define AXI_DAC_CLK_FREQ(x)				(((x) & 0xFFFFFFFF) << 0)
#define AXI_DAC_TO_CLK_FREQ(x)			(((x) >> 0) & 0xFFFFFFFF)

#define AXI_DAC_REG_CLK_RATIO			0x58
#define AXI_DAC_CLK_RATIO(x)			(((x) & 0xFFFFFFFF) << 0)
#define AXI_DAC_TO_CLK_RATIO(x)			(((x) >> 0) & 0xFFFFFFFF)

#define AXI_DAC_REG_STATUS				0x005C
#define AXI_DAC_MUX_PN_ERR				BIT(3)
#define AXI_DAC_MUX_PN_OOS				BIT(2)
#define AXI_DAC_MUX_OVER_RANGE			BIT(1)
#define AXI_DAC_STATUS					BIT(0)

#define AXI_DAC_REG_DDS_SCALE(x)		(0x400 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define AXI_DAC_DDS_SCALE(x)			(((x) & 0xFFFF) << 0)
#define AXI_DAC_TO_DDS_SCALE(x)			(((x) >> 0) & 0xFFFF)

#define AXI_DAC_REG_DDS_INIT_INCR(x)	(0x404 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define AXI_DAC_DDS_INIT(x)				(((x) & 0xFFFF) << 16)
#define AXI_DAC_TO_DDS_INIT(x)			(((x) >> 16) & 0xFFFF)
#define AXI_DAC_DDS_INCR(x)				(((x) & 0xFFFF) << 0)
#define AXI_DAC_TO_DDS_INCR(x)			(((x) >> 0) & 0xFFFF)

#define AXI_DAC_REG_DATA_SELECT(c)		(0x0418 + (c) * 0x40)
#define AXI_DAC_DATA_SELECT(x)			(((x) & 0xF) << 0)
#define AXI_DAC_TO_DATA_SELECT(x)		(((x) >> 0) & 0xF)

const uint32_t sine_lut_iq[1024] = {
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
	0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
	0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
	0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
	0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
	0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
	0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
	0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
	0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
	0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
	0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
	0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
	0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
	0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
	0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
	0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
	0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
	0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
	0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
	0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
	0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
	0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
	0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
	0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
	0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
	0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
	0xFA5E25FB, 0xFC3D2636, 0xFE1E265A
};

/***************************************************************************//**
 * @brief axi_dac_read
 *******************************************************************************/
int32_t axi_dac_read(struct axi_dac *dac,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
	*reg_data = Xil_In32((dac->base + reg_addr));

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_write
 *******************************************************************************/
int32_t axi_dac_write(struct axi_dac *dac,
		      uint32_t reg_addr,
		      uint32_t reg_data)
{
	Xil_Out32((dac->base + reg_addr), reg_data);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief dds_set_frequency
 *
 * freq is in Hz (i.e. set to 1*1000*1000 for 1 MHz)
 *******************************************************************************/
int32_t axi_dac_dds_set_frequency(struct axi_dac *dac,
				  uint32_t chan, uint32_t freq_hz)
{
	uint64_t val64;
	uint32_t reg;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
	val64 = (uint64_t) freq_hz * 0xFFFFULL;
	val64 = val64 / dac->clock_hz;
	reg = (reg & ~AXI_DAC_DDS_INCR(~0)) | AXI_DAC_DDS_INCR(val64) | 1;
	axi_dac_write(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), reg);
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_dds_set_phase
 *
 * phase is in milli angles scaled to 1000 (i.e. 90*1000 is 90 degrees (pi/2))
 *******************************************************************************/
int32_t axi_dac_dds_set_phase(struct axi_dac *dac,
			      uint32_t chan, uint32_t phase)
{
	uint64_t val64;
	uint32_t reg;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
	val64 = (uint64_t) phase * 0x10000ULL + (360000 / 2);
	val64 = val64 / 360000;
	reg = (reg & ~AXI_DAC_DDS_INIT(~0)) | AXI_DAC_DDS_INIT(val64);
	axi_dac_write(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), reg);
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_dds_set_scale
 *
 * scale is in micro units (i.e. 1*1000*1000 is 1.0)
 *******************************************************************************/
int32_t axi_dac_dds_set_scale(struct axi_dac *dac,
			      uint32_t chan,
			      int32_t scale_micro_units)
{
	uint32_t scale_reg;

	scale_reg = scale_micro_units;
	if (scale_micro_units < 0)
		scale_reg = scale_micro_units * -1;
	if (scale_reg >= 1999000)
		scale_reg = 1999000;
	scale_reg = (uint32_t)(((uint64_t)scale_reg * 0x4000) / 1000000);
	if (scale_micro_units < 0)
		scale_reg = scale_reg | 0x8000;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_write(dac, AXI_DAC_REG_DDS_SCALE(chan),
		      AXI_DAC_DDS_SCALE(scale_reg));
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_load_custom_data
 *******************************************************************************/
int32_t axi_dac_load_custom_data(struct axi_dac *dac,
		const uint32_t *custom_data_iq,
		uint32_t custom_tx_count,
		uint32_t address)
{
	uint32_t index;
	uint32_t index_mem = 0;
	uint8_t chan;
	uint8_t num_tx_channels = dac->num_channels / 2;

	for(index = 0; index < custom_tx_count; index++) {
		/* Send the same data on all the channels */
		for (chan = 0; chan < num_tx_channels; chan++) {
			Xil_Out32(address + index_mem * sizeof(uint32_t),
					custom_data_iq[index]);
			index_mem++;
		}
	}
	Xil_DCacheFlush();

	for (chan = 0; chan < dac->num_channels; chan++) {
		axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((chan*2)+0), 0x2);
		axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((chan*2)+1), 0x2);
	}
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_init
 *******************************************************************************/
int32_t axi_dac_init(struct axi_dac **dac_core,
		     const struct axi_dac_init *init)
{
	struct axi_dac *dac;
	uint32_t reg_data;
	uint32_t freq;
	uint32_t ratio;
	uint8_t ch;

	dac = (struct axi_dac *)malloc(sizeof(*dac));
	if (!dac)
		return FAILURE;

	dac->name = init->name;
	dac->base = init->base;
	dac->num_channels = init->num_channels;

	axi_dac_write(dac, AXI_DAC_REG_RSTN, 0);
	axi_dac_write(dac, AXI_DAC_REG_RSTN,
		      AXI_DAC_MMCM_RSTN | AXI_DAC_RSTN);

	mdelay(100);

	axi_dac_read(dac, AXI_DAC_REG_STATUS, &reg_data);
	if(reg_data == 0x0) {
		printf("%s: Status errors\n", dac->name);
		goto error;
	}

	axi_dac_write(dac, AXI_DAC_REG_RATECNTRL, AXI_DAC_RATE(3));

	axi_dac_read(dac, AXI_DAC_REG_CLK_FREQ, &freq);
	axi_dac_read(dac, AXI_DAC_REG_CLK_RATIO, &ratio);
	dac->clock_hz = freq * ratio;
	dac->clock_hz = (dac->clock_hz * 390625) >> 8;

	for (ch = 0; ch < dac->num_channels; ch++) {
		axi_dac_dds_set_frequency(dac, ((ch*2)+0), 3*1000*1000);
		axi_dac_dds_set_frequency(dac, ((ch*2)+1), 3*1000*1000);
		axi_dac_dds_set_phase(dac, ((ch*2)+0), (ch % 2) ? 0 : 90000);
		axi_dac_dds_set_phase(dac, ((ch*2)+1), (ch % 2) ? 0 : 90000);
		axi_dac_dds_set_scale(dac, ((ch*2)+0), 90*1000);
		axi_dac_dds_set_scale(dac, ((ch*2)+1), 90*1000);
		axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((ch*2)+0), 0);
		axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((ch*2)+1), 0);
	}
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	printf("%s: Successfully initialized (%"PRIu64" Hz)\n", dac->name, dac->clock_hz);

	*dac_core = dac;

	return SUCCESS;
error:
	free(dac);

	return FAILURE;
}

/***************************************************************************//**
 * @brief axi_dac_remove
 *******************************************************************************/
int32_t axi_dac_remove(struct axi_dac *dac)
{
	free(dac);

	return SUCCESS;
}
