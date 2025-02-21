/***************************************************************************//**
 *   @file   AD9250_cfg.h
 *   @brief  Header file of AD9250 Driver Configuration.
 *   @author DNechita (Dan.Nechita@analog.com)
 ********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __AD9250_CFG_H__
#define __AD9250_CFG_H__

#include "ad9250.h"

struct ad9250_platform_data ad9250_pdata_lpc = {
	0,	// extrnPDWNmode
	1,	// enClkDCS
	0,	// clkSelection
	0,	// clkDivRatio
	0,	// clkDivPhase
	15,	// adcVref
	0,	// pllLowEncode
	"ad9250-lpc" //name
};

struct ad9250_jesd204b_cfg ad9250_jesd204b_interface = {
	0,	// jtxInStandBy
	3,	// cmlLevel
	0x22,	// quickCfgOption
	1,	// subclass
	0,	// ctrlBitsNo
	0,	// ctrlBitsAssign
	0,	// tailBitsMode
	0xF0,	// did
	0x00,	// bid
	0x00,	// lid0
	0x00,	// lid1
	32,	// k
	1,	// scrambling
	1,	// ilasMode
	0,	// invertLogicBits
	0,	// enIlasTest
	0,	// enSysRef
	0,	// enSyncInB
	0,	// sysRefMode
	1,	// alignSyncInB
	0,	// alignSysRef
	0,	// lane0Assign
	1,	// lane1Assign
};

struct ad9250_fast_detect_cfg ad9250_fast_detect = {
	0,	// enFd
	0,	// pinFunction
	0,	// forcePins
	0,	// pinForceValue
	0,	// fdUpperTresh
	0,	// fdLowerTresh
	0,	// dfDwellTime
};

#endif // __AD9250_CFG_H__
