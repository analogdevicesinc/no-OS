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

#include "altxil_base.h"
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
#define AD9144_ADXCVR_BASEADDR  XPAR_AXI_AD9144_XCVR_BASEADDR
#define AD9680_ADXCVR_BASEADDR  XPAR_AXI_AD9680_XCVR_BASEADDR
#define AD9144_CORE_BASEADDR    XPAR_AXI_AD9144_CORE_BASEADDR
#define AD9680_CORE_BASEADDR    XPAR_AXI_AD9680_CORE_BASEADDR
#define AD9144_JESD_BASEADDR    XPAR_AXI_AD9144_JESD_BASEADDR
#define AD9680_JESD_BASEADDR    XPAR_AXI_AD9680_JESD_BASEADDR
#define AD9144_DMA_BASEADDR     XPAR_AXI_AD9144_DMA_BASEADDR
#define AD9680_DMA_BASEADDR     XPAR_AXI_AD9680_DMA_BASEADDR
#define AD9144_DMA_IRQ		XPAR_FABRIC_AXI_AD9144_DMA_IRQ_INTR
#define AD9680_DMA_IRQ		XPAR_FABRIC_AXI_AD9680_DMA_IRQ_INTR
#endif

#ifdef ZYNQ_PS7
#define ADC_DDR_BASEADDR        XPAR_DDR_MEM_BASEADDR + 0x0800000
#define DAC_DDR_BASEADDR        XPAR_DDR_MEM_BASEADDR + 0xA000000
#endif

#ifdef ZYNQ_PSU
#define ADC_DDR_BASEADDR        XPAR_PSU_DDR_0_S_AXI_BASEADDR + 0x0800000
#define DAC_DDR_BASEADDR        XPAR_PSU_DDR_0_S_AXI_BASEADDR + 0xA000000
#endif

#ifdef MICROBLAZE
#define ADC_DDR_BASEADDR        XPAR_AXI_DDR_CNTRL_BASEADDR + 0x0800000
#define DAC_DDR_BASEADDR        XPAR_AXI_DDR_CNTRL_BASEADDR + 0xA000000
#endif

#define GPIO_TRIG               43
#define GPIO_ADC_PD             42
#define GPIO_DAC_TXEN           41
#define GPIO_DAC_RESET          40
#define GPIO_CLKD_SYNC          38
#define GPIO_ADC_FDB            36
#define GPIO_ADC_FDA            35
#define GPIO_DAC_IRQ            34
#define GPIO_CLKD_STATUS_1      33
#define GPIO_CLKD_STATUS_0      32

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
		"CLKD_ADC_SYSREF", //extended_name
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
		"CLKD_DAC_SYSREF", //extended_name
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
		"DAC_CLK_FMC", //extended_name
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

enum ad9523_channels{
	DAC_CLK,
	ADC_CLK_FMC,
	ADC_SYSREF,
	CLKD_ADC_SYSREF,
	CLKD_DAC_SYSREF,
	DAC_SYSREF,
	DAC_CLK_FMC,
	ADC_CLK,
};

ad9144_init_param default_ad9144_init_param = {
	/* Device Settings */
	2,        // jesd_xbar_lane0_sel
	3,        // jesd_xbar_lane1_sel
	0,        // jesd_xbar_lane2_sel
	1,        // jesd_xbar_lane3_sel
	10000000    // lane_rate_khz
};

ad9680_init_param default_ad9680_init_param = {
	10000000    // lane_rate_khz
};

/***************************************************************************//**
 * @brief daq2_gpio_ctl
 *******************************************************************************/

void daq2_gpio_ctl(void)
{
	ad_gpio_set(GPIO_CLKD_SYNC, 1);
	ad_gpio_set(GPIO_DAC_RESET, 1);
	ad_gpio_set(GPIO_DAC_TXEN, 1);
	ad_gpio_set(GPIO_ADC_PD, 0);
	mdelay(250);
}

//#define DMA_EXAMPLE

/***************************************************************************//**
 * @brief main
 *******************************************************************************/
int main(void)
{
	int8_t		mode;
	// number of captured samples
	uint32_t	no_of_samples = 32768;
	int32_t		ret = 0;

	spi_device	ad9523_spi_device;
	spi_device	ad9144_spi_device;
	spi_device	ad9680_spi_device;

	dac_core	ad9144_core;
	jesd204_core	ad9144_jesd204;
	xcvr_core	ad9144_xcvr;
	dmac_core	ad9144_dma;
	adc_core	ad9680_core;
	jesd204_core	ad9680_jesd204;
	xcvr_core	ad9680_xcvr;
	dmac_core	ad9680_dma;
	dmac_xfer 	ad9680_xfer;

	ad_spi_init(&ad9523_spi_device);
	ad_spi_init(&ad9144_spi_device);
	ad_spi_init(&ad9680_spi_device);

	ad9523_spi_device.chip_select = 0x6;
	ad9144_spi_device.chip_select = 0x5;
	ad9680_spi_device.chip_select = 0x3;

	ad_dcache_flush();

	ad9144_core.dac_baseaddr = AD9144_CORE_BASEADDR;
	ad9144_core.no_of_channels = 2;
	ad9144_core.resolution = 16;
	ad9144_core.fifo_present = 1;

	ad9144_jesd204.base_addr = AD9144_JESD_BASEADDR;
	ad9144_jesd204.rx_tx_n = 0;
	ad9144_jesd204.octets_per_frame = 1;
	ad9144_jesd204.frames_per_multiframe = 32;
	ad9144_jesd204.subclass_mode = 1;

	ad9144_xcvr.base_addr = XPAR_AXI_AD9144_XCVR_BASEADDR;
	ad9144_xcvr.tx_enable = 1;
	ad9144_xcvr.gth_enable = 0;
	ad9144_xcvr.lpm_enable = 1;
	ad9144_xcvr.out_clk_sel = 4;

	ad9144_dma.baseaddr = AD9144_DMA_BASEADDR;
	ad9144_dma.type = DMAC_DOWNSTREAM;
	ad9144_dma.irq_id = AD9144_DMA_IRQ;

	ad9680_core.adc_baseaddr = AD9680_CORE_BASEADDR;
	ad9680_core.no_of_channels = 2;
	ad9680_core.resolution = 14;

	ad9680_jesd204.base_addr = AD9680_JESD_BASEADDR;
	ad9680_jesd204.rx_tx_n = 1;
	ad9680_jesd204.octets_per_frame = 1;
	ad9680_jesd204.frames_per_multiframe = 32;
	ad9680_jesd204.subclass_mode = 1;

	ad9680_xcvr.base_addr = XPAR_AXI_AD9680_XCVR_BASEADDR;
	ad9680_xcvr.tx_enable = 0;
	ad9680_xcvr.gth_enable = 0;
	ad9680_xcvr.lpm_enable = 1;
	ad9680_xcvr.out_clk_sel = 4;

	ad9680_dma.baseaddr = AD9144_DMA_BASEADDR;
	ad9680_dma.type = DMAC_UPSTREAM;
	ad9680_dma.irq_id = AD9680_DMA_IRQ;

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
			ad9523_pdata_lpc.pll2_vco_diff_m1 = 3;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 2;
			ad9523_channels[ADC_CLK].channel_divider = 1;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_khz = 10000000;
			ad9144_xcvr.ref_rate_khz = 500000;
			ad9680_xcvr.sys_clk_sel = 3;
			ad9680_xcvr.lane_rate_khz = 10000000;
			ad9680_xcvr.ref_rate_khz = 500000;
			break;
		case '2':
			ad9523_pdata_lpc.pll2_vco_diff_m1 = 3;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_khz = 10000000;
			ad9144_xcvr.ref_rate_khz = 500000;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_khz = 5000000;
			ad9680_xcvr.ref_rate_khz = 250000;
			break;
		case '3':
			ad9523_pdata_lpc.pll2_vco_diff_m1 = 3;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 4;
			ad9523_channels[DAC_CLK].channel_divider = 2;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 0;
			ad9144_xcvr.lane_rate_khz = 5000000;
			ad9144_xcvr.ref_rate_khz = 250000;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_khz = 5000000;
			ad9680_xcvr.ref_rate_khz = 250000;
			break;
		case '4':
			ad9523_pdata_lpc.pll2_vco_diff_m1 = 4;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 2;
			ad9523_channels[ADC_CLK].channel_divider = 1;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_khz = 7500000;
			ad9144_xcvr.ref_rate_khz = 375000;
			ad9680_xcvr.sys_clk_sel = 3;
			ad9680_xcvr.lane_rate_khz = 7500000;
			ad9680_xcvr.ref_rate_khz = 375000;
			break;
		case '5':
			ad9523_pdata_lpc.pll2_vco_diff_m1 = 4;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 2;
			ad9523_channels[DAC_CLK].channel_divider = 1;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 3;
			ad9144_xcvr.lane_rate_khz = 7500000;
			ad9144_xcvr.ref_rate_khz = 375000;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_khz = 3750000;
			ad9680_xcvr.ref_rate_khz = 187500;
			break;
		case '6':
			ad9523_pdata_lpc.pll2_vco_diff_m1 = 4;
			ad9523_channels[DAC_CLK_FMC].channel_divider = 4;
			ad9523_channels[DAC_CLK].channel_divider = 2;
			ad9523_channels[ADC_CLK_FMC].channel_divider = 4;
			ad9523_channels[ADC_CLK].channel_divider = 2;
			ad9144_xcvr.sys_clk_sel = 0;
			ad9144_xcvr.lane_rate_khz = 3750000;
			ad9144_xcvr.ref_rate_khz = 187500;
			ad9680_xcvr.sys_clk_sel = 0;
			ad9680_xcvr.lane_rate_khz = 3750000;
			ad9680_xcvr.ref_rate_khz = 187500;
			break;
		default:
			ad_printf ("Unknown selection.\n");
			return -1;
	}

	default_ad9144_init_param.lane_rate_khz = ad9144_xcvr.lane_rate_khz;
	default_ad9680_init_param.lane_rate_khz = ad9680_xcvr.lane_rate_khz;

	daq2_gpio_ctl();

	ad9523_setup(&ad9523_spi_device, ad9523_pdata_lpc);

	ad9144_setup(&ad9144_spi_device, default_ad9144_init_param);
	jesd204_init(ad9144_jesd204);
	xcvr_init(ad9144_xcvr);
	jesd204_read_status(ad9144_jesd204);

	ad9680_setup(&ad9680_spi_device, default_ad9680_init_param);
	jesd204_init(ad9680_jesd204);
	xcvr_init(ad9680_xcvr);
	jesd204_read_status(ad9680_jesd204);

	dac_setup(ad9144_core);

#ifdef DMA_EXAMPLE

	dmac_xfer ad9144_xfer;

	ad9144_xfer.startaddr = DAC_DDR_BASEADDR;
	ad9144_xfer.size = dac_buffer_load(ad9144_core, DAC_DDR_BASEADDR);
	ret = dmac_start_transaction(&ad9144_dma, &ad9144_xfer);
	if (ret == -1)
		ad_printf("ERROR AD9144 DMA transaction timed out!\r\n");
#else
	dds_set_frequency(ad9144_core, 0, 5000000);
	dds_set_phase(ad9144_core, 0, 0);
	dds_set_scale(ad9144_core, 0, 500000);
	dds_set_frequency(ad9144_core, 1, 5000000);
	dds_set_phase(ad9144_core, 1, 0);
	dds_set_scale(ad9144_core, 1, 500000);

	dds_set_frequency(ad9144_core, 2, 5000000);
	dds_set_phase(ad9144_core, 2, 90000);
	dds_set_scale(ad9144_core, 2, 500000);
	dds_set_frequency(ad9144_core, 3, 5000000);
	dds_set_phase(ad9144_core, 3, 90000);
	dds_set_scale(ad9144_core, 3, 500000);
#endif

	adc_setup(ad9680_core);

	ad_printf("Initialization done.\n");

	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_ADC_TEST_MODE, 0x05);
	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_OUTPUT_MODE, 0x0);

	adc_pn_mon(ad9680_core, 1);

	ad_printf("PRBS test done.\n");

	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_ADC_TEST_MODE, 0x0f);
	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_OUTPUT_MODE, 0x1);

	ad_printf("Ramp capture done.\n");

	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_DEVICE_INDEX, 0x3);
	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_ADC_TEST_MODE, 0x00);
	ad9680_spi_write(&ad9680_spi_device, AD9680_REG_OUTPUT_MODE, 0x1);


	ad9680_xfer.startaddr = ADC_DDR_BASEADDR;
	ad9680_xfer.size = (no_of_samples * ad9680_core.no_of_channels * ((ad9680_core.resolution + 7) / 8));
	dmac_start_transaction(ad9680_dma, &ad9680_xfer);
	if (ret == -1)
		ad_printf("ERROR AD9144 DMA transaction timed out!\r\n");

	ad_printf("Test mode off capture done.\n");

	ad_dcache_flush();

	return 0;
}
