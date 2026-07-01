/***************************************************************************//**
 *   @file   no_os_xemacps.h
 *   @brief  Public API for the Xilinx PS GEM (XEmacPs) MAC driver.
 *
 *   Standalone Ethernet MAC driver for the Zynq-7000 / ZynqMP Processing
 *   System Gigabit Ethernet MAC.
 *
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#ifndef __XEMACPS_DRV_H__
#define __XEMACPS_DRV_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_util.h"

/* Number of RX buffer descriptors pre-posted to hardware */
#define XGEM_RX_BD_COUNT		64
#define XGEM_TX_BD_COUNT		1

#if defined(__aarch64__)
#define XGEM_CACHE_LINE_SIZE	64
#else
#define XGEM_CACHE_LINE_SIZE	32
#endif

/* Frame buffer size, aligned to the next cache-line size. */
#define XGEM_BUFF_SIZE	no_os_align(XEMACPS_MAX_FRAME_SIZE, XGEM_CACHE_LINE_SIZE)


/* PHY register definitions (IEEE 802.3) */
#define PHY_REG_BMCR				0x00
#define PHY_REG_BMSR				0x01
#define PHY_REG_PHYID1				0x02
#define PHY_REG_PHYID2				0x03
#define PHY_REG_BMSR_LSTATUS		NO_OS_BIT(2)
#define PHY_REG_BMSR_ANEGCMPL		NO_OS_BIT(5)
#define PHY_REG_BMCR_RESET			NO_OS_BIT(15)
#define PHY_REG_BMCR_ANEGEN			NO_OS_BIT(12)
#define PHY_REG_BMCR_RSTANEG		NO_OS_BIT(9)
#define PHY_REG_ANLPAR				0x05
#define PHY_REG_ANLPAR_100FD		NO_OS_BIT(8)
#define PHY_REG_ANLPAR_100HD		NO_OS_BIT(7)
#define PHY_REG_1000STAT			0x0A
#define PHY_REG_1000STAT_LP_FD		NO_OS_BIT(11)
#define PHY_REG_1000STAT_LP_HD		NO_OS_BIT(10)

/* Marvell 88E1510/88E1512 page and register definitions */
#define MARVELL_PHY_ID				0x005043
#define PHY_REG_PAGE				22
#define MRVL_PAGE2_RGMII_CTRL			21
/* Page 2 Reg 21 values: enable RGMII TX and RX delays per speed */
#define MRVL_RGMII_1000_DELAYS			0x0070
#define MRVL_RGMII_100_DELAYS			0x2030
#define MRVL_RGMII_10_DELAYS			0x0030

/* TI DP83867 (ZCU102 on-board PHY) register definitions */
#define TI_PHY_ID				0x080028
/* MMD indirect access registers (Clause 22 window into Clause 45 space) */
#define PHY_REG_MMD_CTRL			0x0D
#define PHY_REG_MMD_DATA			0x0E
#define TI_MMD_DEVAD				0x1F	/* vendor MMD */
/* MMD_CTRL function field: 0 = address, 0x4000 = data, no post-increment */
#define PHY_MMD_CTRL_ADDR			0x0000
#define PHY_MMD_CTRL_DATA			0x4000
/* DP83867 RGMII control / delay registers (in vendor MMD 0x1F) */
#define DP83867_RGMIICTL			0x0032
#define DP83867_RGMIIDCTL			0x0086
/* RGMIICTL bits: enable TX and RX internal delays */
#define DP83867_RGMII_TX_CLK_DELAY_EN		NO_OS_BIT(1)
#define DP83867_RGMII_RX_CLK_DELAY_EN		NO_OS_BIT(0)
/* ZCU102 board delay values (from the Xilinx device tree) */
#define DP83867_ZCU102_TX_DELAY			0xA
#define DP83867_ZCU102_RX_DELAY			0x8

/*
 * ZynqMP CRL_APB GEM reference-clock control. The GEM TX clock divisor must be
 * reprogrammed for the negotiated link speed. This is normally done by the
 * FSBL/PMUFW; when the PMUFW is not running (bare-metal no-os) the driver must
 * do it, otherwise TX is clocked at the wrong rate and frames never reach the
 * link partner (link parks at 100 Mbps, no DHCP).
 */
#define CRL_APB_GEM3_REF_CTRL		0xFF5E005C
#define CRL_APB_GEM_DIV0_MASK		0x00003F00
#define CRL_APB_GEM_DIV0_SHIFT		8
#define CRL_APB_GEM_DIV1_MASK		0x003F0000
#define CRL_APB_GEM_DIV1_SHIFT		16

/* ZCU102 GEM3 TX-clock divisors per speed (from the BSP xparameters.h) */
#define GEM_TXCLK_DIV0_1000		12
#define GEM_TXCLK_DIV1_1000		1
#define GEM_TXCLK_DIV0_100		60
#define GEM_TXCLK_DIV1_100		1
#define GEM_TXCLK_DIV0_10		60
#define GEM_TXCLK_DIV1_10		10

/* Maximum Ethernet frame size, rounded up to a 32-byte cache-line multiple. */
#ifndef XEMACPS_MAX_FRAME_SIZE
#define XEMACPS_MAX_FRAME_SIZE	1536
#endif

/* MAC address length */
#define XEMACPS_ETH_ALEN	6

/* GEM feature flags. Override at build time, e.g. -DGEM_PROMISC */

/* Promiscuous mode — receive all frames regardless of address */
#ifdef GEM_PROMISC
#define GEM_NWCFG_PROMISC	XEMACPS_NWCFG_COPYALLEN_MASK
#else
#define GEM_NWCFG_PROMISC	0
#endif

/* Full duplex (enabled by default) */
#ifndef GEM_HALF_DUPLEX
#define GEM_NWCFG_DUPLEX	XEMACPS_NWCFG_FDEN_MASK
#else
#define GEM_NWCFG_DUPLEX	0
#endif

/* Reject broadcast frames */
#ifdef GEM_NO_BROADCAST
#define GEM_NWCFG_BCAST		XEMACPS_NWCFG_BCASTDI_MASK
#else
#define GEM_NWCFG_BCAST		0
#endif

/* Strip FCS from received frames */
#ifdef GEM_STRIP_FCS
#define GEM_NWCFG_FCS		XEMACPS_NWCFG_FCSREM_MASK
#else
#define GEM_NWCFG_FCS		0
#endif

/* RX checksum offload (enabled by default) */
#ifndef GEM_NO_RX_CHKSUM
#define GEM_NWCFG_RXCHKSUM	XEMACPS_NWCFG_RXCHKSUMEN_MASK
#else
#define GEM_NWCFG_RXCHKSUM	0
#endif

/* Pause frame support (enabled by default) */
#ifndef GEM_NO_PAUSE
#define GEM_NWCFG_PAUSE		XEMACPS_NWCFG_PAUSEEN_MASK
#else
#define GEM_NWCFG_PAUSE		0
#endif

/* Multicast hash filter (enabled by default) */
#ifndef GEM_NO_MCAST_HASH
#define GEM_NWCFG_MCAST		XEMACPS_NWCFG_MCASTHASHEN_MASK
#else
#define GEM_NWCFG_MCAST		0
#endif

/* Unicast hash filter */
#ifdef GEM_UCAST_HASH
#define GEM_NWCFG_UCAST		XEMACPS_NWCFG_UCASTHASHEN_MASK
#else
#define GEM_NWCFG_UCAST		0
#endif

/* SGMII / PCS mode — enable for boards using PS GTR serial Ethernet
 * Not needed for MIO/RGMII boards. */
#ifdef GEM_SGMII
#define GEM_NWCFG_SGMII		(XEMACPS_NWCFG_SGMIIEN_MASK | \
				 XEMACPS_NWCFG_PCSSEL_MASK)
#else
#define GEM_NWCFG_SGMII		0
#endif

/* Combined feature mask */
#define GEM_NWCFG_FEATURES	(GEM_NWCFG_PROMISC | GEM_NWCFG_DUPLEX | \
				 GEM_NWCFG_BCAST | GEM_NWCFG_FCS | \
				 GEM_NWCFG_RXCHKSUM | GEM_NWCFG_PAUSE | \
				 GEM_NWCFG_MCAST | GEM_NWCFG_UCAST | \
				 GEM_NWCFG_SGMII)



/* Opaque driver descriptor - full definition in xemacps.c */
struct xemacps_desc;

/**
 * @struct xemacps_init_param
 * @brief  Initialization parameters for the XEmacPs MAC driver.
 */
struct xemacps_init_param {
	/**
	 * XEmacPs identifier from xparameters.h.
	 */
	uintptr_t device_id;
	/** MDIO address of the on-board PHY */
	uint8_t phy_addr;
	/** Ethernet MAC address for this interface */
	uint8_t hwaddr[XEMACPS_ETH_ALEN];
};

int xemacps_init(struct xemacps_desc **desc, struct xemacps_init_param *param);
int xemacps_remove(struct xemacps_desc *desc);
int xemacps_send(struct xemacps_desc *desc, uint8_t *buf, uint32_t len);
int xemacps_recv(struct xemacps_desc *desc, uint8_t *buf, uint32_t *len);
int xemacps_poll(struct xemacps_desc *desc);
int xemacps_set_mcast_hash(struct xemacps_desc *desc, uint8_t *addr);
bool xemacps_get_link_state(struct xemacps_desc *desc);

#endif /* __XEMACPS_DRV_H__ */
