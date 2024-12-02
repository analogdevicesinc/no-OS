/***************************************************************************//**
 *   @file   ORxGainTable.h
 *   @brief  adrv9002 Observation RX gain table.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef ORX_GAIN_TABLE_H
#define ORX_GAIN_TABLE_H

#include <stdint.h>

struct ORxGainTableEntry {
	uint8_t gainIndex;
	uint8_t rxFeGain;
	uint8_t tiaControl;
	uint8_t adcControl;
	uint8_t extControl;
	uint16_t phaseOffset;
	int16_t digGain;
};

struct ORxGainTableEntry ORxGainTable[] = {
	{2, 248, 0, 0, 0, 0, -2},
	{3, 248, 0, 0, 0, 0, -2},
	{4, 242, 0, 0, 0, 0, -10},
	{5, 242, 0, 0, 0, 0, -10},
	{6, 230, 0, 0, 0, 0, -22},
	{7, 230, 0, 0, 0, 0, -22},
	{8, 210, 0, 0, 0, 0, -20},
	{9, 210, 0, 0, 0, 0, -20},
	{10, 175, 0, 0, 0, 0, -16},
	{11, 175, 0, 0, 0, 0, -16},
	{12, 112, 0, 0, 0, 0, -10},
	{13, 112, 0, 0, 0, 0, -10},
	{14, 0, 0, 0, 0, 0, 0},
};
#endif
