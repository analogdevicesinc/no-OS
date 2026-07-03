/***************************************************************************//**
 *   @file   app.h
 *   @brief  EIT application header.
 *   @author Kister Jimenez (kister.jimenez@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef APP_H_
#define APP_H_
#include <stdint.h>
#include <stdbool.h>
#include "ad5940.h"

struct eit_config {
	uint16_t nElectrodeCnt;
	uint16_t nForceDist;
	uint16_t nSenseDist;
	uint16_t nRefEl;
};

struct measurement_config {
	uint16_t nFrequency;	 // Frequency of Excitation in Khz
	uint16_t nAmplitudePP;	 // Amplitude of DAC output in mV
	bool bMagnitudeMode;	 // If true, will return magnitude
	// otherwise, will return
	// comma-separated complex numbers
	bool bImpedanceReadMode; // If true, it will measure Impedance
	// otherwise, it will measure Voltage.
	bool bSweepEn;			 // Enable Sweep Frequency
};

extern volatile uint32_t
ucInterrupted; /* Flag to indicate interrupt occurred */
int app_main();
int ADG2128_SwRst(struct ad5940_dev *dev);
uint32_t  GetMCUIntFlag(void);
uint32_t  ClrMCUIntFlag(void);
void AD5940BiaStructInit(void);
#endif /* APP_H_ */
