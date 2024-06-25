/***************************************************************************//**
 *   @file   app_transceiver.c
 *   @brief  FPGA XCVR setup and initialization routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
