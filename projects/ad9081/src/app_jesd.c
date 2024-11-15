/***************************************************************************//**
 *   @file   app_jesd.c
 *   @brief  Application JESD setup.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adxcvr.h"
#include "no_os_error.h"
#include "parameters.h"
#include "app_jesd.h"
#include "app_config.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct axi_jesd204_rx *rx_jesd;
struct axi_jesd204_tx *tx_jesd;

struct adxcvr *rx_adxcvr;
struct adxcvr *tx_adxcvr;

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Application JESD setup.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t app_jesd_init(struct no_os_clk clk[2],
		      uint32_t reference_clk_khz,
		      uint32_t rx_device_clk_khz,
		      uint32_t tx_device_clk_khz,
		      uint32_t rx_lane_clk_khz,
		      uint32_t tx_lane_clk_khz)
{
	int32_t ret;

	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = AD9081_TX_JESD_F,
		.frames_per_multiframe = AD9081_TX_JESD_K,
		.converters_per_device = AD9081_TX_JESD_M *
		MULTIDEVICE_INSTANCE_COUNT,
		.converter_resolution = AD9081_TX_JESD_N,
		.bits_per_sample = AD9081_TX_JESD_NP,
		.high_density = AD9081_TX_JESD_HD,
		.control_bits_per_sample = AD9081_TX_JESD_CS,
		.subclass = AD9081_TX_JESD_SUBCLASS,
		.device_clk_khz = tx_device_clk_khz,
		.lane_clk_khz = tx_lane_clk_khz
	};

	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = AD9081_RX_JESD_F,
		.frames_per_multiframe = AD9081_RX_JESD_K,
		.subclass = AD9081_RX_JESD_SUBCLASS,
		.device_clk_khz = rx_device_clk_khz,
		.lane_clk_khz = rx_lane_clk_khz
	};

#ifdef TX_XCVR_BASEADDR
	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 0,
		.lane_rate_khz = tx_lane_clk_khz,
		.ref_rate_khz = reference_clk_khz,
		.export_no_os_clk = true
	};
#endif

#ifdef RX_XCVR_BASEADDR
	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.lane_rate_khz = rx_lane_clk_khz,
		.ref_rate_khz = reference_clk_khz,
		.export_no_os_clk = true
	};
#endif

#ifdef TX_XCVR_BASEADDR
	ret = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (ret)
		return ret;
#endif
#ifdef RX_XCVR_BASEADDR
	ret = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (ret)
		return ret;
#endif

	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;

	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (ret)
		return ret;

	clk[1].clk_desc = tx_jesd->lane_clk;

	ret = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (ret)
		return ret;

	clk[0].clk_desc = rx_jesd->lane_clk;

	return 0;
}
