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
#include "no_os_util.h"

/* Number of RX buffer descriptors pre-posted to hardware */
#define XGEM_RX_BD_COUNT		32

#if defined(__aarch64__)
#define XGEM_CACHE_LINE_SIZE	64
#else
#define XGEM_CACHE_LINE_SIZE	32
#endif

/*
 * Frame buffer size, rounded up to the next cache-line multiple.
 */
#define XGEM_BUFF_SIZE	(((XEMACPS_MAX_FRAME_SIZE) + XGEM_CACHE_LINE_SIZE - 1u) \
							& ~(XGEM_CACHE_LINE_SIZE - 1u))

/* PHY register definitions (IEEE 802.3) */
#define PHY_REG_BMCR				0x00
#define PHY_REG_BMSR				0x01
#define PHY_REG_BMSR_LSTATUS		NO_OS_BIT(2)
#define PHY_REG_BMSR_ANEGCMPL		NO_OS_BIT(5)
#define PHY_REG_BMCR_ANEGEN			NO_OS_BIT(12)
#define PHY_REG_BMCR_RSTANEG		NO_OS_BIT(9)
#define PHY_REG_ANLPAR				0x05
#define PHY_REG_ANLPAR_100FD		NO_OS_BIT(8)
#define PHY_REG_ANLPAR_100HD		NO_OS_BIT(7)
#define PHY_REG_1000STAT			0x0A
#define PHY_REG_1000STAT_LP_FD		NO_OS_BIT(11)
#define PHY_REG_1000STAT_LP_HD		NO_OS_BIT(10)

/* Maximum Ethernet frame size, rounded up to a 32-byte cache-line multiple. */
#ifndef XEMACPS_MAX_FRAME_SIZE
#define XEMACPS_MAX_FRAME_SIZE	1536
#endif

/* MAC address length */
#define XEMACPS_ETH_ALEN	6

/* Opaque driver descriptor - full definition in xemacps.c */
struct xemacps_desc;

/**
 * @struct xemacps_init_param
 * @brief  Initialization parameters for the XEmacPs MAC driver.
 */
struct xemacps_init_param {
	/** XEmacPs device ID from xparameters.h (e.g. XPAR_XEMACPS_0_DEVICE_ID) */
	uint16_t device_id;
	/** MDIO address of the on-board PHY */
	uint32_t phy_addr;
	/** Ethernet MAC address for this interface */
	uint8_t hwaddr[XEMACPS_ETH_ALEN];
};

int xemacps_init(struct xemacps_desc **desc, struct xemacps_init_param *param);
int xemacps_remove(struct xemacps_desc *desc);
int xemacps_send(struct xemacps_desc *desc, uint8_t *buf, uint32_t len);
int xemacps_recv(struct xemacps_desc *desc, uint8_t *buf, uint32_t *len);
int xemacps_poll(struct xemacps_desc *desc);
int xemacps_set_mcast_hash(struct xemacps_desc *desc, uint8_t *addr);

#endif /* __XEMACPS_DRV_H__ */
