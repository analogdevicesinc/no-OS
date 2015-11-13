/***************************************************************************//**
 *   @file   ad_fmcdaq3_ebz.c
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
#include "ad9152.h"
#include "ad9528.h"
#include "ad9680.h"
#include "adc_core.h"
#include "dac_core.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_BASEADDR			XPAR_PS7_GPIO_0_BASEADDR
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000
#define AD9152_CORE_BASEADDR	XPAR_AXI_AD9152_CORE_BASEADDR
#define AD9152_JESD_BASEADDR	XPAR_AXI_AD9152_JESD_BASEADDR
#define AD9680_CORE_BASEADDR	XPAR_AXI_AD9680_CORE_BASEADDR
#define AD9680_DMA_BASEADDR		XPAR_AXI_AD9680_DMA_BASEADDR
#define AD9680_JESD_BASEADDR	XPAR_AXI_AD9680_JESD_BASEADDR
#define DAQ3_GT_BASEADDR		XPAR_AXI_DAQ3_GT_BASEADDR

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct ad9528_channel_spec ad9528_channels[] =
{
	{
		13,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_VCO,			// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"ADC_CLK",			// extended_name
	},

	{
		2,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_VCO,			// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"DAC_CLK",			// extended_name
	},

	{
		4,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_VCO,			// signal_source
		0,					// divider_phase
		2,					// channel_divider
		"ADC_CLK_FMC",		// extended_name
	},

	{
		5,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_SYSREF_VCO,	// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"ADC_SYSREF",		// extended_name
	},

	{
		6,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_SYSREF_VCO,	// signal_source
		0,					// divider_phase
		2,					// channel_divider
		"CLKD_ADC_SYSREF",	// extended_name
	},

	{
		7,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_SYSREF_VCO,	// signal_source
		0,					// divider_phase
		2,					// channel_divider
		"CLKD_DAC_SYSREF",	// extended_name
	},

	{
		8,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_SYSREF_VCO,	// signal_source
		0,					// divider_phase
		1,					// channel_divider
		"DAC_SYSREF",		// extended_name
	},

	{
		9,					// channel_num
		0,					// sync_ignore_en
		0,					// output_dis
		DRIVER_MODE_LVDS,	// driver_mode
		SOURCE_VCO,			// signal_source
		0,					// divider_phase
		2,					// channel_divider
		"DAC_CLK_FMC",		// extended_name
	},
};

struct ad9528_platform_data ad9528_pdata_lpc =
{
		125000000,	// vcxo_freq
		1,			// spi3wire

		/* REFA / REFB input configuration */
		0,	// refa_en
		0,	// refb_en

		/* Differential/ Single-Ended Input Configuration */
		0,	// refa_diff_rcv_en
		0,	// refb_diff_rcv_en
		1,	// osc_in_diff_en

		/*
		 * Valid if differential input disabled
		 * if false defaults to pos input
		 */
		0,	// refa_cmos_neg_inp_en
		0,	// refb_cmos_neg_inp_en
		0,	// osc_in_cmos_neg_inp_en

		/* PLL1 Setting */
		1,	// refa_r_div
		1,	// refb_r_div
		1,	// pll1_feedback_div
		1,	// pll1_feedback_src_vcxo
		1,	// pll1_charge_pump_current_nA
		1,	// pll1_bypass_en

		/* Reference */
		1,	// ref_mode
		SYSREF_SRC_INTERNAL,	// sysref_src
		8,	// sysref_k_div

		/* PLL2 Setting */
		805000,	// pll2_charge_pump_current_nA
		0,	// pll2_ndiv_a_cnt
		8,	// pll2_ndiv_b_cnt
		0,	// pll2_freq_doubler_en
		1,	// pll2_r1_div
		8,	// pll2_n2_div
		4,	// pll2_vco_diff_m1 /* 3..5 */

		/* Loop Filter PLL2 */
		RPOLE2_900_OHM,	// rpole2
		RZERO_3250_OHM,	// rzero
		CPOLE1_16_PF,	// cpole1
		0,	// rzero_bypass_en;

		/* Output Channel Configuration */
		ARRAY_SIZE(ad9528_channels),	// num_channels
		ad9528_channels,	// *channels
};

ad9152_init_param default_ad9152_init_param = {
	0,	// jesd_xbar_lane0_sel
	1,	// jesd_xbar_lane1_sel
	2,	// jesd_xbar_lane2_sel
	3,	// jesd_xbar_lane3_sel
	0,	// lanes2_3_swap_data
};

/***************************************************************************//**
* @brief daq3_gpio_ctl
	ch2 gpios:
		trig			7
		adc_pd			6
		dac_txen		5
		adc_fdb			4
		adc_fda			3
		dac_irq			2
		clkd_status_1	1
		clkd_status_0	0
*******************************************************************************/
void daq3_gpio_ctl(uint32_t base_addr)
{
	Xil_Out32((base_addr + 0x02c4), 0x0063); // direction
	Xil_Out32((base_addr + 0x02c8), 0x0063); // enable
	Xil_Out32((base_addr + 0x0018), 0x0000); // mask
	Xil_Out32((base_addr + 0x004c), 0x0000); // data
	mdelay(10);
	Xil_Out32((base_addr + 0x0018), 0x0060); // mask
	Xil_Out32((base_addr + 0x004c), 0x0020); // data
	mdelay(10);
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	jesd204b_gt_link ad9680_gt_link;
	jesd204b_gt_link ad9152_gt_link;
	jesd204b_state jesd204b_st;
	adc_core ad9680_core;

	daq3_gpio_ctl(GPIO_BASEADDR);

	ad9528_setup(SPI_DEVICE_ID, 0, ad9528_pdata_lpc);

	ad9152_setup(SPI_DEVICE_ID, 1, default_ad9152_init_param);

	jesd204b_st.lanesync_enable = 1;
	jesd204b_st.scramble_enable = 1;
	jesd204b_st.sysref_always_enable = 0;
	jesd204b_st.frames_per_multiframe = 32;
	jesd204b_st.bytes_per_frame = 1;
	jesd204b_st.subclass = 1;
	jesd204b_setup(AD9152_JESD_BASEADDR, jesd204b_st);

	ad9680_setup(SPI_DEVICE_ID, 2);

	jesd204b_st.lanesync_enable = 1;
	jesd204b_st.scramble_enable = 1;
	jesd204b_st.sysref_always_enable = 0;
	jesd204b_st.frames_per_multiframe = 32;
	jesd204b_st.bytes_per_frame = 1;
	jesd204b_st.subclass = 1;
	jesd204b_setup(AD9680_JESD_BASEADDR, jesd204b_st);

	ad9680_gt_link.tx_or_rx = JESD204B_GT_RX;
	ad9680_gt_link.first_lane = 0;
	ad9680_gt_link.last_lane = 3;
	ad9680_gt_link.qpll_or_cpll = JESD204B_GT_QPLL;
	ad9680_gt_link.lpm_or_dfe = JESD204B_GT_DFE;
	ad9680_gt_link.ref_clk = 500;
	ad9680_gt_link.lane_rate = 1000;
	ad9680_gt_link.sysref_int_or_ext = JESD204B_GT_SYSREF_EXT;
	ad9680_gt_link.sys_clk_sel = 3;
	ad9680_gt_link.out_clk_sel = 4;
	ad9680_gt_link.gth_or_gtx = 0;

  memcpy(&ad9152_gt_link, &ad9680_gt_link, sizeof(ad9152_gt_link));

  ad9152_gt_link.tx_or_rx = JESD204B_GT_TX;

	jesd204b_gt_initialize(DAQ3_GT_BASEADDR, 4);
	jesd204b_gt_setup(ad9152_gt_link);
	jesd204b_gt_setup(ad9680_gt_link);

	dac_setup(AD9152_CORE_BASEADDR);

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
	adc_setup(ad9680_core, 2);

	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x05);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0);

	adc_pn_mon(ad9680_core, 2, 1);

	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x0f);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0x1);
  adc_write(ad9680_core, ADC_REG_CHAN_CNTRL(0), 0x51);
  adc_write(ad9680_core, ADC_REG_CHAN_CNTRL(1), 0x51);

	xil_printf("Initialization done.\n");

	adc_capture(ad9680_core, 16384, ADC_DDR_BASEADDR);

  uint32_t n;
  uint32_t ecnt;
  uint32_t rdata;
  uint32_t edata;

  ecnt = 0;
  for (n = 0; n < 16384; n++) {
    rdata = Xil_In32(ADC_DDR_BASEADDR + (n*4)) & 0x3fff3fff;
    if (n == 0) edata = rdata;
    if (rdata != edata) {
	    xil_printf("Capture Error[%d]: rcv(%08x) exp(%08x).\n", n, rdata, edata);
      if (ecnt == 10) break;
      ecnt++;
    }
    if ((edata & 0x3fff) == 0x3fff) {
      edata = edata & 0xffff0000;
    } else {
      edata = edata + 0x00000001;
    }
    if ((edata & 0x3fff0000) == 0x3fff0000) {
      edata = edata & 0x0000ffff;
    } else {
      edata = edata + 0x00010000;
    }
  }

	xil_printf("Capture done.\n");

	ad9680_spi_write(2, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(2, AD9680_REG_ADC_TEST_MODE, 0x00);
	ad9680_spi_write(2, AD9680_REG_OUTPUT_MODE, 0x1);

	return 0;
}
