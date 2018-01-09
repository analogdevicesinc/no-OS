/***************************************************************************//**
 *   @file   AD6673_cfg.h
 *   @brief  Header file of AD6673 Driver Configuration.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __AD6673_CFG_H__
#define __AD6673_CFG_H__

/******************************************************************************/
/****************************** Include Files *********************************/
/******************************************************************************/
#include "ad6673.h"

/******************************************************************************/
/************************ Variables Declarations ******************************/
/******************************************************************************/
struct ad6673_platform_data ad6673_pdata_lpc = {
	0,           // extrnPDWNmode
	1,           // enClkDCS
	0,           // clkSelection
	0,           // clkDivRatio
	0,           // clkDivPhase
	15,          // adcVref
	0,           // pllLowEncode
	"ad6673-lpc" //name
};

struct ad6673_jesd204b_cfg ad6673_jesd204b_interface = {
	0,      // jtxInStandBy
	3,      // cmlLevel
	0x22,   // quickCfgOption
	1,      // subclass
	0,      // ctrlBitsNo
	0,      // ctrlBitsAssign
	0,      // tailBitsMode
	0xF0,   // did
	0x00,   // bid
	0x00,   // lid0
	0x00,   // lid1
	32,     // k
	1,      // scrambling
	1,      // ilasMode
	0,      // invertLogicBits
	0,      // enIlasTest
	0,      // enSysRef
	0,      // enSyncInB
	0,      // sysRefMode
	1,      // alignSyncInB
	0,      // alignSysRef
	0,      // lane0Assign
	1,      // lane1Assign
};

struct ad6673_fast_detect_cfg ad6673_fast_detect = {
	0,      // enFd
	0,      // pinFunction
	0,      // forcePins
	0,      // pinForceValue
	0,      // fdUpperTresh
	0,      // fdLowerTresh
	0,      // dfDwellTime
};

#endif // __AD6673_CFG_H__
