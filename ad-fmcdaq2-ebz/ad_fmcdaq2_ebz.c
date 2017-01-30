/***************************************************************************//**
 *   @file   ad_fmcdaq2_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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
#include "ad9144.h"
#include "ad9523.h"
#include "ad9680.h"
#include "adc_core.h"
#include "dac_core.h"
#include "dmac_core.h"
#include "dac_buffer.h"
#include "xcvr_core.h"
#include "jesd_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef XILINX
#define AD9144_DMA_IRQ		XPAR_FABRIC_AXI_AD9144_DMA_IRQ_INTR
#define AD9680_DMA_IRQ		XPAR_FABRIC_AXI_AD9680_DMA_IRQ_INTR
#endif

#ifdef ZYNQ_PS7
#define ADC_DDR_BASEADDR	XPAR_DDR_MEM_BASEADDR + 0x0800000
#define DAC_DDR_BASEADDR	XPAR_DDR_MEM_BASEADDR + 0xA000000
#endif

#ifdef ZYNQ_PSU
#define ADC_DDR_BASEADDR	XPAR_PSU_DDR_0_S_AXI_BASEADDR + 0x0800000
#define DAC_DDR_BASEADDR	XPAR_PSU_DDR_0_S_AXI_BASEADDR + 0xA000000
#endif

#ifdef MICROBLAZE
#define ADC_DDR_BASEADDR	XPAR_AXI_DDR_CNTRL_BASEADDR + 0x0800000
#define DAC_DDR_BASEADDR	XPAR_AXI_DDR_CNTRL_BASEADDR + 0xA000000
#endif

#define GPIO_TRIG		43
#define GPIO_ADC_PD		42
#define GPIO_DAC_TXEN		41
#define GPIO_DAC_RESET		40
#define GPIO_CLKD_SYNC		38
#define GPIO_ADC_FDB		36
#define GPIO_ADC_FDA		35
#define GPIO_DAC_IRQ		34
#define GPIO_CLKD_STATUS_1	33
#define GPIO_CLKD_STATUS_0	32

enum ad9523_channels {
       DAC_CLK,
       ADC_CLK_FMC,
       ADC_SYSREF,
       CLKD_ADC_SYSREF,
       CLKD_DAC_SYSREF,
       DAC_SYSREF,
       DAC_CLK_FMC,
       ADC_CLK,
};

/***************************************************************************//**
 * @brief main
 *******************************************************************************/
int main(void)
{
	int8_t			mode;
	// number of cap	tured samples
	uint32_t		no_of_samples = 32768;

	spi_device		ad9523_spi_device;
	spi_device		ad9144_spi_device;
	spi_device		ad9680_spi_device;

	ad9523_channel_spec	ad9523_channels[8];
	ad9523_platform_data	ad9523_param;
	ad9144_init_param	ad9144_param;
	ad9680_init_param	ad9680_param;

	dac_core		ad9144_core;
	dac_channel		ad9144_channels[2];
	jesd_core		ad9144_jesd;
	xcvr_core		ad9144_xcvr;
	dmac_core		ad9144_dma;
	adc_core		ad9680_core;
	jesd_core		ad9680_jesd;
	xcvr_core		ad9680_xcvr;
	dmac_core		ad9680_dma;
	dmac_xfer		ad9680_xfer;

	//*************************************************************
	// SPI interface configuration
	//*************************************************************

	ad_spi_init(&ad9523_spi_device);
	ad_spi_init(&ad9144_spi_device);
	ad_spi_init(&ad9680_spi_device);

	ad9523_spi_device.chip_select = 0x6;
	ad9523_spi_device.cpha = 1;
	ad9523_spi_device.cpol = 1;
	ad9144_spi_device.chip_select = 0x5;
	ad9144_spi_device.cpha = 1;
	ad9144_spi_device.cpol = 1;
	ad9680_spi_device.chip_select = 0x3;
	ad9680_spi_device.cpha = 1;
	ad9680_spi_device.cpol = 1;

	//************************************************************
	// clock distribution device (AD9523) configuration
	//************************************************************

	ad9523_param.num_channels = 8;
	ad9523_param.channels = &ad9523_channels[0];
	ad9523_init(&ad9523_param);

	// ad9523 channel configurations
	// DAC_CLK
	ad9523_channels[0].channel_num = 1;
	ad9523_channels[0].channel_divider = 1;
	ad9523_channels[0].output_dis = 0;
	ad9523_channels[0].divider_phase = 1;
	// ADC_CLK_FMC
	ad9523_channels[1].channel_num = 4;
	ad9523_channels[1].channel_divider = 2;
	ad9523_channels[1].output_dis = 0;
	ad9523_channels[1].divider_phase = 1;
	// ADC_SYSREF
	ad9523_channels[2].channel_num = 5;
	ad9523_channels[2].channel_divider = 128;
	ad9523_channels[2].output_dis = 0;
	ad9523_channels[2].divider_phase = 1;
	// CLKD_ADC_SYSREF
	ad9523_channels[3].channel_num = 6;
	ad9523_channels[3].channel_divider = 128;
	ad9523_channels[3].output_dis = 0;
	ad9523_channels[3].divider_phase = 1;
	// CLKD_DAC_SYSREF
	ad9523_channels[4].channel_num = 7;
	ad9523_channels[4].channel_divider = 128;
	ad9523_channels[4].output_dis = 0;
	ad9523_channels[4].divider_phase = 1;
	// DAC_SYSREF
	ad9523_channels[5].channel_num = 8;
	ad9523_channels[5].channel_divider = 128;
	ad9523_channels[5].output_dis = 0;
	ad9523_channels[5].divider_phase = 1;
	// DAC_CLK_FMC
	ad9523_channels[6].channel_num = 9;
	ad9523_channels[6].channel_divider = 2;
	ad9523_channels[6].output_dis = 0;
	ad9523_channels[6].divider_phase = 1;
	// ADC_CLK
	ad9523_channels[7].channel_num = 13;
	ad9523_channels[7].channel_divider = 1;
	ad9523_channels[7].output_dis = 0;
	ad9523_channels[7].divider_phase = 1;

	// VCXO 125Mhz
	ad9523_param.vcxo_freq = 125000000;
	ad9523_param.spi3wire = 1;
	ad9523_param.osc_in_diff_en = 1;
	ad9523_param.pll1_bypass_en = 1;
	ad9523_param.pll2_charge_pump_current_nA = 413000;
	ad9523_param.pll2_ndiv_a_cnt = 0;
	ad9523_param.pll2_ndiv_b_cnt = 6;
	ad9523_param.pll2_freq_doubler_en = 0;
	ad9523_param.pll2_r2_div = 1;
	ad9523_param.pll2_vco_diff_m1 = 3;
	ad9523_param.pll2_vco_diff_m1 = 0;
	ad9523_param.rpole2 = 0;
	ad9523_param.rzero = 7;
	ad9523_param.cpole1 = 2;
	ad9523_param.rzero_bypass_en = 0;
	ad9523_param.num_channels = 8;

	//************************************************************
	// DAC (AD9144) and the transmit path ( AXI_ADXCVR,
	//	JESD204, AXI_AD9144, TX DMAC) configuration
	//************************************************************

	// setup the base addresses

#ifdef XILINX
	ad9144_xcvr.base_address = XPAR_AXI_AD9144_XCVR_BASEADDR;
	ad9144_core.base_address = XPAR_AXI_AD9144_CORE_BASEADDR;
	ad9680_xcvr.base_address = XPAR_AXI_AD9680_XCVR_BASEADDR;
	ad9680_core.base_address = XPAR_AXI_AD9680_CORE_BASEADDR;
	ad9144_jesd.base_address = XPAR_AXI_AD9144_JESD_BASEADDR;
	ad9680_jesd.base_address = XPAR_AXI_AD9680_JESD_BASEADDR;
#endif
#ifdef ALTERA
	ad9144_xcvr.base_address = AXI_AD9144_XCVR_BASE;
	ad9144_core.base_address = AXI_AD9144_CORE_BASE;
	ad9680_xcvr.base_address = AXI_AD9680_XCVR_BASE;
	ad9680_core.base_address = AXI_AD9680_CORE_BASE;
	ad9144_jesd.base_address = AVL_AD9144_XCVR_IP_RECONFIG_BASE;
	ad9680_jesd.base_address = AVL_AD9680_XCVR_IP_RECONFIG_BASE;
	ad9144_xcvr.mmcm_lpll_base_address = AVL_AD9144_XCVR_CORE_PLL_RECONFIG_BASE;
	ad9680_xcvr.mmcm_lpll_base_address = AVL_AD9680_XCVR_CORE_PLL_RECONFIG_BASE;
	ad9144_xcvr.tx_lane_pll_base_address = AVL_AD9144_XCVR_LANE_PLL_RECONFIG_BASE;
#endif

	// transmit path configuration

	ad9144_param.jesd_xbar_lane0_sel = 2;
	ad9144_param.jesd_xbar_lane1_sel = 3;
	ad9144_param.jesd_xbar_lane2_sel = 0;
	ad9144_param.jesd_xbar_lane3_sel = 1;
	ad9144_param.lane_rate_kbps = 10000000;
	ad9144_param.active_converters = 2;

	ad9144_core.no_of_channels = ad9144_param.active_converters;
	ad9144_core.resolution = 16;
	ad9144_core.fifo_present = 1;

	ad9144_channels[0].dds_dual_tone = 0;
	ad9144_channels[0].dds_frequency_0 = 33*1000*1000;
	ad9144_channels[0].dds_phase_0 = 0;
	ad9144_channels[0].dds_scale_0 = 500000;
	ad9144_channels[0].sel = DAC_SRC_DDS;
	ad9144_channels[1].dds_dual_tone = 0;
	ad9144_channels[1].dds_frequency_0 = 11*1000*1000;
	ad9144_channels[1].dds_phase_0 = 0;
	ad9144_channels[1].dds_scale_0 = 500000;
	ad9144_channels[0].pat_data = 0xb1b0a1a0;
	ad9144_channels[1].pat_data = 0xd1d0c1c0;
	ad9144_channels[1].sel = DAC_SRC_DDS;

	ad9144_core.channels = &ad9144_channels[0];

	ad9144_param.stpl_samples[0][0] = (ad9144_channels[0].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[0][1] = (ad9144_channels[0].pat_data>>16) & 0xffff;
	ad9144_param.stpl_samples[0][2] = (ad9144_channels[0].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[0][3] = (ad9144_channels[0].pat_data>>16) & 0xffff;
	ad9144_param.stpl_samples[1][0] = (ad9144_channels[1].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[1][1] = (ad9144_channels[1].pat_data>>16) & 0xffff;
	ad9144_param.stpl_samples[1][2] = (ad9144_channels[1].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[1][3] = (ad9144_channels[1].pat_data>>16) & 0xffff;

	ad9144_jesd.rx_tx_n = 0;
	ad9144_jesd.octets_per_frame = 1;
	ad9144_jesd.frames_per_multiframe = 32;
	ad9144_jesd.subclass_mode = 1;

	ad9144_xcvr.rx_tx_n = 0;
	ad9144_xcvr.gth_enable = 0;
	ad9144_xcvr.lpm_enable = 1;
	ad9144_xcvr.out_clk_sel = 4;
	ad9144_xcvr.sys_clk_sel = 0;
	ad9144_xcvr.lane_rate_kbps = 10000000;

	ad9144_dma.type = DMAC_DOWNSTREAM;
	ad9144_dma.irq_id = AD9144_DMA_IRQ;

	// receive path configuration

	ad9680_core.no_of_channels = 2;
	ad9680_core.resolution = 14;

	ad9680_jesd.rx_tx_n = 1;
	ad9680_jesd.octets_per_frame = 1;
	ad9680_jesd.frames_per_multiframe = 32;
	ad9680_jesd.subclass_mode = 1;

	ad9680_xcvr.rx_tx_n = 1;
	ad9680_xcvr.gth_enable = 0;
	ad9680_xcvr.lpm_enable = 1;
	ad9680_xcvr.out_clk_sel = 4;
	ad9680_xcvr.sys_clk_sel = 0;

	ad9680_dma.type = DMAC_UPSTREAM;
	ad9680_dma.irq_id = AD9680_DMA_IRQ;

	//************************************************************
	// Select the desired lane rate
	//************************************************************

	ad_printf ("Available sampling rates:\n");
	ad_printf ("\t1 - ADC 1000 MSPS; DAC 1000 MSPS\n");
	ad_printf ("\t2 - ADC 500 MSPS; DAC 1000 MSPS\n");
	ad_printf ("\t3 - ADC 500 MSPS; DAC 500 MSPS\n");
	ad_printf ("\t4 - ADC 750 MSPS; DAC 750 MSPS\n");
	ad_printf ("\t5 - ADC 375 MSPS; DAC 750 MSPS\n");
	ad_printf ("\t6 - ADC 375 MSPS; DAC 375 MSPS\n");

	mode = inbyte();

	switch (mode) {
		case '1':
			ad9523_param.pll2_vco_diff_m1 = 3;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 2;
			ad9523_channels[ADC_CLK].channel_divider = 1;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_kbps = 10000000;
			ad9144_xcvr.ref_clock_khz = 500000;
			ad9680_xcvr.sys_clk_sel = 3;
			ad9680_xcvr.lane_rate_kbps = 10000000;
			ad9680_xcvr.ref_clock_khz = 500000;
			break;
		case '2':
			ad9523_param.pll2_vco_diff_m1 = 3;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_kbps = 10000000;
			ad9144_xcvr.ref_clock_khz = 500000;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_kbps = 5000000;
			ad9680_xcvr.ref_clock_khz = 250000;
			break;
		case '3':
			ad9523_param.pll2_vco_diff_m1 = 3;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 4;
			ad9523_channels[DAC_CLK].channel_divider = 2;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 0;
			ad9144_xcvr.lane_rate_kbps = 5000000;
			ad9144_xcvr.ref_clock_khz = 250000;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_kbps = 5000000;
			ad9680_xcvr.ref_clock_khz = 250000;
			break;
		case '4':
			ad9523_param.pll2_vco_diff_m1 = 4;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 2;
			ad9523_channels[ADC_CLK].channel_divider = 1;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_kbps = 7500000;
			ad9144_xcvr.ref_clock_khz = 375000;
			ad9680_xcvr.sys_clk_sel = 3;
			ad9680_xcvr.lane_rate_kbps = 7500000;
			ad9680_xcvr.ref_clock_khz = 375000;
			break;
		case '5':
			ad9523_param.pll2_vco_diff_m1 = 4;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_kbps = 7500000;
			ad9144_xcvr.ref_clock_khz = 375000;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_kbps = 3750000;
			ad9680_xcvr.ref_clock_khz = 187500;
			break;
		case '6':
			ad9523_param.pll2_vco_diff_m1 = 4;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 4;
			ad9523_channels[DAC_CLK].channel_divider = 2;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 0;
			ad9144_xcvr.lane_rate_kbps = 3750000;
			ad9144_xcvr.ref_clock_khz = 187500;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_kbps = 3750000;
			ad9680_xcvr.ref_clock_khz = 187500;
			break;
		default:
			ad_printf ("Unknown selection.\n");
			return -1;
	}

	ad9144_param.lane_rate_kbps = ad9144_xcvr.lane_rate_kbps;
	ad9680_param.lane_rate_kbps = ad9680_xcvr.lane_rate_kbps;

	// setup GPIOs

	ad_platform_init();

	ad_gpio_set(GPIO_CLKD_SYNC, 1);
	ad_gpio_set(GPIO_DAC_RESET, 1);
	ad_gpio_set(GPIO_DAC_TXEN, 1);
	ad_gpio_set(GPIO_ADC_PD, 0);

	// setup clocks

	ad9523_setup(&ad9523_spi_device, &ad9523_param);

	// setup transmit path

	ad9144_setup(&ad9144_spi_device, ad9144_param);
	jesd_setup(ad9144_jesd);
	xcvr_setup(ad9144_xcvr);
	jesd_status(ad9144_jesd);
	dac_setup(ad9144_core);
	ad9144_status(&ad9144_spi_device);

	// transport layer testing

	ad9144_channels[0].sel = DAC_SRC_SED;
	ad9144_channels[1].sel = DAC_SRC_SED;
	dac_data_setup(ad9144_core);
	ad9144_short_pattern_test(&ad9144_spi_device, ad9144_param);

	// PN7 data path test

	ad9144_channels[0].sel = DAC_SRC_PN7;
	ad9144_channels[1].sel = DAC_SRC_PN7;
	ad9144_param.prbs_type = 0;		// PN7
	dac_data_setup(ad9144_core);
	ad9144_datapath_prbs_test(&ad9144_spi_device, ad9144_param);

	// PN15 data path test

	ad9144_channels[0].sel = DAC_SRC_PN15;
	ad9144_channels[1].sel = DAC_SRC_PN15;
	ad9144_param.prbs_type = 1;		// PN15
	dac_data_setup(ad9144_core);
	ad9144_datapath_prbs_test(&ad9144_spi_device, ad9144_param);

	// setup receive path

	ad9680_setup(&ad9680_spi_device, ad9680_param);
	jesd_setup(ad9680_jesd);
	xcvr_setup(ad9680_xcvr);
	jesd_status(ad9680_jesd);
	adc_setup(ad9680_core);
	ad9680_test(&ad9680_spi_device, AD9680_TEST_PN9);
	adc_pn_mon(ad9680_core, ADC_PN9);
	ad9680_test(&ad9680_spi_device, AD9680_TEST_PN23);
	adc_pn_mon(ad9680_core, ADC_PN23A);

	// default data

	ad9144_channels[0].sel = DAC_SRC_DDS;
	ad9144_channels[1].sel = DAC_SRC_DDS;
	dac_data_setup(ad9144_core);
	ad9680_test(&ad9680_spi_device, AD9680_TEST_OFF);
	ad_printf("daq2: done\n");

	ad_platform_close();

	return 0;
}
