/***************************************************************************//**
 *   @file   ad6676_ebz.c
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
#include <xparameters.h>
#include <xil_io.h>
#include "platform_drivers.h"
#include "ad6676.h"
#include "adc_core.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				54 + 32
#define ADC_DDR_BASEADDR		XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				32
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD6676_CORE_BASEADDR	XPAR_AXI_AD6676_CORE_BASEADDR
#define AD6676_DMA_BASEADDR		XPAR_AXI_AD6676_DMA_BASEADDR
#define AD6676_JESD_BASEADDR	XPAR_AXI_AD6676_JESD_BASEADDR
#define AD6676_GT_BASEADDR		XPAR_AXI_AD6676_GT_BASEADDR
#define GPIO_ADC_OEN			GPIO_OFFSET + 9
#define GPIO_ADC_SELA			GPIO_OFFSET + 8
#define GPIO_ADC_SELB			GPIO_OFFSET + 7
#define GPIO_ADC_S0				GPIO_OFFSET + 6
#define GPIO_ADC_S1				GPIO_OFFSET + 5
#define GPIO_ADC_RESETB			GPIO_OFFSET + 4
#define GPIO_ADC_AGC1			GPIO_OFFSET + 3
#define GPIO_ADC_AGC2			GPIO_OFFSET + 2
#define GPIO_ADC_AGC3			GPIO_OFFSET + 1
#define GPIO_ADC_AGC4			GPIO_OFFSET + 0

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
ad6676_init_param default_init_param = {
	200000000UL,	// reference_clk_rate
	0,				// spi_3_wire
	3200000000UL,	// adc_frequency_hz
	250000000UL,	// intermediate_frequency_hz
	MIN_FIF,		// intermediate_frequency_min_hz
	MAX_FIF,		// intermediate_frequency_max_hz
	75000000UL,		// bandwidth_hz
	5,				// bandwidth_margin_low_mhz
	5,				// bandwidth_margin_high_mhz
	0,				// bandwidth_margin_if_mhz
	16,				// decimation
	19,				// external_inductance_l_nh
	64,				// idac1_fullscale_adjust
	0,				// use_external_clk_enable
	1,				// adc_frequency_fixed_enable
	1,				// jesd_scrambling_enable
	1,				// jesd_use_lvds_syncb_enable
	0,				// jesd_powerdown_sysref_enable
	2,				// jesd_l_lanes
	16,				// jesd_f_frames_per_multiframe
	1,				// shuffler_control
	5,				// shuffler_thresh
	GPIO_ADC_OEN,	// gpio_adc_oen
	GPIO_ADC_SELA,	// gpio_adc_sela
	GPIO_ADC_SELB,	// gpio_adc_selb
	GPIO_ADC_S0,	// gpio_adc_s0
	GPIO_ADC_S1,	// gpio_adc_s1
	GPIO_ADC_RESETB,	// gpio_adc_resetb
	GPIO_ADC_AGC1,	// gpio_adc_agc1
	GPIO_ADC_AGC2,	// gpio_adc_agc2
	GPIO_ADC_AGC3,	// gpio_adc_agc3
	GPIO_ADC_AGC4,	// gpio_adc_agc4
};

jesd204b_state jesd204b_st = {
	1,	// lanesync_enable
	1,	// scramble_enable
	0,	// sysref_always_enable
	16,	// frames_per_multiframe
	2,	// bytes_per_frame
	1,	// subclass
};

jesd204b_gt_link gt_link = {
	AD6676_GT_BASEADDR,		// gt_core_addr
	JESD204B_GT_RX,			// tx_or_rx
	0,						// first_lane
	1,						// last_lane
	JESD204B_GT_CPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	500,					// ref_clk
	1000,					// lane_rate
	JESD204B_GT_SYSREF_INT,	// sysref_int_or_ext
	0,						// sys_clk_sel
	4,						// out_clk_sel
	0,						// gth_or_gtx
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	adc_core ad6676_core;

	jesd204b_gt_initialize(gt_link);

	ad6676_setup(SPI_DEVICE_ID,
				 GPIO_DEVICE_ID,
				 0,
				 &default_init_param);

	jesd204b_setup(AD6676_JESD_BASEADDR, jesd204b_st);

	jesd204b_gt_setup(gt_link);
	jesd204b_gt_en_sync_sysref(gt_link);

	ad6676_core.adc_baseaddr = AD6676_CORE_BASEADDR;
	ad6676_core.dmac_baseaddr = AD6676_DMA_BASEADDR;
	ad6676_core.no_of_channels = 2;
	ad6676_core.resolution = 16;

	adc_setup(ad6676_core);

	/* Enable Ramp Test Mode */
	ad6676_spi_write(AD6676_TEST_GEN, TESTGENMODE_RAMP);

	xil_printf("Start capturing data...\n\r");

	adc_capture(ad6676_core, 16384, ADC_DDR_BASEADDR);

	xil_printf("Done.\n\r");

	return 0;
}
