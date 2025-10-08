/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the adrv902x project
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "iio_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
#include "xilinx_uart.h"
#include "xilinx_spi.h"
#include "xil_cache.h"

#include "clkgen_routines.h"
#include "adrv9025.h"
#include "ad9528.h"

#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_app.h"


uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__((aligned(1024)));
uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADRV9025_RX_JESD_CONVS_PER_DEVICE]
__attribute__((aligned(1024)));
#ifdef ORX_DMA_BASEADDR
uint16_t adc1_buffer[ADC_BUFFER_SAMPLES * ADRV9025_ORX_JESD_CONVS_PER_DEVICE]
__attribute__((aligned(1024)));
#endif

/***************************************************************************//**
 * @brief Set AGC parameters in initialization structure.
*******************************************************************************/
int adrv9025_set_agc_params(adi_adrv9025_AgcCfg_t *agcConfig_init_param);

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return status - Result of the example execution.
*******************************************************************************/
int iio_example_main(void)
{
	struct adrv9025_init_param adrv9025_init_par = { 0 };
	struct adi_adrv9025_Device adrv9025_device = { 0 };
	adi_adrv9025_AgcCfg_t agcConfig_init_param = { 0 };
	struct ad9528_platform_data ad9528_pdata = { 0 };
	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param;
	struct axi_clkgen *orx_clkgen = NULL;
	struct axi_clkgen *rx_clkgen = NULL;
	struct axi_clkgen *tx_clkgen = NULL;
	struct ad9528_dev* ad9528_device;
	struct adrv9025_rf_phy *phy;
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

#ifdef ORX_DMA_BASEADDR
	struct axi_dmac_init orx_dmac_init = {
		"orx_dmac",
		ORX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *orx_dmac;
#endif

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
	ad9528_channels[0].channel_divider = 4;
	ad9528_channels[0].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[0].output_dis = 0;

	// channel 1
	ad9528_channels[1].channel_num = 1;
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].divider_phase = 0;
	ad9528_channels[1].channel_divider = 4;
	ad9528_channels[1].signal_source = SOURCE_VCO;
	ad9528_channels[1].output_dis = 0;

	// channel 3
	ad9528_channels[3].channel_num = 3;
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].divider_phase = 0;
	ad9528_channels[3].channel_divider = 4;
	ad9528_channels[3].signal_source = SOURCE_VCO;
	ad9528_channels[3].output_dis = 0;

	// channel 12
	ad9528_channels[12].channel_num = 12;
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].divider_phase = 0;
	ad9528_channels[12].channel_divider = 4;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[12].output_dis = 0;

	// channel 13
	ad9528_channels[13].channel_num = 13;
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].divider_phase = 0;
#ifndef JESD204C_PROFILE
	ad9528_channels[13].channel_divider = 4;
#else
	ad9528_channels[13].channel_divider = 2;
#endif
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
	ad9528_param.pdata->jdev_desired_sysref_freq = 7680000 / 32;
	/* PLL1 config */
	ad9528_param.pdata->pll1_feedback_div = 4;
	ad9528_param.pdata->pll1_charge_pump_current_nA = 5000;
	ad9528_param.pdata->ref_mode = REF_MODE_EXT_REF;
	/* PLL2 config */
	ad9528_param.pdata->pll2_charge_pump_current_nA = 805000;
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
		.num_channels = ADRV9025_RX_JESD_CONVS_PER_DEVICE
	};
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL,
		.rate = 3,
		.num_channels = ADRV9025_TX_JESD_CONVS_PER_DEVICE
	};

#ifdef ORX_CORE_BASEADDR
	struct axi_adc_init orx_adc_init = {
		.name = "orx_adc",
		.base = ORX_CORE_BASEADDR,
		.num_channels = ADRV9025_ORX_JESD_CONVS_PER_DEVICE
	};
#endif

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

#ifdef ORX_JESD_BASEADDR
	struct jesd204_rx_init orx_jesd_init = {
		.name = "orx_jesd",
		.base = ORX_JESD_BASEADDR,
		.octets_per_frame = ADRV9025_ORX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV9025_ORX_JESD_FRAMES_PER_MULTIFRAME,
		.subclass = ADRV9025_RX_JESD_SUBCLASS,
		.device_clk_khz = ADRV9025_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV9025_LANE_RATE_KHZ
	};
#endif

	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = ADRV9025_LANE_RATE_KHZ,
#ifndef JESD204C_PROFILE
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ,
#else
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ * 2,
#endif
		.export_no_os_clk = true
	};
	struct adxcvr *tx_adxcvr;

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
#ifndef JESD204C_PROFILE
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
#else
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
#endif
		.out_clk_sel = ADXCVR_REFCLK,
#ifndef JESD204C_PROFILE
		.lpm_enable = 1,
#else
		.lpm_enable = 0,
#endif
		.lane_rate_khz = ADRV9025_LANE_RATE_KHZ,
#ifndef JESD204C_PROFILE
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ,
#else
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ * 2,
#endif
		.export_no_os_clk = true
	};
	struct adxcvr *rx_adxcvr;

#ifdef ORX_XCVR_BASEADDR
	struct adxcvr_init orx_adxcvr_init = {
		.name = "orx_adxcvr",
		.base = ORX_XCVR_BASEADDR,
#ifndef JESD204C_PROFILE
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
#else
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
#endif
		.out_clk_sel = ADXCVR_REFCLK,
#ifndef JESD204C_PROFILE
		.lpm_enable = 1,
#else
		.lpm_enable = 0,
#endif
		.lane_rate_khz = ADRV9025_LANE_RATE_KHZ,
#ifndef JESD204C_PROFILE
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ,
#else
		.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ * 2,
#endif
		.export_no_os_clk = true
	};
	struct adxcvr *orx_adxcvr;
#endif

	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status)
		goto error_1;

	status = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (status)
		goto error_2;

#ifdef ORX_XCVR_BASEADDR
	status = adxcvr_init(&orx_adxcvr, &orx_adxcvr_init);
	if (status)
		goto error_3;
#endif

	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
#ifdef ORX_JESD_BASEADDR
	struct axi_jesd204_rx *orx_jesd;
#endif

	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;
	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;
#ifdef ORX_XCVR_BASEADDR
	orx_jesd_init.lane_clk = orx_adxcvr->clk_out;
#endif

	status = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (status)
		goto error_4;

	status = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (status)
		goto error_5;

#ifdef ORX_JESD_BASEADDR
	status = axi_jesd204_rx_init(&orx_jesd, &orx_jesd_init);
	if (status)
		goto error_6;
#endif

	adrv9025_init_par.adrv9025_device = &adrv9025_device;
	adrv9025_init_par.dev_clk = ad9528_device->clk_desc[1];
	adrv9025_init_par.streamImageFile = ADRV9025_STREAM_IMAGE_FILE;

	adrv9025_set_agc_params(&agcConfig_init_param);

	adrv9025_init_par.agcConfig_init_param = &agcConfig_init_param;

	status = adrv9025_init(&phy, &adrv9025_init_par);
	if (status) {
		pr_err("error: adrv9025_init() failed\n");
		goto error_7;
	}

	status = axi_dac_init_begin(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_8;
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_9;

#ifdef ORX_CORE_BASEADDR
	status = axi_adc_init_begin(&phy->orx_adc, &orx_adc_init);
	if (status)
		goto error_10;
#else
	phy->orx_adc = NULL;
#endif

	// Reset Tx DAC
	axi_adc_write(phy->rx_adc, 0x4040, 0);
	axi_adc_write(phy->rx_adc, 0x4040,
		      NO_OS_BIT(1) | NO_OS_BIT(0));

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

#ifdef ORX_CORE_BASEADDR
	// Reset ORx ADC
	axi_adc_write(phy->orx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->orx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);
#endif

	no_os_mdelay(200);

	status = clkgen_setup(&rx_clkgen, &tx_clkgen, &orx_clkgen,
			      phy->deviceInitStruct.dataInterface.deframer[0].enableJesd204C);
	if (status)
		goto error_11;

	status = adrv9025_post_setup(phy);
	if (status) {
		pr_err("error: adrv9025_post_setup() failed\n");
		goto error_12;
	}

	// Set DDS data
	axi_dac_data_setup(phy->tx_dac);

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		printf("axi_dmac_init tx init error: %d\n", status);
		goto error_12;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		printf("axi_dmac_init rx init error: %d\n", status);
		goto error_13;
	}

#ifdef ORX_DMA_BASEADDR
	status = axi_dmac_init(&orx_dmac, &orx_dmac_init);
	if (status) {
		printf("axi_dmac_init orx init error: %d\n", status);
		goto error_14;
	}
#endif

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
#ifdef ORX_JESD_BASEADDR
		{
			.jdev = orx_jesd->jdev,
			.link_ids = {FRAMER1_LINK_RX},
			.links_number = 1,
		},
#endif
		{
			.jdev = phy->jdev,
#ifdef ORX_JESD_BASEADDR
			.link_ids = {DEFRAMER0_LINK_TX, FRAMER0_LINK_RX, FRAMER1_LINK_RX},
			.links_number = 3,
#else
			.link_ids = {DEFRAMER0_LINK_TX, FRAMER0_LINK_RX},
			.links_number = 2,
#endif
			.is_top_device = true,
		},
	};

	status = adi_adrv9025_HwOpen(phy->madDevice, &phy->spiSettings);
	if (status) {
		pr_err("error: adi_adrv9025_HwOpen() failed\n");
		goto error_15;
	}

	jesd204_topology_init(&topology, devs,
			      sizeof(devs) / sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);
#ifdef ORX_JESD_BASEADDR
	axi_jesd204_rx_status_read(orx_jesd);

	// Enable ORx1 and ORx3
	uint32_t rxChannelMask = 0;
	uint32_t txChannelMask = 0;

	status = adi_adrv9025_RxTxEnableGet(phy->madDevice, &rxChannelMask,
					    &txChannelMask);
	if (status) {
		pr_err("error: adi_adrv9025_RxTxEnableGet() failed\n");
		goto error_15;
	}

	rxChannelMask |= ADI_ADRV9025_ORX1;

	status = adi_adrv9025_RxTxEnableSet(phy->madDevice, rxChannelMask,
					    txChannelMask);
	if (status) {
		pr_err("error: adi_adrv9025_RxTxEnableSet() failed\n");
		goto error_15;
	}

#endif

	adi_adrv9025_RxAgcMode_t gainMode;
	gainMode.rxChannelMask = ADI_ADRV9025_RX1;
	gainMode.agcMode = ADI_ADRV9025_MGC;
	status = adi_adrv9025_RxGainCtrlModeSet(phy->madDevice, &gainMode, 1);
	if (status) {
		printf("adi_adrv9025_RxGainCtrlModeSet failed: %d\n", status);
		goto error_15;
	}

	/**
	 * UART init params
	 */
	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	/**
	 * IIO application descriptor
	 */
	struct iio_app_desc *app;

	/**
	 * IIO application initialization parameters
	 */
	struct iio_app_init_param app_init_param = { 0 };

	/**
	 * iio axi adc configurations.
	 */
	struct iio_axi_adc_init_param iio_axi_adc_init_par;

	/**
	 * iio axi dac configurations.
	 */
	struct iio_axi_dac_init_param iio_axi_dac_init_par;

	/**
	 * iio instance descriptor.
	 */
	struct iio_axi_adc_desc *iio_axi_adc_desc;
#ifdef ORX_JESD_BASEADDR
	struct iio_axi_adc_desc *iio_axi_adc1_desc;
#endif

	/**
	 * iio instance descriptor.
	 */
	struct iio_axi_dac_desc *iio_axi_dac_desc;

	/**
	 * iio device instance descriptor.
	 */
	struct iio_device *adc_dev_desc, *dac_dev_desc;
#ifdef ORX_JESD_BASEADDR
	struct iio_device *adc1_dev_desc;
#endif

	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = phy->rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
#endif
	};

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		goto error_15;

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = sizeof(adc_buffer),
	};
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

#ifdef ORX_JESD_BASEADDR
	iio_axi_adc_init_par.rx_adc = phy->orx_adc;
	iio_axi_adc_init_par.rx_dmac = orx_dmac;

	status = iio_axi_adc_init(&iio_axi_adc1_desc, &iio_axi_adc_init_par);
	if (status < 0)
		goto error_16;

	struct iio_data_buffer read_buff1 = {
		.buff = (void *)adc1_buffer,
		.size = sizeof(adc1_buffer),
	};
	iio_axi_adc_get_dev_descriptor(iio_axi_adc1_desc, &adc1_dev_desc);
#endif

	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = phy->tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};

	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (status < 0)
		goto error_17;
	struct iio_data_buffer write_buff = {
		.buff = (void *)dac_buffer,
		.size = sizeof(dac_buffer),
	};
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("cf-adrv9025-lpc", iio_axi_adc_desc, adc_dev_desc, &read_buff, NULL, NULL),
#ifdef ORX_JESD_BASEADDR
		IIO_APP_DEVICE("cf-adrv9025-lpc1", iio_axi_adc1_desc, adc1_dev_desc, &read_buff1, NULL, NULL),
#endif
		IIO_APP_DEVICE("cf-adrv9025-dds-core-lpc", iio_axi_dac_desc, dac_dev_desc, NULL, &write_buff, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	/* Wait to display previous messages */
	no_os_mdelay(100);

	status = iio_app_init(&app, app_init_param);
	if (status)
		goto error_18;

	return iio_app_run(app);

error_18:
	iio_axi_dac_remove(iio_axi_dac_desc);
error_17:
#ifdef ORX_JESD_BASEADDR
	iio_axi_adc_remove(iio_axi_adc1_desc);
error_16:
#endif
	iio_axi_adc_remove(iio_axi_adc_desc);
error_15:
#ifdef ORX_DMA_BASEADDR
	axi_dmac_remove(orx_dmac);
error_14:
#endif
	axi_dmac_remove(rx_dmac);
error_13:
	axi_dmac_remove(tx_dmac);
error_12:
	if (!status)
		clkgen_remove(rx_clkgen, tx_clkgen, orx_clkgen);
error_11:
#ifdef ORX_CORE_BASEADDR
	axi_adc_remove(phy->orx_adc);
error_10:
#endif
	axi_adc_remove(phy->rx_adc);
error_9:
	axi_dac_remove(phy->tx_dac);
error_8:
	adrv9025_remove(phy);
error_7:
#ifdef ORX_JESD_BASEADDR
	axi_jesd204_rx_remove(orx_jesd);
error_6:
#endif
	axi_jesd204_rx_remove(rx_jesd);
error_5:
	axi_jesd204_tx_remove(tx_jesd);
error_4:
#ifdef ORX_XCVR_BASEADDR
	adxcvr_remove(orx_adxcvr);
error_3:
#endif
	adxcvr_remove(rx_adxcvr);
error_2:
	adxcvr_remove(tx_adxcvr);
error_1:
	ad9528_remove(ad9528_device);

	/* If we are here and status non-zero, diplay status */
	if (status)
		goto error;

	printf("Bye \n");

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return 0;

error:
	printf("Error %d.\n", status);
	return status;
}

/***************************************************************************//**
 * @brief AGC settings.
 *
 * @return ret - 0 on success, negative value othervise.
*******************************************************************************/
int adrv9025_set_agc_params(adi_adrv9025_AgcCfg_t *agcConfig_init_param)
{
	/* Settings for PEAK DETECT MODE WITH FAST ATTACK */
	/* General AGC Configuration */
	agcConfig_init_param->rxChannelMask = 0x0F;
	agcConfig_init_param->agcPeakWaitTime = 4;
	agcConfig_init_param->agcRxMaxGainIndex = 255;
	agcConfig_init_param->agcRxMinGainIndex = 195;
	agcConfig_init_param->agcGainUpdateCounter = 921600;
	agcConfig_init_param->agcRxAttackDelay = 10;
	agcConfig_init_param->agcSlowLoopSettlingDelay = 16;
	agcConfig_init_param->agcLowThreshPreventGainInc = 1;
	agcConfig_init_param->agcChangeGainIfThreshHigh = 1;
	agcConfig_init_param->agcPeakThreshGainControlMode = 1;
	agcConfig_init_param->agcResetOnRxon = 0;
	agcConfig_init_param->agcEnableSyncPulseForGainCounter = 0;
	agcConfig_init_param->agcEnableFastRecoveryLoop = 0;

	/* adi_adrv9025_AgcPeak_t agcPeak */
	agcConfig_init_param->agcPeak.agcUnderRangeLowInterval = 836;
	agcConfig_init_param->agcPeak.agcUnderRangeMidInterval = 2;
	agcConfig_init_param->agcPeak.agcUnderRangeHighInterval = 4;
	agcConfig_init_param->agcPeak.apdHighThresh = 38;
	agcConfig_init_param->agcPeak.apdLowThresh = 25;
	agcConfig_init_param->agcPeak.apdUpperThreshPeakExceededCnt = 10;
	agcConfig_init_param->agcPeak.apdLowerThreshPeakExceededCnt = 3;
	agcConfig_init_param->agcPeak.enableHb2Overload = 1;
	agcConfig_init_param->agcPeak.hb2OverloadDurationCnt = 2;
	agcConfig_init_param->agcPeak.hb2OverloadThreshCnt = 1;
	agcConfig_init_param->agcPeak.hb2HighThresh = 11598;
	agcConfig_init_param->agcPeak.hb2UnderRangeLowThresh = 8211;
	agcConfig_init_param->agcPeak.hb2UnderRangeMidThresh = 5813;
	agcConfig_init_param->agcPeak.hb2UnderRangeHighThresh = 2913;
	agcConfig_init_param->agcPeak.hb2UpperThreshPeakExceededCnt = 10;
	agcConfig_init_param->agcPeak.hb2UnderRangeHighThreshExceededCnt = 3;
	agcConfig_init_param->agcPeak.hb2UnderRangeMidThreshExceededCnt = 3;
	agcConfig_init_param->agcPeak.hb2UnderRangeLowThreshExceededCnt = 3;
	agcConfig_init_param->agcPeak.hb2OverloadPowerMode = 0;
	agcConfig_init_param->agcPeak.hb2ThreshConfig = 3;

	agcConfig_init_param->agcPeak.apdGainStepAttack = 4;
	agcConfig_init_param->agcPeak.apdGainStepRecovery = 2;
	agcConfig_init_param->agcPeak.hb2GainStepAttack = 4;
	agcConfig_init_param->agcPeak.hb2GainStepHighRecovery = 2;
	agcConfig_init_param->agcPeak.hb2GainStepMidRecovery = 4;
	agcConfig_init_param->agcPeak.hb2GainStepLowRecovery = 8;

	/* adi_adrv9025_AgcPower_t agcPower */
	agcConfig_init_param->agcPower.powerEnableMeasurement = 0;
	agcConfig_init_param->agcPower.powerInputSelect = 0;
	agcConfig_init_param->agcPower.underRangeHighPowerThresh = 9;
	agcConfig_init_param->agcPower.underRangeLowPowerThresh = 2;
	agcConfig_init_param->agcPower.underRangeHighPowerGainStepRecovery = 0;
	agcConfig_init_param->agcPower.underRangeLowPowerGainStepRecovery = 0;
	agcConfig_init_param->agcPower.powerMeasurementDuration = 5;
	agcConfig_init_param->agcPower.rxTddPowerMeasDuration = 5;
	agcConfig_init_param->agcPower.rxTddPowerMeasDelay = 1;
	agcConfig_init_param->agcPower.overRangeHighPowerThresh = 2;
	agcConfig_init_param->agcPower.overRangeLowPowerThresh = 0;
	agcConfig_init_param->agcPower.powerLogShift = 1;
	agcConfig_init_param->agcPower.overRangeHighPowerGainStepAttack = 0;
	agcConfig_init_param->agcPower.overRangeLowPowerGainStepAttack = 0;

	return 0;
}
