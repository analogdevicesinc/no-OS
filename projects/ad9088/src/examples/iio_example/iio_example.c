/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the ad9088 project
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

/*
 * Brings the board up exactly as the DMA example does, then hands the UART to
 * an IIOD server instead of running a fixed measurement, so the device can be
 * driven from a libiio client.
 *
 * This first increment registers only the parts that already ship a no-OS IIO
 * wrapper, which for this board is the ADF4382 alone -- the HMC7044, the
 * JESD204 link cores and the transceivers have none. The AD9088's own
 * converters come next, as an iio_axi_adc/iio_axi_dac pair over the TPL cores
 * and DMACs initialised here; those are brought up already so that step is
 * purely additive.
 */

#include "iio_example.h"
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
#include "iio_adf4382.h"
#include "iio_app.h"

/* Side of the device the JESD204 links under test live on */
#define LINK_SIDE		0

/* Largest M the JESD204 link can report (ADI_APOLLO_CONV_PER_LINK_16) */
#define MAX_LINK_CONVERTERS	16

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. On success iio_app_run() does
 *		 not return, so a return at all means something failed.
 */
int iio_example_main()
{
	struct adf4382_iio_dev *adf4382_iio_dev;
	struct adf4382_iio_dev_init_param adf4382_iio_ip;
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
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};
	uint8_t num_conv;
	uint8_t tx_num_conv;
	uint8_t np;

	int ret = 0;

	pr_info("Enter IIO example\n");

	/*
	 * adf4382_iio_init() calls adf4382_init() itself, so this replaces the
	 * bare adf4382_init() the other examples use rather than joining it --
	 * doing both would configure the same SPI slave twice and leak a
	 * descriptor. The raw handle stays reachable as
	 * adf4382_iio_dev->adf4382_dev if it is ever needed. It stays first in
	 * the order either way, since the AD9088 device clock depends on it.
	 */
	adf4382_iio_ip.adf4382_dev_init = &adf4382_ip;

	ret = adf4382_iio_init(&adf4382_iio_dev, &adf4382_iio_ip);
	if (ret) {
		pr_info("ADF4382 IIO initialization failed\n");
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
	 * Derive the converter counts from the links the FSM just brought up
	 * rather than from the profile macros, so a firmware profile change
	 * cannot silently mis-size the TPL cores. The two directions carry
	 * independent M, and the TX one is what will size the IIO DAC's channel
	 * list, so they are kept apart.
	 */
	num_conv = ad9088_phy->profile.jtx[LINK_SIDE]
		   .tx_link_cfg[0].m_minus1 + 1;
	np = ad9088_phy->profile.jtx[LINK_SIDE].tx_link_cfg[0].np_minus1 + 1;
	tx_num_conv = ad9088_phy->profile.jrx[LINK_SIDE]
		      .rx_link_cfg[0].m_minus1 + 1;

	if (!num_conv || num_conv > MAX_LINK_CONVERTERS) {
		pr_err("Unexpected converter count M=%u\n", num_conv);
		ret = -EINVAL;
		goto error_topology;
	}

	if (!tx_num_conv || tx_num_conv > MAX_LINK_CONVERTERS) {
		pr_err("Unexpected transmit converter count M=%u\n",
		       tx_num_conv);
		ret = -EINVAL;
		goto error_topology;
	}

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = num_conv,
	};

	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.num_channels = tx_num_conv,
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

	/*
	 * Nothing drives the converters yet -- they are registered with IIO in
	 * the next increment. Leave the TX datapath quiet meanwhile rather than
	 * on the tone axi_dac_init()'s DDS setup programs.
	 */
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);

	pr_info("Link geometry: rx M=%u NP=%u, tx M=%u\n", num_conv, np,
		tx_num_conv);
	pr_info("Project configured\n\n");

	/*
	 * The ADF4382's channels are all scan_type = NULL, so this is a
	 * control-only device and takes neither a read nor a write buffer. The
	 * dev pointer is the IIO wrapper rather than the raw adf4382_dev, since
	 * that is what its attribute handlers cast.
	 *
	 * Note the frequency channel attribute is writable, and on this board
	 * the ADF4382 is the AD9088's device-clock source rather than an
	 * independent synthesiser -- writing it retunes the converter clock out
	 * from under a running JESD204 link. Reads are safe.
	 */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4382",
			.dev = adf4382_iio_dev,
			.dev_descriptor = adf4382_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	/*
	 * iio_app_init() creates and owns the UART descriptor itself, and
	 * removes and re-inits it around its banner, so the example must not
	 * have opened the same peripheral -- a second descriptor would leave
	 * stdout dangling and the RX FIFO unfed. main() already keeps its
	 * no_os_uart_stdio() call out of the IIO build for that reason.
	 */
	app_init_param.uart_init_params = platform_uart_ip;

	/* Let the bring-up log drain before IIOD takes the UART over. */
	no_os_mdelay(100);

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error_tx_dac;

	/*
	 * iio_app_run() loops forever, so everything below is reached only on
	 * failure, and nothing may print past this point.
	 */
	return iio_app_run(app);

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
	/* Frees the underlying adf4382_dev as well. */
	adf4382_iio_remove(adf4382_iio_dev);
error:
	if (ret)
		pr_info("Error!\n");

	return ret;
}
