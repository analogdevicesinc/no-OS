/***************************************************************************//**
 *   @file   ORxGainTable.h
 *   @brief  adrv9002 Observation RX gain table.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
	{2,248,0,0,0,0,-2},
	{3,248,0,0,0,0,-2},
	{4,242,0,0,0,0,-10},
	{5,242,0,0,0,0,-10},
	{6,230,0,0,0,0,-22},
	{7,230,0,0,0,0,-22},
	{8,210,0,0,0,0,-20},
	{9,210,0,0,0,0,-20},
	{10,175,0,0,0,0,-16},
	{11,175,0,0,0,0,-16},
	{12,112,0,0,0,0,-10},
	{13,112,0,0,0,0,-10},
	{14,0,0,0,0,0,0},
};
#endif
