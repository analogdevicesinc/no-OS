/**************************************************************************//**
*   @file   AD7175_8_regs.h
*   @brief  AD7175_8 Registers Definitions.
*   @author dnechita (dan.nechita@analog.com)
*
*******************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*****************************************************************************/

#ifndef __AD7175_8_REGS_H__
#define __AD7175_8_REGS_H__

#include "ad717x.h"

#ifdef AD7175_8_INIT
/*! Array holding the info for the AD7175_8 registers - address, initial value, size */
ad717x_st_reg ad7175_8_regs[] = {
	{ AD717X_STATUS_REG, 0x00, 1 },
	{ AD717X_ADCMODE_REG, 0x0000, 2 },
	{
		AD717X_IFMODE_REG,
		AD717X_IFMODE_REG_DOUT_RESET | AD717X_IFMODE_REG_CRC_EN,
		2
	},
	{ AD717X_REGCHECK_REG, 0x0000, 3 },
	{ AD717X_DATA_REG, 0x0000, 3 },
	{ AD717X_GPIOCON_REG, 0x0000, 2 },
	{ AD717X_ID_REG, 0x0000, 2 },
	{
		AD717X_CHMAP0_REG,
		AD717X_CHMAP_REG_CH_EN | AD717X_CHMAP_REG_AINNEG(2),
		2
	},
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
	{ AD717X_SETUPCON0_REG, 0x0000, 2 },
	{ AD717X_SETUPCON1_REG, 0x0000, 2 },
	{ AD717X_SETUPCON2_REG, 0x0000, 2 },
	{ AD717X_SETUPCON3_REG, 0x0000, 2 },
	{ AD717X_SETUPCON4_REG, 0x0000, 2 },
	{ AD717X_SETUPCON5_REG, 0x0000, 2 },
	{ AD717X_SETUPCON6_REG, 0x0000, 2 },
	{ AD717X_SETUPCON7_REG, 0x0000, 2 },
	{
		AD717X_FILTCON0_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2) | AD717X_FILT_CONF_REG_ODR(10),
		2
	},
	{
		AD717X_FILTCON1_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2),
		2
	},
	{
		AD717X_FILTCON2_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2),
		2
	},
	{
		AD717X_FILTCON3_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2),
		2
	},
	{
		AD717X_FILTCON4_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2),
		2
	},
	{
		AD717X_FILTCON5_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2),
		2
	},
	{
		AD717X_FILTCON6_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2),
		2
	},
	{
		AD717X_FILTCON7_REG,
		AD717X_FILT_CONF_REG_ENHFILT(2),
		2
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
#endif

#define AD7175_8_SLAVE_ID    1

#endif /*__AD7175_8_REGS_H__ */
