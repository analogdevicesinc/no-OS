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
 *
 ******************************************************************************/
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
