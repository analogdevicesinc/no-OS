/***************************************************************************//**
 *   @file   RxGainTable.h
 *   @brief  adrv9002 RX gain table.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef RX_GAIN_TABLE_H
#define RX_GAIN_TABLE_H

#include <stdint.h>

struct RxGainTableEntry {
	uint8_t gainIndex;
	uint8_t rxFeGain;
	uint8_t tiaControl;
	uint8_t adcControl;
	uint8_t extControl;
	uint16_t phaseOffset;
	int16_t digGain;
};

struct RxGainTableEntry RxGainTable[] = {
	{183,252,0,0,0,0,25},
	{184,252,0,0,0,0,35},
	{185,251,0,0,0,0,-7},
	{186,251,0,0,0,0,3},
	{187,251,0,0,0,0,13},
	{188,251,0,0,0,0,23},
	{189,250,0,0,0,0,-4},
	{190,250,0,0,0,0,6},
	{191,250,0,0,0,0,16},
	{192,249,0,0,0,0,-7},
	{193,249,0,0,0,0,3},
	{194,248,0,0,0,0,-12},
	{195,248,0,0,0,0,-2},
	{196,247,0,0,0,0,-17},
	{197,247,0,0,0,0,-7},
	{198,246,0,0,0,0,-16},
	{199,246,0,0,0,0,-6},
	{200,245,0,0,0,0,-15},
	{201,245,0,0,0,0,-5},
	{202,244,0,0,0,0,-10},
	{203,243,0,0,0,0,-16},
	{204,242,0,0,0,0,-19},
	{205,242,0,0,0,0,-10},
	{206,241,0,0,0,0,-13},
	{207,240,0,0,0,0,-14},
	{208,239,0,0,0,0,-16},
	{209,238,0,0,0,0,-16},
	{210,237,0,0,0,0,-16},
	{211,236,0,0,0,0,-15},
	{212,234,0,0,0,0,-22},
	{213,233,0,0,0,0,-20},
	{214,232,0,0,0,0,-17},
	{215,230,0,0,0,0,-22},
	{216,229,0,0,0,0,-19},
	{217,227,0,0,0,0,-21},
	{218,226,0,0,0,0,-17},
	{219,224,0,0,0,0,-18},
	{220,222,0,0,0,0,-18},
	{221,220,0,0,0,0,-18},
	{222,218,0,0,0,0,-17},
	{223,215,0,0,0,0,-21},
	{224,213,0,0,0,0,-19},
	{225,210,0,0,0,0,-21},
	{226,208,0,0,0,0,-18},
	{227,205,0,0,0,0,-18},
	{228,202,0,0,0,0,-18},
	{229,199,0,0,0,0,-17},
	{230,195,0,0,0,0,-19},
	{231,192,0,0,0,0,-17},
	{232,188,0,0,0,0,-16},
	{233,184,0,0,0,0,-16},
	{234,180,0,0,0,0,-15},
	{235,175,0,0,0,0,-16},
	{236,170,0,0,0,0,-15},
	{237,165,0,0,0,0,-15},
	{238,160,0,0,0,0,-14},
	{239,154,0,0,0,0,-14},
	{240,148,0,0,0,0,-13},
	{241,142,0,0,0,0,-12},
	{242,135,0,0,0,0,-12},
	{243,128,0,0,0,0,-11},
	{244,120,0,0,0,0,-10},
	{245,112,0,0,0,0,-10},
	{246,104,0,0,0,0,-8},
	{247,94,0,0,0,0,-9},
	{248,85,0,0,0,0,-7},
	{249,75,0,0,0,0,-6},
	{250,64,0,0,0,0,-5},
	{251,53,0,0,0,0,-4},
	{252,41,0,0,0,0,-3},
	{253,28,0,0,0,0,-2},
	{254,14,0,0,0,0,-1},
	{255,0,0,0,0,0,0},
};
#endif
