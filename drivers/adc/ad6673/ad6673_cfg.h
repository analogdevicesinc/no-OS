/***************************************************************************//**
 *   @file   AD6673_cfg.h
 *   @brief  Header file of AD6673 Driver Configuration.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
