/***************************************************************************//**
 * @file dac_core.c
 * @brief Implementation of DAC Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <inttypes.h>
#include "sys/alt_cache.h"
#include "mykonos.h"
#include "dac_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
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
 * @brief dac_read
 *******************************************************************************/
int32_t dac_read(dac_core *core,
				 uint32_t reg_addr,
				 uint32_t *reg_data)
{
	*reg_data = IORD_32DIRECT(core->base_address + 0x4000, reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief dac_write
 *******************************************************************************/
int32_t dac_write(dac_core *core,
				  uint32_t reg_addr,
				  uint32_t reg_data)
{
	IOWR_32DIRECT(core->base_address + 0x4000, reg_addr, reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief dac_dma_read
*******************************************************************************/
void dac_dma_read(uint32_t regAddr, uint32_t *data)
{
	*data = IORD_32DIRECT(AXI_AD9371_TX_DMA_BASE, regAddr);
}

/***************************************************************************//**
 * @brief dac_dma_write
*******************************************************************************/
void dac_dma_write(uint32_t regAddr, uint32_t data)
{
	IOWR_32DIRECT(AXI_AD9371_TX_DMA_BASE, regAddr, data);
}


/***************************************************************************//**
 * @brief dds_set_frequency
 *******************************************************************************/

// freq is in Hz (i.e. set to 1*1000*1000 for 1 MHz)

int32_t dds_set_frequency(dac_core *core,
						  uint32_t chan,
						  uint32_t freq)
{
	uint64_t val64;
	uint32_t reg;

	dac_write(core, DAC_REG_SYNC_CONTROL, 0);
	dac_read(core, DAC_REG_DDS_INIT_INCR(chan), &reg);
	val64 = (uint64_t) freq * 0xFFFFULL;
	val64 = val64 / core->clock;
	reg = (reg & ~DAC_DDS_INCR(~0)) | DAC_DDS_INCR(val64) | 1;
	dac_write(core, DAC_REG_DDS_INIT_INCR(chan), reg);
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);

	return 0;
}

/***************************************************************************//**
 * @brief dds_set_phase
 *******************************************************************************/

// phase is in milli(?) angles scaled to 1000 (i.e. 90*1000 is 90 degrees (pi/2))

int32_t dds_set_phase(dac_core *core,
					  uint32_t chan,
					  uint32_t phase)
{
	uint64_t val64;
	uint32_t reg;

	dac_write(core, DAC_REG_SYNC_CONTROL, 0);
	dac_read(core, DAC_REG_DDS_INIT_INCR(chan), &reg);
	val64 = (uint64_t) phase * 0x10000ULL + (360000 / 2);
	val64 = val64 / 360000;
	reg = (reg & ~DAC_DDS_INIT(~0)) | DAC_DDS_INIT(val64);
	dac_write(core, DAC_REG_DDS_INIT_INCR(chan), reg);
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);

	return 0;
}

/***************************************************************************//**
 * @brief dds_set_scale
 *******************************************************************************/

// scale is in micro units (i.e. 1*1000*1000 is 1.0)
// scale range is (-1.99 to +1.99) - hardware supports -2.0, but we ignore it.
// do not get fancy, above 1.0 (or below -1.0) will mess the dds tones.
// dds always runs 16bits-- so unless your data path handles it- stay within -1 to +1.

int32_t dds_set_scale(dac_core *core,
					  uint32_t chan,
					  int32_t scale_micro_units)
{
	uint32_t pcore_version;
	uint32_t scale_reg;

	dac_read(core, DAC_REG_VERSION, &pcore_version);

	// only ise projects support binary shift scaling, if you think you need
	// this supported in this driver, let us know.

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 6)
	{
		printf("%s ERROR: Sorry, binary scale is NOT supported!\n", __func__);
		return(-1);
	}

	scale_reg = scale_micro_units;
	if (scale_micro_units < 0)
		scale_reg = scale_micro_units * -1;
	if (scale_reg >= 1999000)
		scale_reg = 1999000;
	scale_reg = (uint32_t)(((uint64_t)scale_reg * 0x4000) / 1000000);
	if (scale_micro_units < 0)
		scale_reg = scale_reg | 0x8000;

	dac_write(core, DAC_REG_SYNC_CONTROL, 0);
	dac_write(core, DAC_REG_DDS_SCALE(chan), DAC_DDS_SCALE(scale_reg));
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);
	return(0);
}

/***************************************************************************//**
 * @brief dac_data_src_sel
 *******************************************************************************/

int32_t dac_data_src_sel(dac_core *core,
						 int32_t chan,
						 dac_data_src src)
{
	uint32_t pcore_version;
	uint32_t reg;
	int32_t i;

	dac_read(core, DAC_REG_VERSION, &pcore_version);

	// single core control for all channels

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 7)
	{
		dac_read(core, DAC_REG_DATA_CONTROL, &reg);
		reg = (reg & ~DAC_DATA_SEL(~0)) | DAC_DATA_SEL(src);
		dac_write(core, DAC_REG_DATA_CONTROL, reg);
		return(0);
	}

	// per channel source select

	for (i = 0; i < (core->no_of_channels * 2); i++)
	{
		if ((chan < 0) || (chan == i))
			dac_write(core, DAC_REG_DATA_SELECT(i), src);
	}
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);

	return(0);
}

/***************************************************************************//**
 * @brief dac_setup
 *******************************************************************************/
int32_t dac_setup(mykonosDevice_t *myk_dev,
				  dac_core *core)
{
	uint32_t reg_data;

	if (core->plddr_bypass_gpio) {
		/*
		 * The default HDL design features a optional high bandwidth DDR FIFO,
		 * it can be bypassed using this fabric internal GPIO
		 */
		gpio_set_value(core->plddr_bypass_gpio, !(core->dma_type == DMA_PLDDR_FIFO));
	}

	dac_write(core, DAC_REG_RSTN, 0x00);
	dac_write(core, DAC_REG_RSTN, 0x03);
	mdelay(100);

	dac_read(core, DAC_REG_STATUS, &reg_data);
	if (reg_data == 0x0) {
		printf("%s DAC Core Status errors.\n", __func__);
		return -1;
	}

	core->clock = myk_dev->tx->txProfile->iqRate_kHz * 1000;

	dac_write(core, DAC_REG_RATECNTRL, DAC_RATE(3));

	printf("%s dac core initialized (%"PRIu64" MHz).\n",
			__func__, core->clock / 1000000);

	dac_data_setup(core);

	return 0;
}

/***************************************************************************//**
 * @brief dac_setup
 *******************************************************************************/

int32_t dac_data_setup(dac_core *core)
{
	dac_channel *chan;
	uint32_t i;

	for (i = 0; i < core->no_of_channels; i++) {
		chan = &core->channels[i];
		if (chan->sel == DAC_SRC_DDS)
		{
			dds_set_frequency(core, ((i*2)+0), chan->dds_frequency_tone0);
			dds_set_phase(core, ((i*2)+0), chan->dds_phase_tone0);
			dds_set_scale(core, ((i*2)+0), chan->dds_scale_tone0);
			if (chan->dds_dual_tone == 0)
			{
				dds_set_frequency(core, ((i*2)+1), chan->dds_frequency_tone0);
				dds_set_phase(core, ((i*2)+1), chan->dds_phase_tone0);
				dds_set_scale(core, ((i*2)+1), chan->dds_scale_tone0);
			}
			else
			{
				dds_set_frequency(core, ((i*2)+1), chan->dds_frequency_tone1);
				dds_set_phase(core, ((i*2)+1), chan->dds_phase_tone1);
				dds_set_scale(core, ((i*2)+1), chan->dds_scale_tone1);
			}
		}
		dac_write(core, DAC_REG_DATA_PATTERN(i), chan->pat_data);
		dac_data_src_sel(core, i, chan->sel);
	}

	return(0);
}

/***************************************************************************//**
 * @brief dac_write_custom_data
*******************************************************************************/
void dac_write_custom_data(dac_core *core,
						   const uint32_t *custom_data_iq,
						   uint32_t custom_tx_count)
{
	uint32_t index;
	uint32_t index_mem = 0;
	uint32_t length;
	uint32_t dmac_flags;
	uint8_t chan;
	uint8_t num_tx_channels = core->no_of_channels / 2;

	for(index = 0; index < custom_tx_count; index++)
	{
		for (chan = 0; chan < num_tx_channels; chan++) // send the same data on all the channels
		{
			IOWR_32DIRECT(core->dac_ddr_baseaddr, index_mem * sizeof(uint32_t), custom_data_iq[index]);
			index_mem++;
		}
	}
	alt_icache_flush((void*)core->dac_ddr_baseaddr, index_mem * sizeof(uint32_t));

	length = index_mem * 4;

	switch (core->dma_type)
	{
	case DMA_STREAM:
		dmac_flags = 0;
		break;
	case DMA_CYCLIC:
		dmac_flags = AXI_DMAC_FLAG_CYCLIC;
		break;
	case DMA_PLDDR_FIFO:
		dmac_flags = AXI_DMAC_FLAG_LAST;
		break;
	}

	dac_dma_write(AXI_DMAC_REG_CTRL, 0);
	dac_dma_write(AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);
	dac_dma_write(AXI_DMAC_REG_FLAGS, dmac_flags);
	dac_dma_write(AXI_DMAC_REG_SRC_ADDRESS, core->dac_ddr_baseaddr);
	dac_dma_write(AXI_DMAC_REG_SRC_STRIDE, 0x0);
	dac_dma_write(AXI_DMAC_REG_X_LENGTH, length - 1);
	dac_dma_write(AXI_DMAC_REG_Y_LENGTH, 0x0);
	dac_dma_write(AXI_DMAC_REG_START_TRANSFER, 0x1);
}
