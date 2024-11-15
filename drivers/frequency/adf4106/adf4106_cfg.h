/***************************************************************************//**
 *   @file   ADF4106_cfg.h
 *   @brief  Header file of ADF4106 Driver Configuration. Here can be defined
 *           the register set ups which going to be used in the initialization
 *           phase
 *
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __ADF4106_CFG_H__
#define __ADF4106_CFG_H__

struct adf4106_settings_t adf4106_st = {
	10000000,                       // RFIN is 10Mhz
	200000,                         // User defined PFD max frequency
	10,                             // refCounter
	ADF4106_R_ABP_2_9NS,            // Anti-backlash Pulse Width options
	ADF4106_R_TMB_NORMAL,           // Test Mode Bits
	ADF4106_R_LDP_5,                // Lock Detect Precision
	4,                              // N Counter Latch A
	2,                              // N Counter Latch B
	ADF4106_N_CP_GAIN_1,            // Charge Pump Gain Settings
	ADF4106_CR_NORMAL,              // Counter Reset Bit
	ADF4106_PD1_NORMAL,             // Power Down Bit 1
	ADF4106_MUXOUT_NDIV_OUTPUT,     // Muxout Control
	ADF4106_PDPOL_NEGATIVE,         // Phase Detector Polarity
	ADF4106_CP_NORMAL,              // Charge Pump Output
	ADF4106_FASTLOCK_MODE1,         // Fastlock Mode
	ADF4106_TCC_3,                  // Timer Counter Control
	ADF4106_CS_2_5,                 // Current Setting 1
	ADF4106_CS_2_5,                 // Current Setting 2
	ADF4106_SYNC_PWD,               // Synchronous or asynchronous power down
	ADF4106_PS_8_9                  // Prescaler value
};

#endif // __ADF4106_CFG_H__
