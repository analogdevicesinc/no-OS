/***************************************************************************//**
 *   @file   app_transceiver.c
 *   @brief  FPGA XCVR setup and initialization routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
// stdlibs
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// platform drivers
#include "no_os_error.h"
#include "no_os_util.h"

// xcvr
#ifdef ALTERA_PLATFORM
#include "altera_adxcvr.h"
#else
#include "axi_adxcvr.h"
#endif

// hal
#include "parameters.h"
#include "adi_hal.h"

// header
#include "app_jesd.h"

static struct adxcvr *rx_adxcvr;
static struct adxcvr *tx_adxcvr;
static struct adxcvr *rx_os_adxcvr;

adiHalErr_t fpga_xcvr_init(uint32_t rx_lane_rate_khz,
			   uint32_t tx_lane_rate_khz,
			   uint32_t rx_os_lane_rate_khz,
			   uint32_t device_clock)
{
	int32_t status;


#ifdef ALTERA_PLATFORM
	struct adxcvr_init rx_adxcvr_init = {
		"rx_adxcvr",
		RX_XCVR_BASEADDR,
		{RX_ADXCFG_0_BASEADDR, RX_ADXCFG_1_BASEADDR, 0, 0},
		0,
		rx_lane_rate_khz,
		device_clock,
	};
	struct adxcvr_init tx_adxcvr_init = {
		"tx_adxcvr",
		TX_XCVR_BASEADDR,
		{TX_ADXCFG_0_BASEADDR, TX_ADXCFG_1_BASEADDR, TX_ADXCFG_2_BASEADDR, TX_ADXCFG_3_BASEADDR},
		TX_PLL_BASEADDR,
		tx_lane_rate_khz,
		device_clock,
	};
	struct adxcvr_init rx_os_adxcvr_init = {
		"rx_os_adxcvr",
		RX_OS_XCVR_BASEADDR,
		{RX_OS_ADXCFG_0_BASEADDR, RX_OS_ADXCFG_1_BASEADDR, 0, 0},
		0,
		rx_os_lane_rate_khz,
		device_clock,
	};
#else
	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 1,
		.lane_rate_khz = rx_lane_rate_khz,
		.ref_rate_khz = device_clock,
	};
	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = tx_lane_rate_khz,
		.ref_rate_khz = device_clock,
	};
	struct adxcvr_init rx_os_adxcvr_init = {
		.name = "rx_os_adxcvr",
		.base = RX_OS_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 1,
		.lane_rate_khz = rx_os_lane_rate_khz,
		.ref_rate_khz = device_clock,
	};
#endif

	/* Initialize ADXCR */
#ifndef ADRV9008_2
	status = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", rx_adxcvr_init.name);
		goto error_0;
	}
#endif
#ifndef ADRV9008_1
	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", tx_adxcvr_init.name);
		goto error_8;
	}
	status = adxcvr_init(&rx_os_adxcvr, &rx_os_adxcvr_init);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", rx_os_adxcvr_init.name);
		goto error_9;
	}
#endif
#ifndef ALTERA_PLATFORM
#ifndef ADRV9008_2
	status = adxcvr_clk_enable(rx_adxcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", rx_adxcvr->name);
		goto error_10;
	}
#endif
#ifndef ADRV9008_1
	status = adxcvr_clk_enable(tx_adxcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", tx_adxcvr->name);
		goto error_10;
	}
	status = adxcvr_clk_enable(rx_os_adxcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", rx_os_adxcvr->name);
		goto error_10;
	}
#endif
#endif

	return ADIHAL_OK;

#ifndef ALTERA_PLATFORM
error_10:
#ifndef ADRV9008_1
	adxcvr_remove(rx_os_adxcvr);
#endif
#endif
error_9:
#ifndef ADRV9008_1
	adxcvr_remove(tx_adxcvr);
#endif
error_8:
#ifndef ADRV9008_2
	adxcvr_remove(rx_adxcvr);
#endif
error_0:
	return ADIHAL_ERR;
}

void fpga_xcvr_deinit(void)
{
#ifndef ADRV9008_1
	adxcvr_remove(rx_os_adxcvr);
	adxcvr_remove(tx_adxcvr);
#endif
#ifndef ADRV9008_2
	adxcvr_remove(rx_adxcvr);
#endif
}
