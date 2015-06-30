/***************************************************************************//**
 *   @file   ad_fmcdaq2_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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
#include "ad9144.h"
#include "ad9523.h"
#include "ad9680.h"
#include "adc_core.h"
#include "dac_core.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"

#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID	XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_BASEADDR	XPAR_PS7_GPIO_0_BASEADDR
#else
#define SPI_DEVICE_ID	XPAR_SPI_0_DEVICE_ID
#define GPIO_BASEADDR	XPAR_GPIO_0_BASEADDR
#endif
#define AD9144_CORE_BASEADDR	XPAR_AXI_AD9144_CORE_BASEADDR
#define AD9680_CORE_BASEADDR	XPAR_AXI_AD9680_CORE_BASEADDR
#define AD9680_DMA_BASEADDR		XPAR_AXI_AD9680_DMA_BASEADDR
#define AD9144_JESD_BASEADDR	XPAR_AXI_AD9144_JESD_BASEADDR
#define AD9680_JESD_BASEADDR	XPAR_AXI_AD9680_JESD_BASEADDR
#define DAQ2_GT_BASEADDR		XPAR_AXI_DAQ2_GT_BASEADDR

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct ad9523_channel_spec ad9523_channels[] =
{
	{
		1,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		1,  //channel_divider
		"DAC_CLK", //extended_name
	},
	{
		4,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		2,  //channel_divider
		"ADC_CLK_FMC", //extended_name
	},
	{
		5,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		128,  //channel_divider
		"ADC_SYSREF", //extended_name
	},
	{
		6,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		128,  //channel_divider
		"FMC_ADC_SYSREF", //extended_name
	},
	{
		7,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		128,  //channel_divider
		"FMC_DAC_SYSREF", //extended_name
	},
	{
		8,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		128,  //channel_divider
		"DAC_SYSREF", //extended_name
	},
	{
		9,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		2,  //channel_divider
		"FMC_DAC_REF_CLK", //extended_name
	},
	{
		13,  //channel_num
		0,  //divider_output_invert_en
		0,  //sync_ignore_en
		0,  //low_power_mode_en
		0,  //use_alt_clock_src
		0,  //output_dis
		LVPECL_8mA, //driver_mode
		1,  //divider_phase
		1,  //channel_divider
		"ADC_CLK", //extended_name
	},
};

struct ad9523_platform_data ad9523_pdata_lpc =
{
	125000000, //vcxo_freq
	1,  // spi3wire

	/* Single-Ended Input Configuration */
	0,  //refa_diff_rcv_en
	0,  //refb_diff_rcv_en
	0,  //zd_in_diff_en
	1,  //osc_in_diff_en

	0,  //refa_cmos_neg_inp_en
	0,  //refb_cmos_neg_inp_en
	0,  //zd_in_cmos_neg_inp_en
	0,  //osc_in_cmos_neg_inp_en

	1,  //refa_r_div
	1,  //refb_r_div
	1,  //pll1_feedback_div
	0,  //pll1_charge_pump_current_nA
	0,  //zero_delay_mode_internal_en
	0,  //osc_in_feedback_en
	1,  //pll1_bypass_en
	1,  //pll1_loop_filter_rzero

	REVERT_TO_REFA, //ref_mode

	413000, //pll2_charge_pump_current_nA
	0,  //pll2_ndiv_a_cnt
	6,  //pll2_ndiv_b_cnt
	0,  //pll2_freq_doubler_en
	1,  //pll2_r2_div
	3,  //pll2_vco_diff_m1
	0,  //pll2_vco_diff_m2

	0,  //rpole2
	7,  //rzero
	2,  //cpole1
	0,  //rzero_bypass_en

	/* Output Channel Configuration */
	ARRAY_SIZE(ad9523_channels), //num_channels
	ad9523_channels, //channels
	"ad9523-lpc" //name
};

ad9144_init_param default_ad9144_init_param = {
	2,	// jesd_xbar_lane0_sel
	3,	// jesd_xbar_lane1_sel
	0,	// jesd_xbar_lane2_sel
	1,	// jesd_xbar_lane3_sel
};

/***************************************************************************//**
* @brief daq2_gpio_ctl
	ch2 gpios:
		trig		11
		adc_pd		10
		dac_txen	9
		dac_reset	8
		clkd_sync	6
		adc_fdb		4
		adc_fda		3
		dac_irq		2
		clkd_status_1	1
		clkd_status_0	0
*******************************************************************************/
void daq2_gpio_ctl(uint32_t base_addr)
{
#ifdef _XPARAMETERS_PS_H_
	Xil_Out32((base_addr + 0x02c4), 0x07e0); // direction (6-ctl, 5-status)
	Xil_Out32((base_addr + 0x02c8), 0x07e0); // enable
	Xil_Out32((base_addr + 0x0018), 0x0000); // mask
	Xil_Out32((base_addr + 0x004c), 0x0000); // data
	mdelay(10);
	Xil_Out32((base_addr + 0x0018), 0x07e0); // mask
	Xil_Out32((base_addr + 0x004c), 0x03e0); // data
	mdelay(10);
#else
	Xil_Out32((base_addr + 0x000c), 0x081f); // direction
	Xil_Out32((base_addr + 0x0008), 0x0340); // data
	mdelay(250);
#endif
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	jesd204b_gt_state jesd204b_gt_st;
	jesd204b_state jesd204b_st;

	daq2_gpio_ctl(GPIO_BASEADDR);

	ad9523_setup(SPI_DEVICE_ID, 0, ad9523_pdata_lpc);

	ad9144_setup(SPI_DEVICE_ID, 1, default_ad9144_init_param);

	jesd204b_st.lanesync_enable = 1;
	jesd204b_st.scramble_enable = 1;
	jesd204b_st.sysref_always_enable = 0;
	jesd204b_st.frames_per_multiframe = 32;
	jesd204b_st.bytes_per_frame = 1;
	jesd204b_st.subclass = 1;
	jesd204b_setup(AD9144_JESD_BASEADDR, jesd204b_st);

	ad9680_setup(SPI_DEVICE_ID, 2);

	jesd204b_st.lanesync_enable = 1;
	jesd204b_st.scramble_enable = 1;
	jesd204b_st.sysref_always_enable = 0;
	jesd204b_st.frames_per_multiframe = 32;
	jesd204b_st.bytes_per_frame = 1;
	jesd204b_st.subclass = 1;
	jesd204b_setup(AD9680_JESD_BASEADDR, jesd204b_st);

	jesd204b_gt_st.use_cpll = 0;
	jesd204b_gt_st.rx_sys_clk_sel = 3;
	jesd204b_gt_st.rx_out_clk_sel = 4;
	jesd204b_gt_st.tx_sys_clk_sel = 3;
	jesd204b_gt_st.tx_out_clk_sel = 4;
	jesd204b_gt_setup(DAQ2_GT_BASEADDR, jesd204b_gt_st);

	jesd204b_gt_clk_enable(JESD204B_GT_TX);
	jesd204b_gt_clk_enable(JESD204B_GT_RX);

	jesd204b_gt_clk_synchronize(JESD204B_GT_TX);
	jesd204b_gt_clk_synchronize(JESD204B_GT_RX);

	dac_setup(AD9144_CORE_BASEADDR);

	dds_set_frequency(0, 5000000);
	dds_set_phase(0, 0);
	dds_set_scale(0, 500000);
	dds_set_frequency(1, 5000000);
	dds_set_phase(1, 0);
	dds_set_scale(1, 500000);

	dds_set_frequency(2, 5000000);
	dds_set_phase(2, 90000);
	dds_set_scale(2, 500000);
	dds_set_frequency(3, 5000000);
	dds_set_phase(3, 90000);
	dds_set_scale(3, 500000);

	adc_setup(AD9680_CORE_BASEADDR, AD9680_DMA_BASEADDR, 2);

	xil_printf("Done.\n\r");

	return 0;
}
