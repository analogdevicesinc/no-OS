/***************************************************************************//**
 *   @file   ad_fmcdaq2_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
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
#include <xil_cache.h>
#include <xil_io.h>
#include "platform_drivers.h"
#include "ad9144.h"
#include "ad9523.h"
#include "ad9680.h"
#include "adc_core.h"
#include "dac_core.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				54 + 32
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000
#else
#define GPIO_DEVICE_ID			XPAR_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				32
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD9144_CORE_BASEADDR	XPAR_AXI_AD9144_CORE_BASEADDR
#define AD9680_CORE_BASEADDR	XPAR_AXI_AD9680_CORE_BASEADDR
#define AD9680_DMA_BASEADDR		XPAR_AXI_AD9680_DMA_BASEADDR
#define AD9144_JESD_BASEADDR	XPAR_AXI_AD9144_JESD_BASEADDR
#define AD9680_JESD_BASEADDR	XPAR_AXI_AD9680_JESD_BASEADDR
#define DAQ2_GT_BASEADDR		XPAR_AXI_DAQ2_GT_BASEADDR
#define GPIO_CLKD_STATUS_0		GPIO_OFFSET + 0
#define GPIO_CLKD_STATUS_1		GPIO_OFFSET + 1
#define GPIO_DAC_IRQ			GPIO_OFFSET + 2
#define GPIO_ADC_FDA			GPIO_OFFSET + 3
#define GPIO_ADC_FDB			GPIO_OFFSET + 4
#define GPIO_CLKD_SYNC			GPIO_OFFSET + 6
#define GPIO_DAC_RESET			GPIO_OFFSET + 8
#define GPIO_DAC_TXEN			GPIO_OFFSET + 9
#define GPIO_ADC_PD				GPIO_OFFSET + 10
#define GPIO_TRIG				GPIO_OFFSET + 11

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

jesd204b_state jesd204b_st = {
	1,	// lanesync_enable
	1,	// scramble_enable
	0,	// sysref_always_enable
	32,	// frames_per_multiframe
	1,	// bytes_per_frame
	1,	// subclass
};

jesd204b_gt_link ad9144_gt_link = {
	DAQ2_GT_BASEADDR,		// gt_core_addr
	JESD204B_GT_TX,			// tx_or_rx
	0,						// first_lane
	3,						// last_lane
	JESD204B_GT_QPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	500,					// ref_clk
	1000,					// lane_rate
	JESD204B_GT_SYSREF_EXT,	// sysref_int_or_ext
	3,						// sys_clk_sel
	4,						// out_clk_sel
	0,						// gth_or_gtx
};

jesd204b_gt_link ad9680_gt_link = {
	DAQ2_GT_BASEADDR,		// gt_core_addr
	JESD204B_GT_RX,			// tx_or_rx
	0,						// first_lane
	3,						// last_lane
	JESD204B_GT_QPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	500,					// ref_clk
	1000,					// lane_rate
	JESD204B_GT_SYSREF_EXT,	// sysref_int_or_ext
	3,						// sys_clk_sel
	4,						// out_clk_sel
	0,						// gth_or_gtx
};

/***************************************************************************//**
* @brief daq2_gpio_ctl
*******************************************************************************/
void daq2_gpio_ctl(uint32_t gpio_device_id)
{
	gpio_init(gpio_device_id);

	gpio_direction(GPIO_CLKD_STATUS_0, GPIO_INPUT);
	gpio_direction(GPIO_CLKD_STATUS_1, GPIO_INPUT);
	gpio_direction(GPIO_DAC_IRQ, GPIO_INPUT);
	gpio_direction(GPIO_ADC_FDA, GPIO_INPUT);
	gpio_direction(GPIO_ADC_FDB, GPIO_INPUT);
	gpio_direction(GPIO_CLKD_SYNC, GPIO_OUTPUT);
	gpio_direction(GPIO_DAC_RESET, GPIO_OUTPUT);
	gpio_direction(GPIO_DAC_TXEN, GPIO_OUTPUT);
	gpio_direction(GPIO_ADC_PD, GPIO_OUTPUT);
	gpio_direction(GPIO_TRIG, GPIO_INPUT);

	gpio_set_value(GPIO_CLKD_SYNC, GPIO_HIGH);
	gpio_set_value(GPIO_DAC_RESET, GPIO_HIGH);
	gpio_set_value(GPIO_DAC_TXEN, GPIO_HIGH);
	gpio_set_value(GPIO_ADC_PD, GPIO_LOW);

	mdelay(250);
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	adc_core ad9680_core;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	daq2_gpio_ctl(GPIO_DEVICE_ID);

	ad9523_setup(SPI_DEVICE_ID, 0, ad9523_pdata_lpc);
	jesd204b_gt_initialize(ad9144_gt_link);

	ad9144_setup(SPI_DEVICE_ID, 1, default_ad9144_init_param);
	jesd204b_setup(AD9144_JESD_BASEADDR, jesd204b_st);
	jesd204b_gt_setup(ad9144_gt_link);
	jesd204b_gt_en_sync_sysref(ad9144_gt_link);

	ad9680_setup(SPI_DEVICE_ID, 2);
	jesd204b_setup(AD9680_JESD_BASEADDR, jesd204b_st);
	jesd204b_gt_setup(ad9680_gt_link);
	jesd204b_gt_en_sync_sysref(ad9680_gt_link);

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

	ad9680_core.adc_baseaddr = AD9680_CORE_BASEADDR;
	ad9680_core.dmac_baseaddr = AD9680_DMA_BASEADDR;
	ad9680_core.no_of_channels = 2;
	ad9680_core.resolution = 14;
	adc_setup(ad9680_core);

	xil_printf("Initialization done.\n");

	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x05);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0x0);

	adc_pn_mon(ad9680_core, 1);

	xil_printf("PRBS test done.\n");

	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x0f);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0x1);

	adc_capture(ad9680_core, 32768, ADC_DDR_BASEADDR);

	xil_printf("Ramp capture done.\n");

	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x00);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0x1);

	adc_capture(ad9680_core, 32768, ADC_DDR_BASEADDR);

	xil_printf("Test mode off capture done.\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
