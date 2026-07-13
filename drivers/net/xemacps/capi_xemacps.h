/***************************************************************************//**
 *   @file   capi_xemacps.h
 *   @brief  CAPI MAC driver for the Xilinx PS GEM (XEmacPs).
 *
 *   Implements the capi_eth_mac.h contract for Zynq-7000 / ZynqMP PS
 *   Gigabit Ethernet MAC.  This driver knows nothing about PHYs — the
 *   application layer (or lwIP glue) owns the PHY handle and stitches
 *   the two together via the MDIO helpers exposed below.
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

#ifndef _CAPI_XEMACPS_H_
#define _CAPI_XEMACPS_H_

#include <stdint.h>
#include <stdbool.h>

#include <xparameters.h>
#include <xparameters_ps.h>
#include "no_os_util.h"
#include <capi_eth_mac.h>
#include <capi_mdio.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* Number of RX buffer descriptors pre-posted to hardware */
#define XGEM_RX_BD_COUNT		64
#define XGEM_TX_BD_COUNT		1

#if defined(__aarch64__)
#define XGEM_CACHE_LINE_SIZE	64
#else
#define XGEM_CACHE_LINE_SIZE	32
#endif

/* Maximum Ethernet frame size, rounded up to a 32-byte cache-line multiple. */
#ifndef XEMACPS_MAX_FRAME_SIZE
#define XEMACPS_MAX_FRAME_SIZE	1536
#endif

/* Frame buffer size, aligned to the next cache-line size. */
#define XGEM_BUFF_SIZE	no_os_align(XEMACPS_MAX_FRAME_SIZE, XGEM_CACHE_LINE_SIZE)

/* GEM feature flags. Override at build time, e.g. -DGEM_PROMISC */

#ifdef GEM_PROMISC
#define GEM_NWCFG_PROMISC	XEMACPS_NWCFG_COPYALLEN_MASK
#else
#define GEM_NWCFG_PROMISC	0
#endif

#ifdef GEM_NO_BROADCAST
#define GEM_NWCFG_BCAST		XEMACPS_NWCFG_BCASTDI_MASK
#else
#define GEM_NWCFG_BCAST		0
#endif

#ifdef GEM_STRIP_FCS
#define GEM_NWCFG_FCS		XEMACPS_NWCFG_FCSREM_MASK
#else
#define GEM_NWCFG_FCS		0
#endif

#ifndef GEM_NO_RX_CHKSUM
#define GEM_NWCFG_RXCHKSUM	XEMACPS_NWCFG_RXCHKSUMEN_MASK
#else
#define GEM_NWCFG_RXCHKSUM	0
#endif

#ifndef GEM_NO_PAUSE
#define GEM_NWCFG_PAUSE		XEMACPS_NWCFG_PAUSEEN_MASK
#else
#define GEM_NWCFG_PAUSE		0
#endif

#ifndef GEM_NO_MCAST_HASH
#define GEM_NWCFG_MCAST		XEMACPS_NWCFG_MCASTHASHEN_MASK
#else
#define GEM_NWCFG_MCAST		0
#endif

#ifdef GEM_UCAST_HASH
#define GEM_NWCFG_UCAST		XEMACPS_NWCFG_UCASTHASHEN_MASK
#else
#define GEM_NWCFG_UCAST		0
#endif

#ifdef GEM_SGMII
#define GEM_NWCFG_SGMII		(XEMACPS_NWCFG_SGMIIEN_MASK | \
				 XEMACPS_NWCFG_PCSSEL_MASK)
#else
#define GEM_NWCFG_SGMII		0
#endif

#define GEM_NWCFG_FEATURES	(GEM_NWCFG_PROMISC | \
				 GEM_NWCFG_BCAST | GEM_NWCFG_FCS | \
				 GEM_NWCFG_RXCHKSUM | GEM_NWCFG_PAUSE | \
				 GEM_NWCFG_MCAST | GEM_NWCFG_UCAST | \
				 GEM_NWCFG_SGMII)

/**
 * @brief XEmacPs CAPI MAC ops table.
 *
 * Pass `&xemacps_capi_mac_ops` as `capi_eth_mac_init_config.ops`.
 */
extern const struct capi_eth_mac_ops xemacps_capi_mac_ops;

/**
 * @brief MDIO clause-22 read on the GEM's MDIO bus.
 *
 * The GEM owns the MDIO peripheral.  This helper is called by
 * xemacps_capi_mdio_ops (or directly, for legacy code) to reach PHYs
 * attached to the bus.
 *
 * @param handle    MAC handle returned by capi_eth_mac_init().
 * @param phy_addr  MDIO PHY address (0..31).
 * @param reg_addr  Register address.
 * @param data      Output: register value.
 * @return 0 on success, negative errno on failure.
 */
int xemacps_mdio_read(struct capi_eth_mac_handle *handle,
		      uint8_t phy_addr, uint8_t reg_addr, uint16_t *data);

/**
 * @brief MDIO clause-22 write on the GEM's MDIO bus.
 *
 * @param handle    MAC handle returned by capi_eth_mac_init().
 * @param phy_addr  MDIO PHY address (0..31).
 * @param reg_addr  Register address.
 * @param data      Value to write.
 * @return 0 on success, negative errno on failure.
 */
int xemacps_mdio_write(struct capi_eth_mac_handle *handle,
		       uint8_t phy_addr, uint8_t reg_addr, uint16_t data);

/**
 * @brief CAPI MDIO bus ops table backed by the GEM's MDIO peripheral.
 *
 * Pair with capi_mdio_init(); pass the parent MAC handle in
 * @ref xemacps_mdio_init_config.mac and put that struct's address in
 * capi_mdio_init_config.extra.
 */
extern const struct capi_mdio_ops xemacps_capi_mdio_ops;

/**
 * @brief Init parameters for xemacps_capi_mdio_ops.
 *
 * Passed via capi_mdio_init_config.extra.
 */
struct xemacps_mdio_init_config {
	/** Parent MAC handle (already initialised via capi_eth_mac_init). */
	struct capi_eth_mac_handle *mac;
};

/*
 * GEM RGMII TX_CLK divisors and clock-generator register windows.
 *
 * Register bases come from the BSP where possible:
 *   - Zynq-7000  SLCR : XPS_SYS_CTRL_BASEADDR (xparameters_ps.h)
 *   - ZynqMP    CRL_APB: XPAR_PSU_CRL_APB_BASEADDR (xparameters.h)
 *
 * GEM PS instance base addresses are SoC-fixed (not BSP-driven — the BSP
 * only emits XPAR_XEMACPS_n_BASEADDR for enabled instances), so the four
 * possible bases stay named here for dispatch.
 */

/* Zynq-7000 SLCR window (relative to XPS_SYS_CTRL_BASEADDR = 0xF8000000) */
#define GEM_ZYNQ_SLCR_LOCK_OFF		0x004U
#define GEM_ZYNQ_SLCR_UNLOCK_OFF	0x008U
#define GEM_ZYNQ_SLCR_GEM0_CLK_CTRL_OFF	0x140U
#define GEM_ZYNQ_SLCR_GEM1_CLK_CTRL_OFF	0x144U
#define GEM_ZYNQ_SLCR_UNLOCK_KEY	0x0000DF0DU
#define GEM_ZYNQ_SLCR_LOCK_KEY		0x0000767BU
#define GEM_ZYNQ_SLCR_DIV_MASK		0xFC0FC0FFU
#define GEM_ZYNQ_SLCR_DIV0_SHIFT	8
#define GEM_ZYNQ_SLCR_DIV1_SHIFT	20

/* Zynq-7000 GEM PS base addresses (used for dispatch between SLCR/CRL paths) */
#define GEM_ZYNQ_GEM0_BASE		0xE000B000U
#define GEM_ZYNQ_GEM1_BASE		0xE000C000U

/* ZynqMP CRL_APB GEMx_REF_CTRL offsets (relative to XPAR_PSU_CRL_APB_BASEADDR) */
#define GEM_ZYNQMP_CRL_GEM0_REF_CTRL_OFF	0x50U
#define GEM_ZYNQMP_CRL_GEM1_REF_CTRL_OFF	0x54U
#define GEM_ZYNQMP_CRL_GEM2_REF_CTRL_OFF	0x58U
#define GEM_ZYNQMP_CRL_GEM3_REF_CTRL_OFF	0x5CU
#define GEM_ZYNQMP_CRL_DIV0_SHIFT		8
#define GEM_ZYNQMP_CRL_DIV1_SHIFT		16
#define GEM_ZYNQMP_CRL_DIV0_MASK		(0x3FU << GEM_ZYNQMP_CRL_DIV0_SHIFT)
#define GEM_ZYNQMP_CRL_DIV1_MASK		(0x3FU << GEM_ZYNQMP_CRL_DIV1_SHIFT)

/* ZynqMP GEM PS base addresses */
#define GEM_ZYNQMP_GEM0_BASE		0xFF0B0000U
#define GEM_ZYNQMP_GEM1_BASE		0xFF0C0000U
#define GEM_ZYNQMP_GEM2_BASE		0xFF0D0000U
#define GEM_ZYNQMP_GEM3_BASE		0xFF0E0000U

struct gem_clk_divs {
	uint8_t div0;
	uint8_t div1;
};

/* Divisor tables — indexed by speed (Mbps). See the doc comment above for
 * source-clock assumptions.
 */
static const struct gem_clk_divs gem_zynq_divs[] = {
	[10]   = { .div0 = 8,  .div1 = 50 },
	[100]  = { .div0 = 8,  .div1 = 5  },
	[1000] = { .div0 = 8,  .div1 = 1  },
};

#ifdef XPAR_PSU_CRL_APB_BASEADDR
static const struct gem_clk_divs gem_zynqmp_divs[] = {
	[10]   = { .div0 = 12, .div1 = 50 },
	[100]  = { .div0 = 12, .div1 = 5  },
	[1000] = { .div0 = 12, .div1 = 1  },
};
#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CAPI_XEMACPS_H_ */
