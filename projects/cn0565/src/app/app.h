/***************************************************************************//**
 *   @file   app.h
 *   @brief  EIT application header.
 *   @author Kister Jimenez (kister.jimenez@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
