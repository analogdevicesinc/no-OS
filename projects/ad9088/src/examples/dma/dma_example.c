/***************************************************************************//**
 *   @file   dma_example.c
 *   @brief  DMA capture example for the ad9088 project
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "dma_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "ad9088.h"
#include "jesd204.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "jesd204_clk.h"
#include "parameters.h"
#include "xil_cache.h"

/* Side and datapath the test tone is injected on */
#define TEST_TONE_SIDE		0
#define TEST_TONE_FDDC		0
/* Mid-scale test tone amplitude, within the RX FNCO 0x1FFF limit */
#define TEST_TONE_OFFSET	0x1000

/* Number of I/Q pairs printed per converter when dumping a capture */
#define DUMP_SAMPLES		16

/* Largest M the JESD204 link can report (ADI_APOLLO_CONV_PER_LINK_16) */
#define MAX_LINK_CONVERTERS	16

#define DMA_BUFFER_ALIGN	1024

/*
 * A driven capture must beat the idle noise floor by this factor. The tone path
 * measured 55x on HW, so this is a wide margin against a false pass.
 */
#define DRIVEN_PP_RATIO		10

/*
 * Fixed-capacity static capture buffer. Samples-per-converter is derived from
 * the link's M at runtime and clamped to what fits here, so a profile with more
 * converters shortens the capture instead of overrunning the buffer.
 */
static uint16_t adc_buffer_dma[ADC_BUFFER_SAMPLES * 8]
__attribute__((aligned(DMA_BUFFER_ALIGN)));

static uint32_t dac_buffer_dma[DAC_BUFFER_SAMPLES]
__attribute__((aligned(DMA_BUFFER_ALIGN)));


int dma_example_main()
{
	struct adf4382_dev *adf4382_dev;
	struct hmc7044_dev *hmc7044_dev;
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
	struct jesd204_clk rx_jesd_clk = {0};
	struct jesd204_clk tx_jesd_clk = {0};
	struct no_os_clk_desc rx_lane_clk = {0};
	struct no_os_clk_desc tx_lane_clk = {0};
	struct axi_dmac *rx_dmac;
	struct axi_dmac *tx_dmac;
	struct ad9088_phy *ad9088_phy;
	struct axi_adc *rx_adc;
	struct axi_dac *tx_dac;
	uint32_t samples_per_conv;
	uint32_t transfer_size;
	uint8_t num_conv;
	uint8_t np;
	uint32_t tone_pp;
	uint32_t idle_pp;
	uint32_t loop_pp = 0;
	bool tx_running = false;

	int ret = 0;

	pr_info("Enter DMA example\n");

	ret = adf4382_init(&adf4382_dev, &adf4382_ip);
	if (ret) {
		pr_info("ADF4382 initialization failed\n");
		goto error;
	}

	ret = hmc7044_init(&hmc7044_dev, &hmc7044_ip);
	if (ret) {
		pr_info("HMC7044 initialization failed\n");
		goto error_adf4382;
	}

	ret = axi_dmac_init(&rx_dmac, &rx_dmac_ip);
	if (ret) {
		pr_info("RX DMAC initialization failed\n");
		goto error_hmc7044;
	}

	ret = axi_dmac_init(&tx_dmac, &tx_dmac_ip);
	if (ret) {
		pr_info("TX DMAC initialization failed\n");
		goto error_rx_dmac;
	}

	ret = adxcvr_init(&tx_adxcvr, &tx_adxcvr_ip);
	if (ret) {
		pr_info("TX ADXCVR initialization failed\n");
		goto error_tx_dmac;
	}
	tx_jesd_clk.xcvr = tx_adxcvr;

	ret = adxcvr_init(&rx_adxcvr, &rx_adxcvr_ip);
	if (ret) {
		pr_info("RX ADXCVR initialization failed\n");
		goto error_tx_adxcvr;
	}
	rx_jesd_clk.xcvr = rx_adxcvr;

	rx_lane_clk.platform_ops = &jesd204_clk_ops;
	rx_lane_clk.dev_desc = &rx_jesd_clk;
	rx_jesd204_ip.lane_clk = &rx_lane_clk;

	tx_lane_clk.platform_ops = &jesd204_clk_ops;
	tx_lane_clk.dev_desc = &tx_jesd_clk;
	tx_jesd204_ip.lane_clk = &tx_lane_clk;

	ret = axi_jesd204_rx_init(&rx_jesd, &rx_jesd204_ip);
	if (ret) {
		pr_info("JESD RX initialization failed\n");
		goto error_rx_adxcvr;
	}
	rx_jesd_clk.jesd_rx = rx_jesd;

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd204_ip);
	if (ret) {
		pr_info("JESD TX initialization failed\n");
		goto error_rx_jesd;
	}
	tx_jesd_clk.jesd_tx = tx_jesd;

	ret = ad9088_init(&ad9088_phy, &ad9088_ip);
	if (ret) {
		pr_info("AD9088 initialization failed\n");
		goto error_tx_jesd;
	}

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = hmc7044_dev->jdev,
			.link_ids = {
				FRAMER_LINK_A0_RX,
				DEFRAMER_LINK_A0_TX
			},
			.links_number = 2,
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER_LINK_A0_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER_LINK_A0_TX},
			.links_number = 1,
		},
		{
			.jdev = ad9088_phy->jdev,
			.link_ids = {
				FRAMER_LINK_A0_RX,
				DEFRAMER_LINK_A0_TX
			},
			.links_number = 2,
			.is_top_device = true,
		},
	};

	ret = jesd204_topology_init(&topology, devs,
				    NO_OS_ARRAY_SIZE(devs));
	if (ret) {
		pr_info("JESD204 topology init failed\n");
		goto error_ad9088;
	}

	ret = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (ret) {
		pr_info("JESD204 FSM start failed\n");
		goto error_topology;
	}

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	/*
	 * Derive the capture geometry from the link the FSM just brought up
	 * rather than hardcoding it, so a profile change cannot silently
	 * corrupt the buffer layout.
	 */
	num_conv = ad9088_phy->profile.jtx[TEST_TONE_SIDE].tx_link_cfg[0].m_minus1 + 1;
	np = ad9088_phy->profile.jtx[TEST_TONE_SIDE].tx_link_cfg[0].np_minus1 + 1;

	if (!num_conv || num_conv > MAX_LINK_CONVERTERS) {
		pr_err("Unexpected converter count M=%u\n", num_conv);
		ret = -EINVAL;
		goto error_topology;
	}

	/* Clamp the capture depth to what the static buffer can hold. */
	samples_per_conv = ADC_BUFFER_SAMPLES;
	if (samples_per_conv * num_conv > NO_OS_ARRAY_SIZE(adc_buffer_dma))
		samples_per_conv = NO_OS_ARRAY_SIZE(adc_buffer_dma) / num_conv;

	transfer_size = samples_per_conv * num_conv * sizeof(adc_buffer_dma[0]);

	pr_info("Capture geometry: M=%u NP=%u samples/conv=%lu bytes=%lu\n",
		num_conv, np, (unsigned long)samples_per_conv,
		(unsigned long)transfer_size);

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = num_conv,
	};

	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.num_channels = num_conv,
	};

	ret = axi_adc_init(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("RX TPL core init failed (%d)\n", ret);
		goto error_topology;
	}

	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("TX TPL core init failed (%d)\n", ret);
		goto error_rx_adc;
	}

	/* Leave the TX datapath idle so captures 1 and 2 see nothing from it. */
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);
	pr_info("Project configured\n");

	axi_dac_remove(tx_dac);
	axi_adc_remove(rx_adc);

	return 0;

error_tx_on:
	dma_example_tx_stop(tx_dac, tx_dmac);
	goto error_tx_dac;
error_tone:
	ad9088_set_fnco_test_tone(ad9088_phy, ADI_APOLLO_RX, TEST_TONE_SIDE,
				  TEST_TONE_FDDC, false, 0);
error_tx_dac:
	axi_dac_remove(tx_dac);
error_rx_adc:
	axi_adc_remove(rx_adc);
error_topology:
	jesd204_topology_remove(topology);
error_ad9088:
	ad9088_remove(ad9088_phy);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_rx_adxcvr:
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_tx_dmac:
	axi_dmac_remove(tx_dmac);
error_rx_dmac:
	axi_dmac_remove(rx_dmac);
error_hmc7044:
	hmc7044_remove(hmc7044_dev);
error_adf4382:
	adf4382_remove(adf4382_dev);
error:
	pr_info("Error!\n");

	return ret;
}
