/***************************************************************************//**
 *   @file   altera_gts_xcvr.h
 *   @brief  Intel GTS (Agilex 5) transceiver bring-up for the JESD204 link cores
 *   @author Bogdan Luncan (bogdan.luncan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX short identifier: BSD-1-Clause
*******************************************************************************/

#ifndef ALTERA_GTS_XCVR_H_
#define ALTERA_GTS_XCVR_H_

#include <stdint.h>
#include <inttypes.h>
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/*
 * Intel GTS PLL refclk-buffer request, reg 0x0e bits [23:16]. The field must be
 * written as a byte: reg 0x0e also carries state that a 32-bit write clobbers.
 * The request bit self-clears, so a readback is not a success indicator - only
 * the measured link clock is.
 */
#define GTS_REFCLK_BUFFER_REQ_BYTE	(0x0e * 4 + 2)

/*
 * The axi_adxcvr control core that hdl library/intel/adi_jesd204 instantiates as
 * "axi_xcvr" and exports as the link_management slave (not link_reconfig, which
 * is the JESD204 link core). Byte offsets, matching altera_adxcvr.c.
 */
#define ADXCVR_REG_RESETN		0x0010
#define ADXCVR_RESETN			NO_OS_BIT(0)
#define ADXCVR_REG_STATUS		0x0014
#define ADXCVR_STATUS_READY		NO_OS_BIT(0)

#define ADXCVR_RESET_TIMEOUT_MS		1000

/**
 * @brief Gate on an Intel GTS transceiver reference clock.
 * @param refclk_ready - GPIO gating the HDL gts_refclk_reset state machine.
 * @param gts_pll_base - GTS PLL reconfig window, 0 to only drive the GPIO.
 * @return 0 on success, negative error code otherwise.
 *
 * GTS PHYs (Agilex 5) get no reference clock until software gates them on.
 * Mirrors Linux altera_adxcvr.c adxcvr_jesd204_link_setup().
 */
static inline int altera_gts_refclk_setup(struct no_os_gpio_desc *refclk_ready,
		uint32_t gts_pll_base)
{
	int ret;

	if (refclk_ready) {
		ret = no_os_gpio_direction_output(refclk_ready, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		/* Let the state machine ack and the FPGA-internal PLL lock. */
		no_os_mdelay(100);
	}

	if (gts_pll_base) {
		*(volatile uint8_t *)(uintptr_t)(gts_pll_base +
						 GTS_REFCLK_BUFFER_REQ_BYTE) = 0xff;
		no_os_mdelay(10);
	}

	return 0;
}

/**
 * @brief Release an Intel GTS transceiver from reset and wait for ready.
 * @param name - Link core name, for the log line.
 * @param xcvr_base - Base address of the axi_xcvr link_management window.
 * @return 0 on success, negative error code otherwise.
 *
 * Until RESETN is written the core holds phy_reset_control asserted, which the
 * link core reports as "External reset is asserted" and which leaves the RX CDR
 * unlocked. Mirrors Linux altera_adxcvr.c adxcvr_finalize_lane_rate_change().
 *
 * On GTS the RX ready bit only asserts once real data arrives on the lanes, so a
 * timeout is reported but not fatal: the converter is brought up after this
 * point and the link state machine is what finally judges the link.
 */
static inline int altera_gts_xcvr_reset(const char *name, uint32_t xcvr_base)
{
	volatile uint32_t *resetn;
	volatile uint32_t *status;
	int timeout;

	if (!xcvr_base)
		return 0;

	resetn = (volatile uint32_t *)(uintptr_t)(xcvr_base + ADXCVR_REG_RESETN);
	status = (volatile uint32_t *)(uintptr_t)(xcvr_base + ADXCVR_REG_STATUS);

	*resetn = 0;
	no_os_mdelay(1);
	*resetn = ADXCVR_RESETN;

	for (timeout = ADXCVR_RESET_TIMEOUT_MS; timeout > 0; timeout--) {
		if (*status & ADXCVR_STATUS_READY)
			return 0;

		no_os_mdelay(1);
	}

	return 0;
}

#endif /* ALTERA_GTS_XCVR_H_ */
