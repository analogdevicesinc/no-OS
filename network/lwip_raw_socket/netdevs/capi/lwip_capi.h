/***************************************************************************//**
 *   @file   lwip_capi.h
 *   @brief  Generic lwIP glue for any CAPI MAC + CAPI PHY pair.
 *
 *   The glue is MAC-agnostic and PHY-agnostic.  It drives both
 *   subsystems entirely through their CAPI ops tables and uses
 *   application-supplied MDIO callbacks to bridge the PHY to the MAC.
 *   To plug a new MAC or PHY into lwIP, no glue change is required.
 *
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
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

#ifndef _LWIP_CAPI_H_
#define _LWIP_CAPI_H_

#ifdef NO_OS_LWIP_NETWORKING

#include <stdint.h>

#include "lwip_socket.h"
#include <capi_eth_mac.h>
#include <capi_eth_phy.h>

/**
 * @struct lwip_capi_param
 * @brief  Bundled MAC + PHY configuration for the generic CAPI lwIP glue.
 *
 * The application populates this and stores its address in
 * `lwip_network_param.mac_param`.  The glue does not interpret any of
 * the fields itself — it forwards them to the relevant CAPI op.
 *
 * The MAC handle must be initialised by the caller (via
 * `capi_eth_mac_init`) before `no_os_lwip_init` runs.  This lets the
 * caller wire a static `struct capi_eth_mac_handle *` slot and use
 * it from MDIO trampolines that match `capi_eth_phy_read_fn` (which
 * has no context pointer).
 */
struct lwip_capi_param {
	/** Pre-initialised MAC handle (owns DMA rings, MDIO bus). */
	struct capi_eth_mac_handle *mac;

	/* ---- PHY ---- */
	/** Ops table for the PHY driver. */
	const struct capi_eth_phy_ops *phy_ops;
	/** Optional PHY-specific extra config (capi_eth_phy_init_config.extra). */
	void *phy_extra;
	/** PHY MDIO address; 0 = auto-detect via fn_read. */
	uint8_t phy_addr;
	/** Caller-supplied MDIO transport (typically wraps a MAC helper). */
	capi_eth_phy_read_fn  fn_read;
	/** Caller-supplied MDIO transport. */
	capi_eth_phy_write_fn fn_write;
	/** PHY mode (speed/duplex/autoneg/mdix/loopback/isolate). */
	struct capi_eth_phy_mode_config phy_mode;
	/** Media interface (RGMII, RMII, SGMII, ...). */
	enum capi_eth_interface phy_interface;

	/** MAC hardware address — used for the multicast filter setup. */
	uint8_t hwaddr[6];
};

/**
 * @brief Generic lwIP no-OS ops table.
 *
 * Pass `&lwip_capi_ops` as `lwip_network_param.platform_ops` and set
 * `lwip_network_param.mac_param` to a `struct lwip_capi_param *`.
 */
extern const struct no_os_lwip_ops lwip_capi_ops;

#endif /* NO_OS_LWIP_NETWORKING */
#endif /* _LWIP_CAPI_H_ */
