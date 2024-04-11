/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for the adrv902x project
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "basic_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_spi.h"

#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_adxcvr.h"
#include "axi_dmac.h"

#include "xilinx_gpio.h"
#include "xilinx_spi.h"

#include "adrv9025.h"
#include "ad9528.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int basic_example_main(void)
{
	struct adrv9025_init_param adrv9025_init_par = { 0 };
	struct adi_adrv9025_Device adrv9025_device = { 0 };
	struct ad9528_platform_data ad9528_pdata = { 0 };
	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param;
	struct ad9528_dev* ad9528_device;
	struct adrv9025_rf_phy *phy;
	int status;

	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *rx_dmac;
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *tx_dmac;

	ad9528_param.spi_init = ad9528_spi_param;

	// Export no_os_clk_desc for each channel
	ad9528_param.export_no_os_clk = true;

	// ad9528 defaults
	ad9528_param.gpio_resetb = &clkchip_gpio_init_param;
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 14;
	ad9528_param.pdata->channels = &ad9528_channels[0];
	ad9528_init(&ad9528_param);

	// channel 0
	ad9528_channels[0].channel_num = 0;
	ad9528_channels[0].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[0].divider_phase = 0;
	ad9528_channels[0].channel_divider = 5;
	ad9528_channels[0].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[0].output_dis = 0;

	// channel 1
	ad9528_channels[1].channel_num = 1;
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].divider_phase = 0;
	ad9528_channels[1].channel_divider = 5;
	ad9528_channels[1].signal_source = SOURCE_VCO;
	ad9528_channels[1].output_dis = 0;

	// channel 3
	ad9528_channels[3].channel_num = 3;
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].divider_phase = 0;
	ad9528_channels[3].channel_divider = 5;
	ad9528_channels[3].signal_source = SOURCE_VCO;
	ad9528_channels[3].output_dis = 0;

	// channel 12
	ad9528_channels[12].channel_num = 12;
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].divider_phase = 0;
	ad9528_channels[12].channel_divider = 5;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[12].output_dis = 0;

	// channel 13
	ad9528_channels[13].channel_num = 13;
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].divider_phase = 0;
	ad9528_channels[13].channel_divider = 5;
	ad9528_channels[13].signal_source = SOURCE_VCO;
	ad9528_channels[13].output_dis = 0;

	// pllx settings
	ad9528_param.pdata->spi3wire = 0;
	ad9528_param.pdata->vcxo_freq = 122880000;
	ad9528_param.pdata->osc_in_diff_en = 0;
	ad9528_param.pdata->refa_en = 1;
	ad9528_param.pdata->refa_diff_rcv_en = 1;
	ad9528_param.pdata->refb_diff_rcv_en = 0;
	ad9528_param.pdata->osc_in_diff_en = 0;
	/* JESD */
	ad9528_param.pdata->jdev_desired_sysref_freq = 7680000 / 2;
	/* PLL1 config */
	ad9528_param.pdata->pll1_feedback_div = 4;
	ad9528_param.pdata->pll1_charge_pump_current_nA = 5000;
	ad9528_param.pdata->ref_mode = REF_MODE_EXT_REF;
	/* PLL2 config */
	ad9528_param.pdata->pll2_charge_pump_current_nA = 805000;
	ad9528_param.pdata->pll2_vco_div_m1 = 3;
	ad9528_param.pdata->pll2_r1_div = 1;
	ad9528_param.pdata->pll2_ndiv_a_cnt = 3;
	ad9528_param.pdata->pll2_ndiv_b_cnt = 27;
	ad9528_param.pdata->pll2_n2_div = 10;
	/* SYSREF config */
	ad9528_param.pdata->sysref_src = SYSREF_SRC_INTERNAL;
	ad9528_param.pdata->sysref_k_div = 512;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_NSHOT;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_8_PULSES;
	ad9528_param.pdata->sysref_req_trigger_mode = SYSREF_LEVEL_HIGH;
	ad9528_param.pdata->sysref_req_en = false;
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	ad9528_param.pdata->pll1_bypass_en = false;
	ad9528_param.pdata->pll2_bypass_en = false;
	ad9528_param.pdata->stat0_pin_func_sel = 1; /* PLL1 & PLL2 Locked */
	ad9528_param.pdata->stat1_pin_func_sel = 7; /* REFA Correct */

	status = ad9528_setup(&ad9528_device, ad9528_param);
	if (status != 0) {
		printf("error: ad9528_setup() failed\n");
		goto error;
	}

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 8
	};
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL,
		.rate = 3,
		.num_channels = 8
	};

	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = ADRV9025_TX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV9025_TX_JESD_FRAMES_PER_MULTIFRAME,
		.converters_per_device = ADRV9025_TX_JESD_CONVS_PER_DEVICE,
		.converter_resolution = ADRV9025_TX_JESD_CONV_RESOLUTION,
		.bits_per_sample = ADRV9025_TX_JESD_BITS_PER_SAMPLE,
		.high_density = ADRV9025_TX_JESD_HIGH_DENSITY,
		.control_bits_per_sample = ADRV9025_TX_JESD_CTRL_BITS_PER_SAMPLE,// optional
		.subclass = ADRV9025_TX_JESD_SUBCLASS,
		.device_clk_khz = ADRV9025_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV9025_LANE_RATE_KHZ
	};

	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = ADRV9025_RX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV9025_RX_JESD_FRAMES_PER_MULTIFRAME,
		.subclass = ADRV9025_RX_JESD_SUBCLASS,
		.device_clk_khz = ADRV9025_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV9025_LANE_RATE_KHZ
	};

	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = ADRV9025_LANE_RATE_KHZ,
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ,
		.export_no_os_clk = true
	};
	struct adxcvr *tx_adxcvr;

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 1,
		.lane_rate_khz = ADRV9025_LANE_RATE_KHZ,
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ,
		.export_no_os_clk = true
	};
	struct adxcvr *rx_adxcvr;

	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status)
		goto error_1;

	status = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (status)
		goto error_2;

	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;

	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;
	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;

	status = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (status)
		goto error_3;

	status = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (status)
		goto error_4;

	adrv9025_init_par.adrv9025_device = &adrv9025_device;
	adrv9025_init_par.dev_clk = ad9528_device->clk_desc[1];
	adrv9025_init_par.streamImageFile = ADRV9025_STREAM_IMAGE_FILE;

	status = adrv9025_init(&phy, &adrv9025_init_par);
	if (status) {
		pr_err("error: adrv9025_init() failed\n");
		goto error_5;
	}

	status = axi_dac_init_begin(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_6;
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_7;

	// Reset Tx DAC
	axi_adc_write(phy->rx_adc, 0x4040, 0);
	axi_adc_write(phy->rx_adc, 0x4040,
		      NO_OS_BIT(1) | NO_OS_BIT(0));

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	status = adrv9025_post_setup(phy);
	if (status) {
		pr_err("error: adrv9025_post_setup() failed\n");
		goto error_8;
	}

	// Set DDS data
	axi_dac_data_setup(phy->tx_dac);

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		printf("axi_dmac_init tx init error: %d\n", status);
		goto error_8;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		printf("axi_dmac_init rx init error: %d\n", status);
		goto error_9;
	}

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = ad9528_device->jdev,
			.link_ids = {DEFRAMER0_LINK_TX, FRAMER0_LINK_RX},
			.links_number = 2,
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER0_LINK_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER0_LINK_TX},
			.links_number = 1,
		},
		{
			.jdev = phy->jdev,
			.link_ids = {DEFRAMER0_LINK_TX, FRAMER0_LINK_RX},
			.links_number = 2,
			.is_top_device = true,
		},
	};

	jesd204_topology_init(&topology, devs,
			      sizeof(devs)/sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	axi_dmac_remove(rx_dmac);
error_9:
	axi_dmac_remove(tx_dmac);
error_8:
	axi_adc_remove(phy->rx_adc);
error_7:
	axi_dac_remove(phy->tx_dac);
error_6:
	adi_adrv9025_HwClose(phy->madDevice);
	adrv9025_remove(phy);
error_5:
	axi_jesd204_rx_remove(rx_jesd);
error_4:
	axi_jesd204_tx_remove(tx_jesd);
error_3:
	adxcvr_remove(rx_adxcvr);
error_2:
	adxcvr_remove(tx_adxcvr);
error_1:
	ad9528_remove(ad9528_device);

	/* If we are here and status non-zero, diplay status */
	if (status)
		goto error;

	printf("Bye \n");

	return 0;

error:
	printf("Error %d.\n", status);
	return status;
}
