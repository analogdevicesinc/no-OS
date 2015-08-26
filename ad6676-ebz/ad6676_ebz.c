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
#define GPIO_BASEADDR			XPAR_PS7_GPIO_0_BASEADDR
#define ADC_DDR_BASEADDR		XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define GPIO_BASEADDR			XPAR_GPIO_0_BASEADDR
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD6676_CORE_BASEADDR	XPAR_AXI_AD6676_CORE_BASEADDR
#define AD6676_DMA_BASEADDR		XPAR_AXI_AD6676_DMA_BASEADDR
#define AD6676_JESD_BASEADDR	XPAR_AXI_AD6676_JESD_BASEADDR
#define AD6676_GT_GT_BASEADDR	XPAR_AXI_AD6676_GT_BASEADDR

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
};

/***************************************************************************//**
* @brief ad6676_ebz_gpio_ctl
*			gpios:
*				adc_oen		9	- 0
*				adc_sela	8	- 0
*				adc_selb	7	- 1
*				adc_s0		6	- 0
*				adc_s1		5	- 1
*				adc_resetb	4	- 1
*				adc_agc1	3	- by default input - 0
*				adc_agc2	2	- by default input - 0
*				adc_agc3	1	- by default output
*				adc_agc4	0	- by default output
*******************************************************************************/
void ad6676_ebz_gpio_ctl(uint32_t base_addr)
{
#ifdef _XPARAMETERS_PS_H_
	Xil_Out32((base_addr + 0x02c4), 0x03ff); // direction (6-ctl, 5-status)
	Xil_Out32((base_addr + 0x02c8), 0x03ff); // enable
	Xil_Out32((base_addr + 0x0018), 0x0000); // mask
	Xil_Out32((base_addr + 0x004c), 0x0000); // data
	mdelay(10);

	Xil_Out32((base_addr + 0x0018), 0x03ff); // mask
	Xil_Out32((base_addr + 0x004c), 0x00b0); // data
#else
	Xil_Out32((base_addr + 0x000c), 0x0003); // direction
	Xil_Out32((base_addr + 0x0008), 0x00b0); // data
#endif
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	jesd204b_gt_state jesd204b_gt_st;
	jesd204b_state jesd204b_st;

	ad6676_ebz_gpio_ctl(GPIO_BASEADDR);

	ad6676_setup(SPI_DEVICE_ID, 0, &default_init_param);

	jesd204b_gt_st.use_cpll = 1;
	jesd204b_gt_st.rx_sys_clk_sel = 0;
	jesd204b_gt_st.rx_out_clk_sel = 4;
	jesd204b_gt_st.tx_sys_clk_sel = 0;
	jesd204b_gt_st.tx_out_clk_sel = 4;
	jesd204b_gt_setup(AD6676_GT_GT_BASEADDR, jesd204b_gt_st);

	jesd204b_st.lanesync_enable = 1;
	jesd204b_st.scramble_enable = 1;
	jesd204b_st.sysref_always_enable = 0;
	jesd204b_st.frames_per_multiframe = 16;
	jesd204b_st.bytes_per_frame = 2;
	jesd204b_st.subclass = 1;
	jesd204b_setup(AD6676_JESD_BASEADDR, jesd204b_st);

	jesd204b_gt_clk_enable(JESD204B_GT_RX);

	adc_setup(AD6676_CORE_BASEADDR, AD6676_DMA_BASEADDR,  2);

	/* Enable Ramp Test Mode */
	ad6676_spi_write(AD6676_TEST_GEN, TESTGENMODE_RAMP);

	xil_printf("Start capturing data...\n\r");
	adc_capture(16384, ADC_DDR_BASEADDR);
	xil_printf("Done.\n\r");

	return 0;
}
