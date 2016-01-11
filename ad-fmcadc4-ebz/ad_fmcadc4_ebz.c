/***************************************************************************//**
 *   @file   ad_fmcadc4_ebz.c
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
#include "ad9528.h"
#include "ad9680.h"
#include "adc_core.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"
#include <xgpiops.h>
#include <xspips.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				54 + 32
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000
#define AD9680_CORE_0_BASEADDR	XPAR_AXI_AD9680_CORE_0_BASEADDR
#define AD9680_CORE_1_BASEADDR	XPAR_AXI_AD9680_CORE_1_BASEADDR
#define AD9680_DMA_BASEADDR		XPAR_AXI_AD9680_DMA_BASEADDR
#define AD9680_JESD_BASEADDR	XPAR_AXI_AD9680_JESD_BASEADDR
#define FMCADC4_GT_BASEADDR		XPAR_AXI_FMCADC4_GT_BASEADDR
#define GPIO_AD9528_RSTN		GPIO_OFFSET + 0
#define GPIO_AD9528_STATUS		GPIO_OFFSET + 1
#define GPIO_AD9680_1_FDA		GPIO_OFFSET + 2
#define GPIO_AD9680_1_FDB		GPIO_OFFSET + 3
#define GPIO_AD9680_2_FDA		GPIO_OFFSET + 4
#define GPIO_AD9680_2_FDB		GPIO_OFFSET + 5

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
extern uint8_t spi_decoded_cs;

struct ad9528_channel_spec ad9528_channels[] =
{
	{
		1,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_VCO,			// signal_source
		0,					// divider_phase
		2,					// channel_divider
		"FPGA",				// extended_name
	},

	{
		2,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_SYSREF_VCO,	// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"ADC2-sysref",		// extended_name
	},

	{
		3,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_VCO,			// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"ADC2",				// extended_name
	},

	{
		8,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_VCO,			// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"ADC1",				// extended_name
	},

	{
		9,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_SYSREF_VCO,	// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"ADC1-sysref",		// extended_name
	},

	{
		13,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_SYSREF_VCO,	// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"FPGA-sysref",		// extended_name
	},
};

struct ad9528_platform_data ad9528_pdata_lpc =
{
	80000000,	// vcxo_freq
	1,			// spi3wire

	/* REFA / REFB input configuration */
	0,	// refa_en
	0,	// refb_en

	/* Differential/ Single-Ended Input Configuration */
	1,	// refa_diff_rcv_en
	0,	// refb_diff_rcv_en
	0,	// osc_in_diff_en

	/*
	 * Valid if differential input disabled
	 * if false defaults to pos input
	 */
	0,	// refa_cmos_neg_inp_en
	0,	// refb_cmos_neg_inp_en
	1,	// osc_in_cmos_neg_inp_en

	/* PLL1 Setting */
	1,		// refa_r_div
	1,		// refb_r_div
	8,		// pll1_feedback_div
	1,		// pll1_feedback_src_vcxo
	10000,	// pll1_charge_pump_current_nA
	0,		// pll1_bypass_en

	/* Reference */
	REF_MODE_STAY_ON_REFB,	// ref_mode
	SYSREF_SRC_INTERNAL,	// sysref_src
	20,						// sysref_k_div

	/* PLL2 Setting */
	805000,	// pll2_charge_pump_current_nA
	0,	// pll2_ndiv_a_cnt
	25,	// pll2_ndiv_b_cnt
	0,	// pll2_freq_doubler_en
	2,	// pll2_r1_div
	25,	// pll2_n2_div
	4,	// pll2_vco_diff_m1 /* 3..5 */

	/* Loop Filter PLL2 */
	RPOLE2_900_OHM,	// rpole2
	RZERO_3250_OHM,	// rzero
	CPOLE1_16_PF,	// cpole1
	0,				// rzero_bypass_en;

	/* Output Channel Configuration */
	ARRAY_SIZE(ad9528_channels),	// num_channels
	ad9528_channels,				// *channels
};

/***************************************************************************//**
* @brief adc4_gpio_ctl
*******************************************************************************/
void adc4_gpio_ctl(uint32_t device_id)
{
	gpio_init(device_id);

	gpio_direction(GPIO_AD9528_RSTN, GPIO_OUTPUT);
	gpio_direction(GPIO_AD9528_STATUS, GPIO_OUTPUT);
	gpio_direction(GPIO_AD9680_1_FDA, GPIO_INPUT);
	gpio_direction(GPIO_AD9680_1_FDB, GPIO_INPUT);
	gpio_direction(GPIO_AD9680_2_FDA, GPIO_INPUT);
	gpio_direction(GPIO_AD9680_2_FDB, GPIO_INPUT);

	gpio_set_value(GPIO_AD9528_STATUS, 0);
	gpio_set_value(GPIO_AD9528_RSTN, 0);
	mdelay(10);

	gpio_set_value(GPIO_AD9528_RSTN, 1);
	mdelay(10);
}

jesd204b_state jesd204b_st = {
	1,	// lanesync_enable
	1,	// scramble_enable
	0,	// sysref_always_enable
	32,	// frames_per_multiframe
	1,	// bytes_per_frame
	1,	// subclass
};

jesd204b_gt_link gt_link = {
	FMCADC4_GT_BASEADDR,	// gt_core_addr
	JESD204B_GT_RX,			// tx_or_rx
	0,						// first_lane
	7,						// last_lane
	JESD204B_GT_QPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	500,					// ref_clk
	1000,					// lane_rate
	JESD204B_GT_SYSREF_INT,	// sysref_int_or_ext
	3,						// sys_clk_sel
	4,						// out_clk_sel
	0,						// gth_or_gtx
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	adc_core ad9680_0_core;
	adc_core ad9680_1_core;

	adc4_gpio_ctl(GPIO_DEVICE_ID);

	spi_decoded_cs = 1;

#ifdef MODE_1_24G
	ad9528_pdata_lpc.pll2_ndiv_a_cnt = 1;
	ad9528_pdata_lpc.pll2_ndiv_b_cnt = 23;
	ad9528_pdata_lpc.pll2_n2_div = 31;
	ad9528_pdata_lpc.pll2_vco_diff_m1 = 3;
#endif

	ad9528_setup(SPI_DEVICE_ID, 0, ad9528_pdata_lpc);

	jesd204b_gt_initialize(gt_link);

	ad9680_setup(SPI_DEVICE_ID, 1);

	ad9680_setup(SPI_DEVICE_ID, 2);

	jesd204b_setup(AD9680_JESD_BASEADDR, jesd204b_st);

	jesd204b_gt_setup(gt_link);
	jesd204b_gt_en_sync_sysref(gt_link);

	ad9680_0_core.adc_baseaddr = AD9680_CORE_0_BASEADDR;
	ad9680_0_core.dmac_baseaddr = AD9680_DMA_BASEADDR;
	ad9680_0_core.no_of_channels = 2;
	ad9680_0_core.resolution = 14;
	adc_setup(ad9680_0_core);

	ad9680_1_core.adc_baseaddr = AD9680_CORE_1_BASEADDR;
	ad9680_1_core.dmac_baseaddr = 0;
	ad9680_1_core.no_of_channels = 2;
	ad9680_1_core.resolution = 14;
	adc_setup(ad9680_1_core);

	ad9680_spi_write(1, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(1, AD9680_REG_ADC_TEST_MODE, 0x05);
	ad9680_spi_write(1, AD9680_REG_OUTPUT_MODE, 0);
	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x05);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0);

	adc_pn_mon(ad9680_0_core, ADC_PN23A);
	adc_pn_mon(ad9680_1_core, ADC_PN23A);

	xil_printf("Initialization done.\n\r");

	ad9680_spi_write(1, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(1, AD9680_REG_ADC_TEST_MODE, 0x0F);
	ad9680_spi_write(1, AD9680_REG_OUTPUT_MODE, 0x1);

	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x0F);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0x1);

	adc_capture(ad9680_0_core, 32768, ADC_DDR_BASEADDR);

	xil_printf("Ramp capture done.\n");

	return 0;
}
