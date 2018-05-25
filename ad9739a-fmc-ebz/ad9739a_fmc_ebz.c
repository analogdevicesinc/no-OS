/***************************************************************************//**
 *   @file   ad9739a_fmc_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"
#include "dac_core.h"
#include "ad9739a.h"
#include "adf4350.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/

adf4350_init_param default_adf4350_init_param = {
	/* SPI */
	{
		ZYNQ_PS7_SPI,	// type
		0,				// chip_select
		0,				// cpha
		0,				// cpol
	},

	/* Device settings */
	25000000,		// clkin;
	10000,			// channel_spacing;
	2500000000ul,		// power_up_frequency;
	0,			// reference_div_factor;
	0,			// reference_doubler_enable;
	0,			// reference_div2_enable;

	/* r2_user_settings */
	1,			// phase_detector_polarity_positive_enable;
	0,			// lock_detect_precision_6ns_enable;
	0,			// lock_detect_function_integer_n_enable;
	2500,			// charge_pump_current;
	0,			// muxout_select;
	0,			// low_spur_mode_enable;

	/* r3_user_settings */
	0,			// cycle_slip_reduction_enable;
	0,			// charge_cancellation_enable;
	0,			// anti_backlash_3ns_enable;
	0,			// band_select_clock_mode_high_enable;
	0,			// clk_divider_12bit;
	0,			// clk_divider_mode;

	/* r4_user_settings */
	0,			// aux_output_enable;
	1,			// aux_output_fundamental_enable;
	0,			// mute_till_lock_enable;
	3,			// output_power;
	0,			// aux_output_power;
};


/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	spi_init_param		spi_param;
	struct ad9739a_dev	*ad9739a_device;
	adf4350_dev			*adf4350_device;
	dac_core 			ad9739a_core;
	dac_channel			ad9739a_channel[1];
	struct ad9739a_init_param	init_param;

	spi_param.type = ZYNQ_PS7_SPI;
	spi_param.chip_select = 0x1;
	spi_param.cpha = 0;
	spi_param.cpol = 0;

	/* SPI */
	init_param.spi_init = spi_param;
	/* Device settings */
	init_param.common_mode_voltage_dacclk_p = 0xF;	// common_mode_voltage_dacclk_p
	init_param.common_mode_voltage_dacclk_n = 0xF;	// common_mode_voltage_dacclk_n
	init_param.full_scale_current = 20.0;		// full_scale_current

	ad9739a_core.base_address = XPAR_AXI_AD9739A_BASEADDR;
	ad9739a_core.resolution = 16;
	ad9739a_core.no_of_channels = 1;
	ad9739a_core.channels = &ad9739a_channel[0];

	adf4350_setup(&adf4350_device, default_adf4350_init_param);

	dac_write(&ad9739a_core, DAC_REG_DATA_CONTROL, DAC_DATA_FORMAT);

	ad9739a_channel[0].dds_dual_tone = 0;
	ad9739a_channel[0].dds_frequency_0 = 33*1000*1000;
	ad9739a_channel[0].dds_scale_0 = 250000;
	ad9739a_channel[0].dds_phase_0 = 0;
	ad9739a_channel[0].sel = DAC_SRC_DDS;

	dac_setup(&ad9739a_core);

	ad9739a_setup(&ad9739a_device, init_param);

	printf("Done.\n");

	return 0;
}
