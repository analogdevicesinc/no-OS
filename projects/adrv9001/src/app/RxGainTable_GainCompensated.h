/***************************************************************************//**
 *   @file   RxGainTable_GainCompensated.h
 *   @brief  adrv9002 RX gain compensated gain table.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef RX_GAIN_TABLE_GAIN_COMPENSATED_H
#define RX_GAIN_TABLE_GAIN_COMPENSATED_H

#include <stdint.h>

struct RxGainTable_GainCompensatedEntry {
	uint8_t gainIndex;
	uint8_t rxFeGain;
	uint8_t tiaControl;
	uint8_t adcControl;
	uint8_t extControl;
	uint16_t phaseOffset;
	int16_t digGain;
};

struct RxGainTable_GainCompensatedEntry RxGainTable_GainCompensated[] = {
	{187,251,0,0,0,0,693},
	{188,251,0,0,0,0,693},
	{189,250,0,0,0,0,656},
	{190,250,0,0,0,0,656},
	{191,250,0,0,0,0,656},
	{192,249,0,0,0,0,623},
	{193,249,0,0,0,0,623},
	{194,248,0,0,0,0,598},
	{195,248,0,0,0,0,598},
	{196,247,0,0,0,0,573},
	{197,247,0,0,0,0,573},
	{198,246,0,0,0,0,554},
	{199,246,0,0,0,0,554},
	{200,245,0,0,0,0,535},
	{201,245,0,0,0,0,535},
	{202,244,0,0,0,0,520},
	{203,243,0,0,0,0,504},
	{204,242,0,0,0,0,491},
	{205,242,0,0,0,0,490},
	{206,241,0,0,0,0,477},
	{207,240,0,0,0,0,466},
	{208,239,0,0,0,0,454},
	{209,238,0,0,0,0,444},
	{210,237,0,0,0,0,434},
	{211,236,0,0,0,0,425},
	{212,234,0,0,0,0,408},
	{213,233,0,0,0,0,400},
	{214,232,0,0,0,0,393},
	{215,230,0,0,0,0,378},
	{216,229,0,0,0,0,371},
	{217,227,0,0,0,0,359},
	{218,226,0,0,0,0,353},
	{219,224,0,0,0,0,342},
	{220,222,0,0,0,0,332},
	{221,220,0,0,0,0,322},
	{222,218,0,0,0,0,313},
	{223,215,0,0,0,0,299},
	{224,213,0,0,0,0,291},
	{225,210,0,0,0,0,279},
	{226,208,0,0,0,0,272},
	{227,205,0,0,0,0,262},
	{228,202,0,0,0,0,252},
	{229,199,0,0,0,0,243},
	{230,195,0,0,0,0,231},
	{231,192,0,0,0,0,223},
	{232,188,0,0,0,0,214},
	{233,184,0,0,0,0,204},
	{234,180,0,0,0,0,195},
	{235,175,0,0,0,0,184},
	{236,170,0,0,0,0,175},
	{237,165,0,0,0,0,165},
	{238,160,0,0,0,0,156},
	{239,154,0,0,0,0,146},
	{240,148,0,0,0,0,137},
	{241,142,0,0,0,0,128},
	{242,135,0,0,0,0,118},
	{243,128,0,0,0,0,109},
	{244,120,0,0,0,0,100},
	{245,112,0,0,0,0,90},
	{246,104,0,0,0,0,82},
	{247,94,0,0,0,0,71},
	{248,85,0,0,0,0,63},
	{249,75,0,0,0,0,54},
	{250,64,0,0,0,0,45},
	{251,53,0,0,0,0,36},
	{252,41,0,0,0,0,27},
	{253,28,0,0,0,0,18},
	{254,14,0,0,0,0,9},
	{255,0,0,0,0,0,0}
};
#endif
