/***************************************************************************//**
 *   @file   AD9517_cfg.h
 *   @brief  Header file of AD9517 Driver Configuration.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#ifndef __AD9517_CFG_H__
#define __AD9517_CFG_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
//#include "AD9517.h"

struct ad9517_platform_data ad9517_pdata_lpc = {
	/* PLL Reference */
	250000000, // ref_1_freq
	250000000, // ref_2_freq
	1, // diff_ref_en
	1, // ref_1_power_on
	1, // ref_2_power_on
	0, // ref_sel_pin_en
	1, // ref_sel_pin
	0, // ref_2_en

	250000000, // ext_clk_freq
	1600000000, // int_vco_freq
	0, // vco_clk_sel
	0, // power_down_vco_clk
	"ad9517-lpc" // name[16]
};

struct ad9517_lvpecl_channel_spec ad9517_lvpecl_channels[] = {
	{
		0, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_780mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH0" // name[16] - Optional descriptive channel name.
	},
	{
		1, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_780mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH1" // name[16] - Optional descriptive channel name.
	},
	{
		2, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_780mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH2" // name[16] - Optional descriptive channel name.
	},
	{
		3, // channel_num - Output channel number.
		0, // out_invert_en - Invert the polarity of the output clock.
		LVPECL_960mV, // out_diff_voltage - LVPECL output differential voltage.
		"CH3" // name[16] - Optional descriptive channel name.
	}
};

struct ad9517_lvds_cmos_channel_spec ad9517_lvds_cmos_channels[] = {
	{
		4, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		0, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH4" // name[16] - Optional descriptive channel name.
	},
	{
		5, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		0, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH5" // name[16] - Optional descriptive channel name.
	},
	{
		6, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		1, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH6" // name[16] - Optional descriptive channel name.
	},
	{
		7, // channel_num - Output channel number.
		0, // out_invert
		LVDS, // logic_level - Select LVDS or CMOS logic levels.
		0, // cmos_b_en - In CMOS mode, turn on/off the CMOS B output.
		LVDS_3_5mA, // out_lvds_current - LVDS output current level.
		"CH7" // name[16] - Optional descriptive channel name.
	}
};

#endif // __AD9517_CFG_H__
