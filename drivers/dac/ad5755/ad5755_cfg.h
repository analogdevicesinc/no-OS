/***************************************************************************//**
 *   @file   AD5755_cfg.h
 *   @brief  Header file of AD5755 Driver Configuration.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
 *
 ******************************************************************************/
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
