/**************************************************************************//**
*   @file   AD7175_2_regs.h
*   @brief  AD7175_2 Registers Definitions.
*   @author dnechita (dan.nechita@analog.com)
*
*******************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef __AD7175_2_REGS_H__
#define __AD7175_2_REGS_H__

#include "ad717x.h"

#ifdef AD7175_2_INIT
/*! Array holding the info for the AD7175_2 registers - address, initial value, size */
ad717x_st_reg ad7175_2_regs[] =
{
	{ AD717X_STATUS_REG, 0x00, 1 },
	{ AD717X_ADCMODE_REG, 0x0000, 2	},
	{
		AD717X_IFMODE_REG, 
		AD717X_IFMODE_REG_DOUT_RESET | AD717X_IFMODE_REG_CRC_EN,
		2
	},
	{ AD717X_REGCHECK_REG, 0x0000, 3},
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
	{ AD717X_SETUPCON0_REG, 0x0000, 2 },
	{ AD717X_SETUPCON1_REG,	0x0000,	2 },
	{ AD717X_SETUPCON2_REG, 0x0000,	2 },
	{ AD717X_SETUPCON3_REG, 0x0000, 2 },
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
	{ AD717X_OFFSET0_REG, 0, 3 },
	{ AD717X_OFFSET1_REG, 0, 3 },
	{ AD717X_OFFSET2_REG, 0, 3 },
	{ AD717X_OFFSET3_REG, 0, 3 },
	{ AD717X_GAIN0_REG, 0, 3 },
	{ AD717X_GAIN1_REG, 0, 3 },
	{ AD717X_GAIN2_REG, 0, 3 },
	{ AD717X_GAIN3_REG, 0, 3 },
};
#endif

#define AD7175_2_SLAVE_ID    1

#endif /*__AD7175_2_REGS_H__ */
