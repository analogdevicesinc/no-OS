/**************************************************************************//**
*   @file   AD7172_2_regs.h
*   @brief  AD7172_2 Registers Definitions.
*   @author dnechita (dan.nechita@analog.com)
*
*******************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AD7172_2_REGS_H__
#define __AD7172_2_REGS_H__

#include "ad717x.h"

#ifdef AD7172_2_INIT
/*! Array holding the info for the AD7172_2 registers - address, initial value, size */
ad717x_st_reg ad7172_2_regs[] = {
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

#define AD7172_2_SLAVE_ID    1

#endif /*__AD7172_2_REGS_H__ */
