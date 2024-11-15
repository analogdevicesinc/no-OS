/***************************************************************************//**
 *   @file   AD5755_cfg.h
 *   @brief  Header file of AD5755 Driver Configuration.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __AD5755_CFG_H__
#define __AD5755_CFG_H__

struct ad5755_setup AD5755_st = {
	0,                          //DUT_AD0
	0,                          //DUT_AD1
	1,                          //enablePacketErrorCheck
	0,                          //pocBit
	1,                          //statReadBit
	0,                          //shtCcLimBit
	{0, 0, 0, 0},               //{rsetChA, rsetChB, rsetChC, rsetChD}
	{0, 0, 0, 0},               //{ovrngBitA, ovrngBitB, ovrngBitC, ovrngBitD}
	0,                          //dcDcCompBit
	AD5755_PHASE_ALL_DC_DC,     //dcDcPhaseBit
	AD5755_FREQ_410_HZ,         //dcDcFreqBit
	AD5755_MAX_23V              //dcDcMaxVBit
};

#endif // __AD5755_CFG_H__
