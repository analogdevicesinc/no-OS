/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example eval-adf4382 project
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include "basic_example.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "ad9088.h"
#include "jesd204.h"
#include "axi_adxcvr.h"
#include "jesd204_clk.h"

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int basic_example_main()
{
	struct adf4382_dev *adf4382_dev;
	struct hmc7044_dev *hmc7044_dev;
	struct adf4030_dev *adf4030_dev;
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	/*
	 * The Agilex 5 bitstream does have an ADI adxcvr control core per
	 * direction (hdl library/intel/adi_jesd204 instantiates axi_adxcvr as
	 * "axi_xcvr", exported as the link_management window), but neither
	 * adxcvr driver in the tree fits it: axi_adxcvr.c is Xilinx-only and
	 * altera_adxcvr.c reprograms Arria10/Stratix10 ATX and CDR PLLs through
	 * per-lane PMA windows that GTS does not expose. The GTS PHY is fully
	 * configured by the bitstream, so all that is left is releasing the
	 * transceiver from reset, which axi_jesd204_{rx,tx}.c does directly from
	 * xcvr_base. These handles therefore exist only on the Xilinx path.
	 */
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
#endif
	struct jesd204_clk rx_jesd_clk = {0};
	struct jesd204_clk tx_jesd_clk = {0};
	struct no_os_clk_desc rx_lane_clk = {0};
	struct no_os_clk_desc tx_lane_clk = {0};
	struct axi_dmac *rx_dmac;
	struct axi_dmac *tx_dmac;
	struct ad9088_phy *ad9088_phy;

	int ret = 0;

	pr_info("Enter basic example\n");

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

	/* After the HMC7044: the ADF4030's reference comes from HMC7044 ch1. */
	ret = adf4030_init(&adf4030_dev, &adf4030_ip);
	if (ret) {
		pr_info("ADF4030 initialization failed (ret=%d)\n", ret);
		goto error_hmc7044;
	}

	/*
	 * Enables MCS calibration, which trims the AD9088's internal SYSREF onto
	 * the external edge. Needs both clock chips probed, so it goes here.
	 */
	ret = ad9088_mcs_ops_bind(adf4030_dev, adf4382_dev);
	if (ret) {
		pr_info("MCS ops bind failed\n");
		goto error_adf4030;
	}

	ret = axi_dmac_init(&rx_dmac, &rx_dmac_ip);
	if (ret) {
		pr_info("RX DMAC initialization failed\n");
		goto error_adf4030;
	}

	ret = axi_dmac_init(&tx_dmac, &tx_dmac_ip);
	if (ret) {
		pr_info("TX DMAC initialization failed\n");
		goto error_rx_dmac;
	}

#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	/*
	 * Xilinx path only: configure the ADI adxcvr transceivers. On Agilex the
	 * lane clocks keep xcvr == NULL - jesd204_clk.c speaks the Xilinx adxcvr
	 * API - and the transceiver is instead released from reset inside the
	 * link cores' CLOCKS_ENABLE handler.
	 */
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
#endif

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
	/* ADF4030 is the SYSREF provider and must precede the top device. */
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = adf4030_dev->jdev,
			.link_ids = {FRAMER_LINK_A0_RX,
				     DEFRAMER_LINK_A0_TX},
			.links_number = 2,
			.is_sysref_provider = true,
		},
		{
			.jdev = hmc7044_dev->jdev,
			.link_ids = {FRAMER_LINK_A0_RX,
				     DEFRAMER_LINK_A0_TX},
			.links_number = 2,
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
			.link_ids = {DEFRAMER_LINK_A0_TX,
				     DEFRAMER_LINK_B0_TX,
				     FRAMER_LINK_A0_RX,
				     FRAMER_LINK_B0_RX},
			.links_number = 4,
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

	pr_info("Project configured\n");

	/* NCO smoke test: tune RX CNCO0 side A, read it back. */
	{
		int64_t nco_set = 125000000; /* 125 MHz */
		int64_t nco_get = 0;

		ret = ad9088_set_cnco_freq(ad9088_phy, ADI_APOLLO_RX, 0, 0,
					   nco_set);
		if (ret) {
			pr_err("NCO test: set CNCO freq failed (%d)\n", ret);
		} else {
			ret = ad9088_get_cnco_freq(ad9088_phy, ADI_APOLLO_RX, 0,
						   0, &nco_get);
			if (ret)
				pr_err("NCO test: get CNCO freq failed (%d)\n",
				       ret);
			else
				pr_info("NCO test: RX CNCO0-A set %lld Hz, read %lld Hz\n",
					nco_set, nco_get);
		}
	}

	/* NCO smoke test: tune RX FNCO0 side A, read it back. */
	{
		int64_t nco_set = 40200000; /* 40 MHz */
		int64_t nco_get = 0;

		ret = ad9088_set_fnco_freq(ad9088_phy, ADI_APOLLO_RX, 0, 0,
					   nco_set);
		if (ret) {
			pr_err("NCO test: set FNCO freq failed (%d)\n", ret);
		} else {
			ret = ad9088_get_fnco_freq(ad9088_phy, ADI_APOLLO_RX, 0,
						   0, &nco_get);
			if (ret)
				pr_err("NCO test: get FNCO freq failed (%d)\n",
				       ret);
			else
				pr_info("NCO test: RX FNCO0-A set %lld Hz, read %lld Hz\n",
					nco_set, nco_get);
		}
	}

	/* NCO smoke test: tune TX CNCO0 side A, read it back. */
	{
		int64_t nco_set = 125000000; /* 125 MHz */
		int64_t nco_get = 0;

		ret = ad9088_set_cnco_freq(ad9088_phy, ADI_APOLLO_TX, 0, 0,
					   nco_set);
		if (ret) {
			pr_err("NCO test: set TX CNCO freq failed (%d)\n", ret);
		} else {
			ret = ad9088_get_cnco_freq(ad9088_phy, ADI_APOLLO_TX, 0,
						   0, &nco_get);
			if (ret)
				pr_err("NCO test: get TX CNCO freq failed (%d)\n",
				       ret);
			else
				pr_info("NCO test: TX CNCO0-A set %lld Hz, read %lld Hz\n",
					nco_set, nco_get);
		}
	}

	/* NCO smoke test: tune TX FNCO0 side A, read it back. */
	{
		int64_t nco_set = 40200000; /* 40 MHz */
		int64_t nco_get = 0;

		ret = ad9088_set_fnco_freq(ad9088_phy, ADI_APOLLO_TX, 0, 0,
					   nco_set);
		if (ret) {
			pr_err("NCO test: set TX FNCO freq failed (%d)\n", ret);
		} else {
			ret = ad9088_get_fnco_freq(ad9088_phy, ADI_APOLLO_TX, 0,
						   0, &nco_get);
			if (ret)
				pr_err("NCO test: get TX FNCO freq failed (%d)\n",
				       ret);
			else
				pr_info("NCO test: TX FNCO0-A set %lld Hz, read %lld Hz\n",
					nco_set, nco_get);
		}
	}

	return 0;

error_topology:
	jesd204_topology_remove(topology);
error_ad9088:
	ad9088_remove(ad9088_phy);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_rx_adxcvr:
#ifndef CONFIG_ALTERA_PLATFORM_NIOSV
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_tx_dmac:
#endif
	axi_dmac_remove(tx_dmac);
error_rx_dmac:
	axi_dmac_remove(rx_dmac);
error_adf4030:
	adf4030_remove(adf4030_dev);
error_hmc7044:
	hmc7044_remove(hmc7044_dev);
error_adf4382:
	adf4382_remove(adf4382_dev);
error:
	pr_info("Error!\n");

	return ret;
}
