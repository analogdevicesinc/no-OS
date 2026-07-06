/***************************************************************************//**
 *   @file   capi_dp83867.h
 *   @brief  Capi driver header for the TI DP83867 gigabit Ethernet PHY.
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

#ifndef _CAPI_DP83867_H_
#define _CAPI_DP83867_H_

#include <stdint.h>
#include <stdbool.h>
#include <capi_eth_phy.h>
#include "no_os_util.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* DP83867 PHY Identifier (OUI + model, as reported by PHYID1/PHYID2) */
#define DP83867_PHY_ID                          0x2000a231
#define DP83867_PHY_ID_MASK                     0xfffffff0

/* IEEE 802.3 standard MII registers (clause 22) */
#define DP83867_REG_CONTROL                     0x00
#define DP83867_REG_STATUS                      0x01
#define DP83867_REG_PHY_ID1                     0x02
#define DP83867_REG_PHY_ID2                     0x03
#define DP83867_REG_AUTONEG_ADV                 0x04
#define DP83867_REG_1000BT_CONTROL              0x09

/* Control Register (0x00) bits */
#define DP83867_CTRL_RESET                      NO_OS_BIT(15)
#define DP83867_CTRL_LOOPBACK                   NO_OS_BIT(14)
#define DP83867_CTRL_SPEED_LSB                  NO_OS_BIT(13)
#define DP83867_CTRL_AUTONEG_EN                 NO_OS_BIT(12)
#define DP83867_CTRL_POWER_DOWN                 NO_OS_BIT(11)
#define DP83867_CTRL_ISOLATE                    NO_OS_BIT(10)
#define DP83867_CTRL_RESTART_AUTONEG            NO_OS_BIT(9)
#define DP83867_CTRL_DUPLEX                     NO_OS_BIT(8)
#define DP83867_CTRL_SPEED_MSB                  NO_OS_BIT(6)

/* Auto-Negotiation Advertisement (0x04) bits */
#define DP83867_ADV_100FD                       NO_OS_BIT(8)
#define DP83867_ADV_100HD                       NO_OS_BIT(7)
#define DP83867_ADV_10FD                        NO_OS_BIT(6)
#define DP83867_ADV_10HD                        NO_OS_BIT(5)
#define DP83867_ADV_SELECTOR_IEEE               0x0001U

/* 1000BASE-T Control (0x09) bits */
#define DP83867_1000BT_ADV_1000FD               NO_OS_BIT(9)
#define DP83867_1000BT_ADV_1000HD               NO_OS_BIT(8)

/* Vendor registers (direct access, clause 22) */
#define DP83867_REG_PHYCR                       0x10
#define DP83867_REG_PHYSTS                      0x11
#define DP83867_REG_MICR                        0x12
#define DP83867_REG_ISR                         0x13
#define DP83867_REG_CFG2                        0x14
#define DP83867_REG_LEDCR1                      0x18
#define DP83867_REG_LEDCR2                      0x19
#define DP83867_REG_CFG3                        0x1e
#define DP83867_REG_CTRL_EXT                    0x1f

/* PHYSTS (0x11) — live link status */
#define DP83867_PHYSTS_1000                     NO_OS_BIT(15)
#define DP83867_PHYSTS_100                      NO_OS_BIT(14)
#define DP83867_PHYSTS_DUPLEX                   NO_OS_BIT(13)
#define DP83867_PHYSTS_LINK                     NO_OS_BIT(10)
#define DP83867_PHYSTS_MDIX                     NO_OS_BIT(6)

/* PHYCR (0x10) — auto MDI-X control */
#define DP83867_PHYCR_MDI_CROSSOVER_MASK        NO_OS_GENMASK(6, 5)
#define DP83867_PHYCR_MDI_CROSSOVER_MANUAL_MDI  0x0
#define DP83867_PHYCR_MDI_CROSSOVER_MANUAL_MDIX 0x1
#define DP83867_PHYCR_MDI_CROSSOVER_AUTO        0x2

/* MICR (0x12) — Micro interrupt control */
#define DP83867_MICR_LINK_STS_CHNG_INT_EN       NO_OS_BIT(10)

/* ISR (0x13) — read-to-clear */
#define DP83867_ISR_LINK_STS_CHNG               NO_OS_BIT(10)

/* CTRL_EXT (0x1F) */
#define DP83867_SW_RESET                        NO_OS_BIT(15)
#define DP83867_SW_RESTART                      NO_OS_BIT(14)

/* CFG3 (0x1E) */
#define DP83867_CFG3_INT_OE                     NO_OS_BIT(7)

/* Vendor extended registers live in MMD device address 0x1F. They can be
 * reached either via a native Clause 45 access (preferred) or via the
 * standard C22-indirect MMD access at registers 0x0D/0x0E — the CAPI MDIO
 * thin layer picks the fast path when the bus advertises c45 ops and
 * falls back to the indirect sequence otherwise.
 */
#define DP83867_DEVAD                           0x1f

/* Extended registers (MMD 0x1F) */
#define DP83867_REG_RGMIICTL                    0x0032
#define DP83867_REG_RGMIIDCTL                   0x0086
#define DP83867_REG_IO_MUX_CFG                  0x0170

/* RGMIICTL bits */
#define DP83867_RGMII_TX_CLK_DELAY_EN           NO_OS_BIT(1)
#define DP83867_RGMII_RX_CLK_DELAY_EN           NO_OS_BIT(0)

/* RGMIIDCTL — TX delay in bits [7:4], RX delay in bits [3:0].
 * Delay codes match dt-bindings/net/ti-dp83867.h (0=250ps..0xF=4ns, step 250ps).
 */
#define DP83867_RGMIIDCTL_TX_DELAY_MASK         NO_OS_GENMASK(7, 4)
#define DP83867_RGMIIDCTL_RX_DELAY_MASK         NO_OS_GENMASK(3, 0)
#define DP83867_RGMII_DELAY_2_00_NS             0x7  /* Typical / reset default */

/* LEDCR1 (0x18) — LED[3:0] function select, 4-bit field per LED */
#define DP83867_LED_FN_LED0_MASK                NO_OS_GENMASK(3, 0)
#define DP83867_LED_FN_LED1_MASK                NO_OS_GENMASK(7, 4)
#define DP83867_LED_FN_LED2_MASK                NO_OS_GENMASK(11, 8)
#define DP83867_LED_FN_LED3_MASK                NO_OS_GENMASK(15, 12)

/* LED function codes (see TI DP83867 datasheet Table 8-32) — same encoding
 * for every LED slot, unlike the Marvell 88E1510.
 */
#define DP83867_LED_FN_LINK                     0x0
#define DP83867_LED_FN_RX_TX                    0x1
#define DP83867_LED_FN_TX                       0x2
#define DP83867_LED_FN_RX                       0x3
#define DP83867_LED_FN_COLLISION                0x4
#define DP83867_LED_FN_LINK_1000                0x5
#define DP83867_LED_FN_LINK_100                 0x6
#define DP83867_LED_FN_LINK_10                  0x7
#define DP83867_LED_FN_LINK_10_100              0x8
#define DP83867_LED_FN_LINK_100_1000            0x9
#define DP83867_LED_FN_FULL_DUPLEX              0xa
#define DP83867_LED_FN_LINK_RX_TX               0xb
#define DP83867_LED_FN_RX_TX_ERR                0xd
#define DP83867_LED_FN_RX_ERR                   0xe

/* Standard LED assignment for ZCU102-style boards:
 *   LED[0] = activity blink (RX or TX)
 *   LED[1] = solid link at 100M or 1G
 *   LED[2] = 1000M-only link indicator
 *   LED[3] = unused (leave in reset default)
 */
#define DP83867_LEDCR1_DEFAULT \
	(no_os_field_prep(DP83867_LED_FN_LED0_MASK, DP83867_LED_FN_RX_TX) | \
	 no_os_field_prep(DP83867_LED_FN_LED1_MASK, DP83867_LED_FN_LINK_100_1000) | \
	 no_os_field_prep(DP83867_LED_FN_LED2_MASK, DP83867_LED_FN_LINK_1000))

/**
 * @brief DP83867 RGMII timing configuration
 */
struct dp83867_rgmii_config {
	/** Enable internal RX clock delay */
	bool rx_delay_en;
	/** Enable internal TX clock delay */
	bool tx_delay_en;
	/** RX delay code (0=250ps .. 0xF=4ns, step 250ps). Ignored if rx_delay_en=false. */
	uint8_t rx_delay_code;
	/** TX delay code (0=250ps .. 0xF=4ns, step 250ps). Ignored if tx_delay_en=false. */
	uint8_t tx_delay_code;
};

/**
 * @brief DP83867 platform-specific init configuration
 *
 * Pass a pointer to this structure in capi_eth_phy_init_config.extra
 */
struct dp83867_extra_config {
	/** RGMII delay configuration (used when interface == RGMII) */
	struct dp83867_rgmii_config rgmii;
};

/**
 * @brief DP83867 driver handle
 */
struct dp83867_handle {
	struct capi_eth_phy_handle base;

	uint8_t phy_addr;
	struct capi_mdio_handle *mdio_bus;

	enum capi_eth_interface interface;
	struct dp83867_rgmii_config rgmii;

	capi_eth_phy_event_callback callback;
	void *callback_arg;
	uint16_t irq_mask;
};

/**
 * @brief DP83867 ops table — assign to capi_eth_phy_init_config.ops
 */
extern const struct capi_eth_phy_ops dp83867_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CAPI_DP83867_H_ */
