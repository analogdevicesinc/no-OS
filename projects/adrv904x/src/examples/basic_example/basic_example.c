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

#include "adrv904x.h"
#include "ad9528.h"

#include "adi_adrv904x_datainterface.h"

#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_rx.h"
#include "no_os_axi_io.h"
#include "adi_adrv904x_radioctrl.h"

#define DAC_BUFFER_SAMPLES              8192
#define ADC_BUFFER_SAMPLES              32768
#define ADC_CHANNELS                    8

uint32_t dac_buffer_dma[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
uint16_t adc_buffer_dma[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((
                       aligned));


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
	struct adrv904x_init_param adrv904x_init_par = { 0 };
	struct adi_adrv904x_Device adrv904x_device = { 0 };
	struct ad9528_platform_data ad9528_pdata = { 0 };
	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param;
	struct ad9528_dev* ad9528_device;
	struct adrv904x_rf_phy *phy;
	int status;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

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
	strcpy(ad9528_channels[0].extended_name, "DEV_SYSREF");
	ad9528_channels[0].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[0].divider_phase = 0;
	ad9528_channels[0].channel_divider = 4;
	ad9528_channels[0].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[0].output_dis = 0;

	// channel 1
	ad9528_channels[1].channel_num = 1;
	strcpy(ad9528_channels[1].extended_name, "DEV_CLK");
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].divider_phase = 0;
	ad9528_channels[1].channel_divider = 4;
	ad9528_channels[1].signal_source = SOURCE_VCO;
	ad9528_channels[1].output_dis = 0;

	// channel 3
	ad9528_channels[3].channel_num = 3;
	strcpy(ad9528_channels[3].extended_name, "CORE_CLK");
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].divider_phase = 0;
	ad9528_channels[3].channel_divider = 4;
	ad9528_channels[3].signal_source = SOURCE_VCO;
	ad9528_channels[3].output_dis = 0;

	// channel 11
	ad9528_channels[11].channel_num = 11;
	strcpy(ad9528_channels[11].extended_name, "REF_CLK1");
	ad9528_channels[11].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[11].divider_phase = 0;
	ad9528_channels[11].channel_divider = 2;
	ad9528_channels[11].signal_source = SOURCE_VCO;
	ad9528_channels[11].output_dis = 0;

	// channel 12
	ad9528_channels[12].channel_num = 12;
	strcpy(ad9528_channels[12].extended_name, "FPGA_SYSREF");
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].divider_phase = 0;
	ad9528_channels[12].channel_divider = 4;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[12].output_dis = 0;

	// channel 13
	ad9528_channels[13].channel_num = 13;
	strcpy(ad9528_channels[13].extended_name, "REF_CLK0");
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].divider_phase = 0;
	ad9528_channels[13].channel_divider = 2;
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
	ad9528_param.pdata->pll2_charge_pump_current_nA = 815000;
	ad9528_param.pdata->pll2_vco_div_m1 = 4;
	ad9528_param.pdata->pll2_r1_div = 1;
	ad9528_param.pdata->pll2_ndiv_a_cnt = 3;
	ad9528_param.pdata->pll2_ndiv_b_cnt = 27;
	ad9528_param.pdata->pll2_n2_div = 4;
	ad9528_param.pdata->pll2_freq_doubler_en = 1;
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
	ad9528_param.pdata->stat0_pin_func_sel = 0x9; /* PLL1 in holdover */
	ad9528_param.pdata->stat1_pin_func_sel = 3; /* 7 - REFA Correct; 3 - PLL2 Locked */

	status = ad9528_setup(&ad9528_device, ad9528_param);
	if (status != 0) {
		printf("error: ad9528_setup() failed\n");
		goto error;
	}

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 16
	};
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL,
		.rate = 3,
		.num_channels = 16
	};

	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = ADRV904X_TX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV904X_TX_JESD_FRAMES_PER_MULTIFRAME,
		.converters_per_device = ADRV904X_TX_JESD_CONVS_PER_DEVICE,
		.converter_resolution = ADRV904X_TX_JESD_CONV_RESOLUTION,
		.bits_per_sample = ADRV904X_TX_JESD_BITS_PER_SAMPLE,
		.high_density = ADRV904X_TX_JESD_HIGH_DENSITY,
		.control_bits_per_sample = ADRV904X_TX_JESD_CTRL_BITS_PER_SAMPLE,// optional
		.subclass = ADRV904X_TX_JESD_SUBCLASS,
		.device_clk_khz = ADRV904X_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV904X_LANE_RATE_KHZ
	};

	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = ADRV904X_RX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV904X_RX_JESD_FRAMES_PER_MULTIFRAME,
		.subclass = ADRV904X_RX_JESD_SUBCLASS,
		.device_clk_khz = ADRV904X_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV904X_LANE_RATE_KHZ
	};

	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = ADRV904X_LANE_RATE_KHZ,
		.ref_rate_khz = 491520,
		.export_no_os_clk = true
	};
	struct adxcvr *tx_adxcvr;

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = ADRV904X_LANE_RATE_KHZ,
		.ref_rate_khz = 491520,
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

	adrv904x_init_par.adrv904x_device = &adrv904x_device;
	adrv904x_init_par.dev_clk = ad9528_device->clk_desc[1];

	status = adrv904x_init(&phy, &adrv904x_init_par);
	if (status) {
		pr_err("error: adrv904x_init() failed\n");
		goto error_5;
	}

	status = axi_dac_init(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_6;
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_7;

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	status = adrv904x_post_setup(phy);
	if (status) {
		pr_err("error: adrv904x_post_setup() failed\n");
		goto error_8;
	}

	status = axi_dac_set_datasel(phy->tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
	if (status) {
		pr_err("error: axi_dac_set_datasel() failed\n");
		goto error_8;
	}
	
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

//	extern const uint32_t sine_lut_iq[1024];
//	axi_dac_load_custom_data(phy->tx_dac, sine_lut_iq,
//					 NO_OS_ARRAY_SIZE(sine_lut_iq),
//					 (uintptr_t)dac_buffer_dma);

//	uint32_t data;
//	no_os_axi_io_read(0x9C450000, 0x0010, &data);
//	printf("	Rx data offload SYNTHESIS_CONFIG %#x\n", data);
//
//	no_os_axi_io_read(0x9C440000, 0x0010, &data);
//	printf("	Tx data offload SYNTHESIS_CONFIG %#x\n", data);
//
//	// bypass Rx data offload
//	no_os_axi_io_read(0x9C450000, 0x0088, &data);
//	printf("	Rx data offload bypass %#x\n", data);
//	data |= 0x01;
//	no_os_axi_io_write(0x9C450000, 0x0088, data);
//	no_os_axi_io_read(0x9C450000, 0x0088, &data);
//	printf("	Rx data offload bypass %#x\n", data);
//
//	// bypass Tx data offload
//	no_os_axi_io_read(0x9C440000, 0x0088, &data);
//	printf("	Tx data offload bypass %#x\n", data);
//	data |= 0x01;
//	no_os_axi_io_write(0x9C440000, 0x0088, data);
//	no_os_axi_io_read(0x9C440000, 0x0088, &data);
//	printf("	Tx data offload bypass %#x\n", data);

	struct axi_dma_transfer transfer = {
		// Number of bytes to write/read
		.size = sizeof(sine_lut_iq),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = CYCLIC,
		// Address of data source
		.src_addr = (uintptr_t)dac_buffer_dma,
		// Address of data destination
		.dest_addr = 0
	};

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
	
	adi_adrv904x_AdcSampleXbarCfg_t adcXbar = { 0 };
	status = adi_adrv904x_AdcSampleXbarGet(phy->kororDevice, ADI_ADRV904X_FRAMER_0, &adcXbar);
	if (status) {
		pr_err("error: adi_adrv904x_AdcSampleXbarGet() failed\n");
		goto error_10;
	}

	adcXbar.AdcSampleXbar[0] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[1] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_Q;
	adcXbar.AdcSampleXbar[2] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[3] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_Q;
	adcXbar.AdcSampleXbar[4] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[5] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_Q;
	adcXbar.AdcSampleXbar[6] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[7] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_Q;
	adcXbar.AdcSampleXbar[8] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[9] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_Q;
	adcXbar.AdcSampleXbar[10] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[11] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_Q;
	adcXbar.AdcSampleXbar[12] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[13] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_Q;
	adcXbar.AdcSampleXbar[14] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_I;
	adcXbar.AdcSampleXbar[15] = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_Q;

	status = adi_adrv904x_AdcSampleXbarSet(phy->kororDevice, ADI_ADRV904X_FRAMER_0, &adcXbar);
	if (status) {
		pr_err("error: adi_adrv904x_AdcSampleXbarSet() failed\n");
		goto error_10;
	}

	adi_adrv904x_TxAtten_t txAttenuation = { 0 };

	for (uint8_t chan = 0; chan < 8; chan++) {
		status = adi_adrv904x_TxAttenGet(phy->kororDevice, ADI_ADRV904X_TX0 << chan, &txAttenuation);
		if (status) {
			pr_err("error: adi_adrv904x_TxAttenGet() failed\n");
			goto error_10;
		}

		if (ADI_ADRV904X_TX0 << chan == txAttenuation.txChannelMask)
			printf("gain for Tx channel %d %d\n", chan, txAttenuation.txAttenuation_mdB);
		else
			printf("ERROR\n");
	}

	adi_adrv904x_TxTestNcoConfig_t txNcoConfig = {ADI_ADRV904X_TX7,                       /* chanSelect */
												  0U,                                     /* bandSelect */
												  1U,                                     /* enable */
												  ADI_ADRV904X_TX_TEST_NCO_0,             /* ncoSelect */
												  0U,                                     /* phase */
												  10000,                                  /* frequencyKhz */
												  ADI_ADRV904X_TX_TEST_NCO_ATTEN_6DB     /* attenCtrl */
   };

   status = adi_adrv904x_TxTestToneSet(phy->kororDevice, &txNcoConfig);
   if (status)
		   printf("ERR\n");

   uint32_t orxChannelMask;
   uint32_t rxChannelMask;
   uint32_t txChannelMask;
   adi_adrv904x_ChannelEnableGet(phy->kororDevice, &orxChannelMask, &rxChannelMask, &txChannelMask);
   printf("orx rx tx state %x %x %x\n", orxChannelMask, rxChannelMask, txChannelMask);

//   adi_adrv904x_FramerLoopbackSet(phy->kororDevice, ADI_ADRV904X_FRAMER_0);
//   adi_adrv904x_DeframerLoopbackSet(phy->kororDevice);

   adi_adrv904x_RxGain_t rxGain =  { ADI_ADRV904X_RX7, 250U };

   for (uint8_t chan = 0; chan < 8; chan++) {
		status = adi_adrv904x_RxMgcGainGet(phy->kororDevice,
				ADI_ADRV904X_RX0 << chan,
				&rxGain);
		if (ADI_ADRV904X_TX0 << chan == rxGain.rxChannelMask)
			printf("Initial gain for channel %d %d\n", chan, rxGain.gainIndex);
		else
			printf("ERROR\n");

		rxGain.rxChannelMask = ADI_ADRV904X_TX0 << chan;
		rxGain.gainIndex = 240;

		status = adi_adrv904x_RxGainSet(phy->kororDevice, &rxGain, 1U);

		rxGain.gainIndex = 0;

		status = adi_adrv904x_RxMgcGainGet(phy->kororDevice,
				ADI_ADRV904X_TX0 << chan,
			   &rxGain);

		if (ADI_ADRV904X_TX0 << chan == rxGain.rxChannelMask)
			printf("gain for channel %d %d\n", chan, rxGain.gainIndex);
		else
			printf("ERROR\n");
   }

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

//	/* Transfer the data. */
//	axi_dmac_transfer_start(tx_dmac, &transfer);
//
//	/* Flush cache data. */
//	Xil_DCacheInvalidateRange((uintptr_t)dac_buffer_dma, sizeof(sine_lut_iq));
//
//	no_os_mdelay(1000);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	 struct axi_dma_transfer read_transfer = {
			 // Number of bytes to write/read
			 .size = sizeof(adc_buffer_dma),
			 // Transfer done flag
			 .transfer_done = 0,
			 // Signal transfer mode
			 .cyclic = NO,
			 // Address of data source
			 .src_addr = 0,
			 // Address of data destination
			 .dest_addr = (uintptr_t)adc_buffer_dma
	 };

	 /* Read the data from the ADC DMA. */
	 axi_dmac_transfer_start(rx_dmac, &read_transfer);

	 /* Wait until transfer finishes */
	 status = axi_dmac_transfer_wait_completion(rx_dmac, 1000);
	 if(status)
			 goto error_10;

	 Xil_DCacheInvalidateRange((uintptr_t)adc_buffer_dma, sizeof(adc_buffer_dma));
	 pr_info("DMA_EXAMPLE Rx: address=%#lx samples=%lu channels=%u bits=%lu\n",
			 (uintptr_t)adc_buffer_dma, NO_OS_ARRAY_SIZE(adc_buffer_dma),
			 rx_adc_init.num_channels,
			 8 * sizeof(adc_buffer_dma[0]));


	while(1);

	jesd204_fsm_stop(topology, JESD204_LINKS_ALL);

error_10:
	axi_dmac_remove(rx_dmac);
error_9:
	axi_dmac_remove(tx_dmac);
error_8:
	axi_adc_remove(phy->rx_adc);
error_7:
	axi_dac_remove(phy->tx_dac);
error_6:
	adrv904x_remove(phy);
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
