/***************************************************************************//**
 *   @file   ADF4153_cfg.h
 *   @brief  Header file of ADF4153 Driver Configuration. Here can be defined
 *           the register set ups which going to be used in the initialization
 *           phase
 *
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __ADF4153_CFG_H__
#define __ADF4153_CFG_H__

struct adf4153_settings_t adf4153_st = {
	25000000,                           // RFIN is 25Mhz
	150000,                             // Channel spacing is 150 Khz
	10,                                 // 12-bit fractional value
	8,                                  // 9-bit integer value
	ADF4153_FASTLOCK_DISABLED,          // Fast-Lock
	16,                                 // 12-bit interpolator modulus value
	7,                                  // 4-bit R Counter
	ADF4153_PRESCALER_4_5,              // Prescale
	ADF4153_MUXOUT_LOGICHIGH,           // Muxout
	ADF4153_LOAD_NORMAL,                // Load Control
	ADF4153_CR_DISABLED,                // Counter Reset
	ADF4153_CP_DISABLED,                // CP Three-state
	ADF4153_PD_DISABLED,                // Power-down
	ADF4153_LDP_24,                     // LDP
	ADF4153_PD_POL_POSITIVE,            // PD Polarity
	ADF4153_CP_CURRENT_0_63,            // Current settings
	ADF4153_REF_DOUBLER_DIS,            // Reference doubler
	1,                                  // Resync
	ADF4153_LOW_SPUR_MODE               // Noise and Spur mode
};

#endif // __ADF4153_CFG_H__
