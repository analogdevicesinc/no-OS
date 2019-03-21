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
 ******************************************************************************/
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
