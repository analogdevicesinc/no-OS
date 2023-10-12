/***************************************************************************//**
 *   @file   app_jesd.c
 *   @brief  JESD setup and initialization routines.
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

// jesd
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"

// hal
#include "parameters.h"
#include "adi_hal.h"

// header
#include "app_jesd.h"

struct axi_jesd204_rx *rx_jesd = NULL;
struct axi_jesd204_tx *tx_jesd = NULL;
struct axi_jesd204_rx *rx_os_jesd = NULL;

adiHalErr_t jesd_init(uint32_t rx_div40_rate_hz,
		      uint32_t tx_div40_rate_hz,
		      uint32_t rx_os_div40_rate_hz)
{
	int32_t status;
	uint32_t rx_lane_rate_khz = rx_div40_rate_hz / 1000 * 40;
	struct jesd204_rx_init rx_jesd_init = {
		"rx_jesd",
		RX_JESD_BASEADDR,
		4,
		32,
		1,
		rx_div40_rate_hz / 1000,
		rx_lane_rate_khz
	};
	uint32_t tx_lane_rate_khz = tx_div40_rate_hz / 1000 * 40;
	struct jesd204_tx_init tx_jesd_init = {
		"tx_jesd",
		TX_JESD_BASEADDR,
		2,
		32,
		4,
		14,
		16,
		true,
		2,
		1,
		tx_div40_rate_hz / 1000,
		tx_lane_rate_khz
	};
	uint32_t rx_os_lane_rate_khz = rx_os_div40_rate_hz / 1000 * 40;
	struct jesd204_rx_init rx_os_jesd_init = {
		"rx_os_jesd",
		RX_OS_JESD_BASEADDR,
		2,
		32,
		1,
		rx_os_div40_rate_hz / 1000,
		rx_os_lane_rate_khz,
	};

	/* Initialize JESD */
#ifndef ADRV9008_2
	status = axi_jesd204_rx_init_legacy(&rx_jesd, &rx_jesd_init);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n", rx_jesd_init.name);
		goto error_5;
	}
#endif
#ifndef ADRV9008_1
	status = axi_jesd204_tx_init_legacy(&tx_jesd, &tx_jesd_init);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_init_legacy() failed\n", tx_jesd_init.name);
		goto error_6;
	}
	status = axi_jesd204_rx_init_legacy(&rx_os_jesd, &rx_os_jesd_init);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n",
		       rx_os_jesd_init.name);
		goto error_7;
	}

#endif
	return ADIHAL_OK;

error_7:
#ifndef ADRV9008_1
	axi_jesd204_rx_remove(rx_os_jesd);
#endif
error_6:
#ifndef ADRV9008_1
	axi_jesd204_tx_remove(tx_jesd);
#endif
error_5:
#ifndef ADRV9008_2
	axi_jesd204_rx_remove(rx_jesd);
#endif

	return ADIHAL_ERR;
}

void jesd_deinit(void)
{
#ifndef	ADRV9008_1
	axi_jesd204_rx_remove(rx_os_jesd);
	axi_jesd204_tx_remove(tx_jesd);
#endif
#ifndef ADRV9008_2
	axi_jesd204_rx_remove(rx_jesd);
#endif
}

void jesd_status(void)
{
#ifndef ADRV9008_2
	axi_jesd204_rx_status_read(rx_jesd);
#endif
#ifndef ADRV9008_1
	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_os_jesd);
#endif
}

void jesd_rx_watchdog(void)
{
#ifndef ADRV9008_2
	axi_jesd204_rx_watchdog(rx_jesd);
#endif
#ifndef ADRV9008_1
	axi_jesd204_rx_watchdog(rx_os_jesd);
#endif
}

