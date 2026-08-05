/***************************************************************************//**
 *   @file   clkgen_routines.c
 *   @brief  AXI clkgen setup for ADRV903X project.
 *   @author Analog Devices Inc.
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
#include "clkgen_routines.h"
#include "app_config.h"
#include "parameters.h"

/**
 * @brief Initialize and set rates for RX and TX AXI clkgen instances.
 *
 * Both clkgens share the same parent clock (DEVICE_CLK) and output rate
 * (lane_rate / 66 for JESD204C).
 *
 * @param rx_clkgen - Output pointer for the RX clkgen instance.
 * @param tx_clkgen - Output pointer for the TX clkgen instance.
 * @return 0 on success, negative error code on failure.
 */
int clkgen_setup(struct axi_clkgen **rx_clkgen, struct axi_clkgen **tx_clkgen)
{
	/* JESD204C: divide lane rate by 66 to get the clkgen output rate */
	uint32_t div_rate_hz = ADRV903X_LANE_RATE_KHZ / 66 * 1000;
	int ret;

	struct axi_clkgen_init rx_clkgen_init = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = ADRV903X_DEVICE_CLK_KHZ * 1000
	};

	struct axi_clkgen_init tx_clkgen_init = {
		.name = "tx_clkgen",
		.base = TX_CLKGEN_BASEADDR,
		.parent_rate = ADRV903X_DEVICE_CLK_KHZ * 1000
	};

	ret = axi_clkgen_init(rx_clkgen, &rx_clkgen_init);
	if (ret) {
		pr_err("clkgen: %s init failed: %d\n", rx_clkgen_init.name, ret);
		return ret;
	}

	ret = axi_clkgen_init(tx_clkgen, &tx_clkgen_init);
	if (ret) {
		pr_err("clkgen: %s init failed: %d\n", tx_clkgen_init.name, ret);
		goto error_rx;
	}

	ret = axi_clkgen_set_rate(*rx_clkgen, div_rate_hz);
	if (ret) {
		pr_err("clkgen: %s set_rate failed: %d\n", (*rx_clkgen)->name,
		       ret);
		goto error_tx;
	}

	ret = axi_clkgen_set_rate(*tx_clkgen, div_rate_hz);
	if (ret) {
		pr_err("clkgen: %s set_rate failed: %d\n", (*tx_clkgen)->name,
		       ret);
		goto error_tx;
	}

	return 0;

error_tx:
	axi_clkgen_remove(*tx_clkgen);
error_rx:
	axi_clkgen_remove(*rx_clkgen);
	return ret;
}

/**
 * @brief Remove AXI clkgen instances.
 *
 * @param rx_clkgen - RX clkgen instance.
 * @param tx_clkgen - TX clkgen instance.
 * @return 0
 */
int clkgen_remove(struct axi_clkgen *rx_clkgen, struct axi_clkgen *tx_clkgen)
{
	if (tx_clkgen)
		axi_clkgen_remove(tx_clkgen);
	if (rx_clkgen)
		axi_clkgen_remove(rx_clkgen);
	return 0;
}
