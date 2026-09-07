/***************************************************************************//**
 *   @file   adrv902x/src/common/clkgen_routines.c
 *   @brief  Implements clkgen init and settings to be used by adrv902x examples.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "clkgen_routines.h"

/**
 * @brief Init and set rates for axi_clkgen components
 * @param **rx_clkgen - axi_clkgen for Rx path
 * @param **tx_clkgen - axi_clkgen for Tx path
 * @param **orx_clkgen - axi_clkgen for ORx path
 * @param jesd204c_en - 1 if JESD204C
 * @return 0 in case of success, negative value otherwise
 */
int clkgen_setup(struct axi_clkgen **rx_clkgen, struct axi_clkgen **tx_clkgen,
		 struct axi_clkgen **orx_clkgen, uint8_t jesd204c_en)
{
#ifdef ORX_CLKGEN_BASEADDR
	uint32_t orx_div_rate_hz;
#endif
	uint32_t tx_div_rate_hz;
	uint32_t rx_div_rate_hz;
	uint32_t divisor = 40;
	int status;

	if (jesd204c_en)
		divisor = 66;

	struct axi_clkgen_init rx_clkgen_init = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = ADRV9025_DEVICE_CLK_KHZ * 1000
	};
	rx_div_rate_hz = ADRV9025_LANE_RATE_KHZ / divisor * 1000;

	struct axi_clkgen_init tx_clkgen_init = {
		.name = "tx_clkgen",
		.base = TX_CLKGEN_BASEADDR,
		.parent_rate = ADRV9025_DEVICE_CLK_KHZ * 1000
	};
	tx_div_rate_hz = ADRV9025_LANE_RATE_KHZ / divisor * 1000;

#ifdef ORX_CLKGEN_BASEADDR
	struct axi_clkgen_init orx_clkgen_init = {
		.name = "orx_clkgen",
		.base = ORX_CLKGEN_BASEADDR,
		.parent_rate = ADRV9025_DEVICE_CLK_KHZ * 1000
	};
	orx_div_rate_hz = ADRV9025_LANE_RATE_KHZ / divisor * 1000;
#endif

	status = axi_clkgen_init(rx_clkgen, &rx_clkgen_init);
	if (status) {
		pr_err("error: %s initialization error: %d\n", (*rx_clkgen)->name, status);
		goto error_0;
	}

	status = axi_clkgen_init(tx_clkgen, &tx_clkgen_init);
	if (status) {
		pr_err("error: %s initialization error: %d\n", (*tx_clkgen)->name, status);
		goto error_1;
	}

#ifdef ORX_CLKGEN_BASEADDR
	status = axi_clkgen_init(orx_clkgen, &orx_clkgen_init);
	if (status) {
		pr_err("error: %s initialization error: %d\n", (*orx_clkgen)->name, status);
		goto error_2;
	}
#endif

	status = axi_clkgen_set_rate(*rx_clkgen, rx_div_rate_hz);
	if (status != 0) {
		pr_err("error: %s: set rate failed\n", (*rx_clkgen)->name);
		goto error_3;
	}

	status = axi_clkgen_set_rate(*tx_clkgen, tx_div_rate_hz);
	if (status != 0) {
		pr_err("error: %s: set rate failed\n", (*tx_clkgen)->name);
		goto error_3;
	}

#ifdef ORX_CLKGEN_BASEADDR
	status = axi_clkgen_set_rate(*orx_clkgen, orx_div_rate_hz);
	if (status != 0) {
		pr_err("error: %s: set rate failed\n", (*orx_clkgen)->name);
		goto error_3;
	}
#endif

	return 0;
error_3:
#ifdef ORX_CLKGEN_BASEADDR
	axi_clkgen_remove(*orx_clkgen);
error_2:
#endif
	axi_clkgen_remove(*tx_clkgen);
error_1:
	axi_clkgen_remove(*rx_clkgen);
error_0:
	return status;
}

/**
 * @brief remove axi_clkgen structs.
 * @param rx_clkgen - axi_clkgen for Rx path
 * @param tx_clkgen - axi_clkgen for Tx path
 * @param orx_clkgen - axi_clkgen for ORx path
 * @return 0
 */
int clkgen_remove(struct axi_clkgen *rx_clkgen, struct axi_clkgen *tx_clkgen,
		  struct axi_clkgen *orx_clkgen)
{
	if (orx_clkgen)
		axi_clkgen_remove(orx_clkgen);

	if (tx_clkgen)
		axi_clkgen_remove(tx_clkgen);

	if (rx_clkgen)
		axi_clkgen_remove(rx_clkgen);

	return 0;
}
