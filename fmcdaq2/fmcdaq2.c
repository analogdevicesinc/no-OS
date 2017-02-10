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
	DAC_DEVICE_CLK,
	DAC_DEVICE_SYSREF,
	DAC_FPGA_CLK,
	DAC_FPGA_SYSREF,
	ADC_DEVICE_CLK,
	ADC_DEVICE_SYSREF,
	ADC_FPGA_CLK,
	ADC_FPGA_SYSREF,
};

/***************************************************************************//**
 * @brief main
 *******************************************************************************/
int fmcdaq2_reconfig(ad9144_init_param *p_ad9144_param, xcvr_core *p_ad9144_xcvr,
		     ad9680_init_param *p_ad9680_param, xcvr_core *p_ad9680_xcvr,
		     ad9523_platform_data *p_ad9523_param)
{

#ifdef	ALTERA
#endif

#ifdef	ZYNQ_PS7

	uint8_t mode = 0;

	ad_printf ("Available sampling rates:\n");
	ad_printf ("\t1 - ADC 1000 MSPS; DAC 1000 MSPS\n");
	ad_printf ("\t2 - ADC  500 MSPS; DAC 1000 MSPS\n");
	ad_printf ("\t3 - ADC  500 MSPS; DAC  500 MSPS\n");
	ad_printf ("\t4 - ADC  750 MSPS; DAC  750 MSPS\n");

	mode = ad_uart_read();

	switch (mode) {
		case '1':
			p_ad9523_param->pll2_vco_diff_m1 = 3;
			(&p_ad9523_param->channels[DAC_FPGA_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[DAC_DEVICE_CLK])->channel_divider = 1;
			(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[ADC_FPGA_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[ADC_DEVICE_CLK])->channel_divider = 1;
			(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->channel_divider = 128;
			p_ad9144_xcvr->reconfig_bypass = 0;
			p_ad9144_xcvr->lpm_enable = 1;
			p_ad9144_xcvr->out_clk_sel = 4;
			p_ad9144_xcvr->sys_clk_sel = 3;
			p_ad9144_param->lane_rate_kbps = 10000000;
			p_ad9144_xcvr->lane_rate_kbps = 10000000;
			p_ad9144_xcvr->ref_clock_khz = 500000;
			p_ad9680_xcvr->reconfig_bypass = 0;
			p_ad9680_xcvr->lpm_enable = 1;
			p_ad9680_xcvr->out_clk_sel = 4;
			p_ad9680_xcvr->sys_clk_sel = 3;
			p_ad9680_param->lane_rate_kbps = 10000000;
			p_ad9680_xcvr->lane_rate_kbps = 10000000;
			p_ad9680_xcvr->ref_clock_khz = 500000;
			break;
		case '2':
			p_ad9523_param->pll2_vco_diff_m1 = 3;
			(&p_ad9523_param->channels[DAC_FPGA_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[DAC_DEVICE_CLK])->channel_divider = 1;
			(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[ADC_FPGA_CLK])->channel_divider = 4;
			(&p_ad9523_param->channels[ADC_DEVICE_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->channel_divider = 256;
			(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->channel_divider = 256;
			p_ad9144_xcvr->reconfig_bypass = 0;
			p_ad9144_xcvr->lpm_enable = 0;
			p_ad9144_xcvr->out_clk_sel = 4;
			p_ad9144_xcvr->sys_clk_sel = 3;
			p_ad9144_param->lane_rate_kbps = 10000000;
			p_ad9144_xcvr->lane_rate_kbps = 10000000;
			p_ad9144_xcvr->ref_clock_khz = 500000;
			p_ad9680_xcvr->reconfig_bypass = 0;
			p_ad9680_xcvr->lpm_enable = 1;
			p_ad9680_xcvr->out_clk_sel = 4;
			p_ad9680_xcvr->sys_clk_sel = 0;
			p_ad9680_param->lane_rate_kbps = 5000000;
			p_ad9680_xcvr->lane_rate_kbps = 5000000;
			p_ad9680_xcvr->ref_clock_khz = 250000;
			break;
		case '3':
			p_ad9523_param->pll2_vco_diff_m1 = 3;
			(&p_ad9523_param->channels[DAC_FPGA_CLK])->channel_divider = 4;
			(&p_ad9523_param->channels[DAC_DEVICE_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->channel_divider = 256;
			(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->channel_divider = 256;
			(&p_ad9523_param->channels[ADC_FPGA_CLK])->channel_divider = 4;
			(&p_ad9523_param->channels[ADC_DEVICE_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->channel_divider = 256;
			(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->channel_divider = 256;
			p_ad9144_xcvr->reconfig_bypass = 0;
			p_ad9144_xcvr->lpm_enable = 1;
			p_ad9144_xcvr->out_clk_sel = 4;
			p_ad9144_xcvr->sys_clk_sel = 0;
			p_ad9144_param->lane_rate_kbps = 5000000;
			p_ad9144_xcvr->lane_rate_kbps = 5000000;
			p_ad9144_xcvr->ref_clock_khz = 250000;
			p_ad9680_xcvr->reconfig_bypass = 0;
			p_ad9680_xcvr->lpm_enable = 1;
			p_ad9680_xcvr->out_clk_sel = 4;
			p_ad9680_xcvr->sys_clk_sel = 0;
			p_ad9680_param->lane_rate_kbps = 5000000;
			p_ad9680_xcvr->lane_rate_kbps = 5000000;
			p_ad9680_xcvr->ref_clock_khz = 250000;
			break;
		case '4':
			p_ad9523_param->pll2_vco_diff_m1 = 4;
			(&p_ad9523_param->channels[DAC_FPGA_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[DAC_DEVICE_CLK])->channel_divider = 1;
			(&p_ad9523_param->channels[DAC_DEVICE_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[DAC_FPGA_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[ADC_FPGA_CLK])->channel_divider = 2;
			(&p_ad9523_param->channels[ADC_DEVICE_CLK])->channel_divider = 1;
			(&p_ad9523_param->channels[ADC_DEVICE_SYSREF])->channel_divider = 128;
			(&p_ad9523_param->channels[ADC_FPGA_SYSREF])->channel_divider = 128;
			p_ad9144_xcvr->reconfig_bypass = 0;
			p_ad9144_xcvr->lpm_enable = 0;
			p_ad9144_xcvr->out_clk_sel = 4;
			p_ad9144_xcvr->sys_clk_sel = 0;
			p_ad9144_param->lane_rate_kbps = 7500000;
			p_ad9144_xcvr->lane_rate_kbps = 7500000;
			p_ad9144_xcvr->ref_clock_khz = 375000;
			p_ad9680_xcvr->reconfig_bypass = 0;
			p_ad9680_xcvr->lpm_enable = 1;
			p_ad9680_xcvr->out_clk_sel = 4;
			p_ad9680_xcvr->sys_clk_sel = 0;
			p_ad9680_param->lane_rate_kbps = 7500000;
			p_ad9680_xcvr->lane_rate_kbps = 7500000;
			p_ad9680_xcvr->ref_clock_khz = 375000;
			break;
		default:
			ad_printf("Unknown selection.\n");
			return -1;
	}
#endif
	return(0);
}

/***************************************************************************//**
 * @brief main
 *******************************************************************************/
int main(void)
{
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
	adc_core		ad9680_core;
	jesd_core		ad9680_jesd;
	xcvr_core		ad9680_xcvr;
	dmac_core               ad9680_dma;
	dmac_xfer               rx_xfer;

	//********************************************************************************
	// setup the base addresses
	//********************************************************************************

#ifdef XILINX
	ad9144_xcvr.base_address = XPAR_AXI_AD9144_XCVR_BASEADDR;
	ad9144_core.base_address = XPAR_AXI_AD9144_CORE_BASEADDR;
	ad9144_jesd.base_address = XPAR_AXI_AD9144_JESD_BASEADDR;
	ad9680_xcvr.base_address = XPAR_AXI_AD9680_XCVR_BASEADDR;
	ad9680_core.base_address = XPAR_AXI_AD9680_CORE_BASEADDR;
	ad9680_jesd.base_address = XPAR_AXI_AD9680_JESD_BASEADDR;
	ad9680_dma.base_address = XPAR_AXI_AD9680_DMA_BASEADDR;
#endif

#ifdef ZYNQ
	rx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
#endif

#ifdef MICROBLAZE
	rx_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000;
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
	ad9680_dma.base_address = AXI_AD9680_DMA_BASE;
	rx_xfer.start_address =  0x800000;
#endif

	//********************************************************************************
	// SPI interface configuration
	//********************************************************************************

	ad_spi_init(&ad9523_spi_device);
	ad_spi_init(&ad9144_spi_device);
	ad_spi_init(&ad9680_spi_device);

	ad9523_spi_device.chip_select = 0x6;
	ad9144_spi_device.chip_select = 0x5;
	ad9680_spi_device.chip_select = 0x3;

	//********************************************************************************
	// clock distribution device (AD9523) configuration
	//********************************************************************************

	ad9523_param.num_channels = 8;
	ad9523_param.channels = &ad9523_channels[0];
	ad9523_init(&ad9523_param);

	// dac device-clk-sysref, fpga-clk-sysref

	ad9523_channels[DAC_DEVICE_CLK].channel_num = 1;
	ad9523_channels[DAC_DEVICE_CLK].channel_divider = 1;
	ad9523_channels[DAC_DEVICE_SYSREF].channel_num = 7;
	ad9523_channels[DAC_DEVICE_SYSREF].channel_divider = 128;
	ad9523_channels[DAC_FPGA_CLK].channel_num = 9;
	ad9523_channels[DAC_FPGA_CLK].channel_divider = 2;
	ad9523_channels[DAC_FPGA_SYSREF].channel_num = 8;
	ad9523_channels[DAC_FPGA_SYSREF].channel_divider = 128;

	// adc device-clk-sysref, fpga-clk-sysref

	ad9523_channels[ADC_DEVICE_CLK].channel_num = 13;
	ad9523_channels[ADC_DEVICE_CLK].channel_divider = 1;
	ad9523_channels[ADC_DEVICE_SYSREF].channel_num = 6;
	ad9523_channels[ADC_DEVICE_SYSREF].channel_divider = 128;
	ad9523_channels[ADC_FPGA_CLK].channel_num = 4;
	ad9523_channels[ADC_FPGA_CLK].channel_divider = 2;
	ad9523_channels[ADC_FPGA_SYSREF].channel_num = 5;
	ad9523_channels[ADC_FPGA_SYSREF].channel_divider = 128;

	// VCXO 125MHz

	ad9523_param.vcxo_freq = 125000000;
	ad9523_param.spi3wire = 1;
	ad9523_param.osc_in_diff_en = 1;
	ad9523_param.pll2_charge_pump_current_nA = 413000;
	ad9523_param.pll2_freq_doubler_en = 0;
	ad9523_param.pll2_r2_div = 1;
	ad9523_param.pll2_ndiv_a_cnt = 0;
	ad9523_param.pll2_ndiv_b_cnt = 6;
	ad9523_param.pll2_vco_diff_m1 = 3;
	ad9523_param.pll2_vco_diff_m2 = 0;
	ad9523_param.rpole2 = 0;
	ad9523_param.rzero = 7;
	ad9523_param.cpole1 = 2;

	//********************************************************************************
	// DAC (AD9144) and the transmit path (AXI_ADXCVR,
	//	JESD204, AXI_AD9144, TX DMAC) configuration
	//********************************************************************************

	xcvr_getconfig(&ad9144_xcvr);
	ad9144_xcvr.mmcm_present = 0;
	ad9144_xcvr.reconfig_bypass = 1;
	ad9144_xcvr.lane_rate_kbps = 10000000;

	ad9144_jesd.rx_tx_n = 0;
	ad9144_jesd.scramble_enable = 1;
	ad9144_jesd.octets_per_frame = 1;
	ad9144_jesd.frames_per_multiframe = 32;
	ad9144_jesd.subclass_mode = 1;

	ad9144_channels[0].dds_dual_tone = 0;
	ad9144_channels[0].dds_frequency_0 = 33*1000*1000;
	ad9144_channels[0].dds_phase_0 = 0;
	ad9144_channels[0].dds_scale_0 = 500000;
	ad9144_channels[0].pat_data = 0xb1b0a1a0;
	ad9144_channels[1].dds_dual_tone = 0;
	ad9144_channels[1].dds_frequency_0 = 11*1000*1000;
	ad9144_channels[1].dds_phase_0 = 0;
	ad9144_channels[1].dds_scale_0 = 500000;
	ad9144_channels[1].pat_data = 0xd1d0c1c0;
	ad9144_channels[0].sel = DAC_SRC_DDS;
	ad9144_channels[1].sel = DAC_SRC_DDS;

	ad9144_param.lane_rate_kbps = 10000000;
	ad9144_param.active_converters = 2;

	ad9144_core.no_of_channels = ad9144_param.active_converters;
	ad9144_core.resolution = 16;
	ad9144_core.channels = &ad9144_channels[0];

	ad9144_param.stpl_samples[0][0] = (ad9144_channels[0].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[0][1] = (ad9144_channels[0].pat_data>>16) & 0xffff;
	ad9144_param.stpl_samples[0][2] = (ad9144_channels[0].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[0][3] = (ad9144_channels[0].pat_data>>16) & 0xffff;
	ad9144_param.stpl_samples[1][0] = (ad9144_channels[1].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[1][1] = (ad9144_channels[1].pat_data>>16) & 0xffff;
	ad9144_param.stpl_samples[1][2] = (ad9144_channels[1].pat_data>> 0) & 0xffff;
	ad9144_param.stpl_samples[1][3] = (ad9144_channels[1].pat_data>>16) & 0xffff;

	//********************************************************************************
	// ADC (AD9680) and the transmit path ( AXI_ADXCVR,
	//	JESD204, AXI_AD9680, TX DMAC) configuration
	//********************************************************************************

	ad9680_param.lane_rate_kbps = 10000000;

	xcvr_getconfig(&ad9680_xcvr);
	ad9680_xcvr.mmcm_present = 0;
	ad9680_xcvr.reconfig_bypass = 1;
	ad9680_xcvr.lane_rate_kbps = ad9680_param.lane_rate_kbps;

	ad9680_jesd.scramble_enable = 1;
	ad9680_jesd.octets_per_frame = 1;
	ad9680_jesd.frames_per_multiframe = 32;
	ad9680_jesd.subclass_mode = 1;

	ad9680_core.no_of_channels = 2;
	ad9680_core.resolution = 14;

	//********************************************************************************
	// configure the receiver DMA
	//********************************************************************************

	ad9680_dma.type = DMAC_RX;
	ad9680_dma.transfer = &rx_xfer;
	rx_xfer.id = 0;
	rx_xfer.size = 32768;

	// change the default JESD configurations, if required
#ifdef ZYNQ_PS7
	fmcdaq2_reconfig(&ad9144_param, &ad9144_xcvr, &ad9680_param, &ad9680_xcvr, &ad9523_param);
#endif
	//********************************************************************************
	// bring up the system
	//********************************************************************************

	// setup GPIOs

	ad_platform_init();
	ad_gpio_set(GPIO_DAC_RESET, 0);
	mdelay(1);

	ad_gpio_set(GPIO_CLKD_SYNC, 1);
	ad_gpio_set(GPIO_DAC_RESET, 1);
	ad_gpio_set(GPIO_DAC_TXEN, 1);
	ad_gpio_set(GPIO_ADC_PD, 0);

	// setup clocks

	ad9523_setup(&ad9523_spi_device, &ad9523_param);

	// set up the devices
	ad9680_setup(&ad9680_spi_device, ad9680_param);
	ad9144_setup(&ad9144_spi_device, ad9144_param);

	// set up the JESD core
	jesd_setup(ad9680_jesd);
	jesd_setup(ad9144_jesd);

	// set up the XCVRs
	if (ad9144_xcvr.sys_clk_sel == 3) {	// DAC_XCVR controls the QPLL reset
		xcvr_setup(ad9144_xcvr);
		xcvr_setup(ad9680_xcvr);
	} else {				// ADC_XCVR controls the CPLL reset
		xcvr_setup(ad9680_xcvr);
		xcvr_setup(ad9144_xcvr);
	}

	// JESD core status
	jesd_status(ad9144_jesd);
	jesd_status(ad9680_jesd);

	// interface core set up
	adc_setup(ad9680_core);
	dac_setup(ad9144_core);

	ad9144_status(&ad9144_spi_device);

	//********************************************************************************
	// transport path testing
	//********************************************************************************

	ad9144_channels[0].sel = DAC_SRC_SED;
	ad9144_channels[1].sel = DAC_SRC_SED;
	dac_data_setup(ad9144_core);
	ad9144_short_pattern_test(&ad9144_spi_device, ad9144_param);

	// PN7 data path test

	ad9144_channels[0].sel = DAC_SRC_PN23;
	ad9144_channels[1].sel = DAC_SRC_PN23;
	dac_data_setup(ad9144_core);
	ad9144_param.prbs_type = AD9144_PRBS7;
	ad9144_datapath_prbs_test(&ad9144_spi_device, ad9144_param);

	// PN15 data path test

	ad9144_channels[0].sel = DAC_SRC_PN31;
	ad9144_channels[1].sel = DAC_SRC_PN31;
	dac_data_setup(ad9144_core);
	ad9144_param.prbs_type = AD9144_PRBS15;
	ad9144_datapath_prbs_test(&ad9144_spi_device, ad9144_param);

	//********************************************************************************
	// receive path testing
	//********************************************************************************

	ad9680_test(&ad9680_spi_device, AD9680_TEST_PN9);
	adc_pn_mon(ad9680_core, ADC_PN9);
	ad9680_test(&ad9680_spi_device, AD9680_TEST_PN23);
	adc_pn_mon(ad9680_core, ADC_PN23A);

	// default data

	ad9144_channels[0].sel = DAC_SRC_DDS;
	ad9144_channels[1].sel = DAC_SRC_DDS;
	dac_data_setup(ad9144_core);
	ad9680_test(&ad9680_spi_device, AD9680_TEST_OFF);

	ad_printf("daq2: setup and configuration is done\n");

	//********************************************************************************
	// external loopback - capture data with DMA
	//********************************************************************************

	if(!dmac_start_transaction(ad9680_dma)){
		ad_printf("daq2: RX capture done.\n");
	};

	ad_platform_close();
	return(0);
}
