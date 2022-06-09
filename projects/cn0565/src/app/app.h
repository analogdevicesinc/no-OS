/***************************************************************************//**
 *   @file   app.h
 *   @brief  EIT application header.
 *   @author Kister Jimenez (kister.jimenez@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
