/***************************************************************************//**
*   @file   ad411x_regs.h
*   @brief  ad4111 Registers Definitions.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2018(c) Analog Devices, Inc.
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef AD4111_CFG_H_
#define AD4111_CFG_H_

#include "ad717x.h"

ad717x_st_reg ad4111_regs[] = {
	{ AD717X_STATUS_REG, 0x00, 1 },
	{
		AD717X_ADCMODE_REG,
		AD717X_ADCMODE_REG_MODE(0),
		2
	},
	{ AD717X_IFMODE_REG, 0x0000, 2 },
	{ AD717X_REGCHECK_REG, 0x0000, 3 },
	{ AD717X_DATA_REG, 0x0000, 3 },
	{
		AD717X_GPIOCON_REG,
		AD717X_GPIOCON_REG_SYNC_EN,
		2
	},
	{ AD717X_ID_REG, 0x0000, 2 },
	{ AD717X_CHMAP0_REG, 0x0000, 2 },
	{ AD717X_CHMAP1_REG, 0x0000, 2 },
	{ AD717X_CHMAP2_REG, 0x0000, 2 },
	{ AD717X_CHMAP3_REG, 0x0000, 2 },
	{ AD717X_CHMAP4_REG, 0x0000, 2 },
	{ AD717X_CHMAP5_REG, 0x0000, 2 },
	{ AD717X_CHMAP6_REG, 0x0000, 2 },
	{ AD717X_CHMAP7_REG, 0x0000, 2 },
	{ AD717X_CHMAP8_REG, 0x0000, 2 },
	{ AD717X_CHMAP9_REG, 0x0000, 2 },
	{ AD717X_CHMAP10_REG, 0x0000, 2 },
	{ AD717X_CHMAP11_REG, 0x0000, 2 },
	{ AD717X_CHMAP12_REG, 0x0000, 2 },
	{ AD717X_CHMAP13_REG, 0x0000, 2 },
	{ AD717X_CHMAP14_REG, 0x0000, 2 },
	{ AD717X_CHMAP15_REG, 0x0000, 2 },
	{ AD717X_SETUPCON0_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{ AD717X_SETUPCON1_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{ AD717X_SETUPCON2_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{ AD717X_SETUPCON3_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{ AD717X_SETUPCON4_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{ AD717X_SETUPCON5_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{ AD717X_SETUPCON6_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{ AD717X_SETUPCON7_REG, 0x0000 | AD717X_SETUP_CONF_REG_REF_SEL(2), 2 },
	{
		AD717X_FILTCON0_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{
		AD717X_FILTCON1_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{
		AD717X_FILTCON2_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{
		AD717X_FILTCON3_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{
		AD717X_FILTCON4_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{
		AD717X_FILTCON5_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{
		AD717X_FILTCON6_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{
		AD717X_FILTCON7_REG, AD717X_FILT_CONF_REG_ENHFILT(2), 2
	},
	{AD717X_OFFSET0_REG, 0, 3 },
	{AD717X_OFFSET1_REG, 0, 3 },
	{AD717X_OFFSET2_REG, 0, 3 },
	{AD717X_OFFSET3_REG, 0, 3 },
	{AD717X_OFFSET4_REG, 0, 3 },
	{AD717X_OFFSET5_REG, 0, 3 },
	{AD717X_OFFSET6_REG, 0, 3 },
	{AD717X_OFFSET7_REG, 0, 3 },
	{AD717X_GAIN0_REG, 0, 3 },
	{AD717X_GAIN1_REG, 0, 3 },
	{AD717X_GAIN2_REG, 0, 3 },
	{AD717X_GAIN3_REG, 0, 3 },
	{AD717X_GAIN4_REG, 0, 3 },
	{AD717X_GAIN5_REG, 0, 3 },
	{AD717X_GAIN6_REG, 0, 3 },
	{AD717X_GAIN7_REG, 0, 3 },
};

#endif /* AD4111_CFG_H_ */
