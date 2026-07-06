/***************************************************************************//**
 *   @file   capi_marvell_88e1510.h
 *   @brief  Capi driver header for the Marvell 88E1510 PHY models.
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

#ifndef _CAPI_MARVELL_88E1510_H_
#define _CAPI_MARVELL_88E1510_H_

#include <stdint.h>
#include <stdbool.h>
#include <capi_eth_phy.h>
#include "no_os_util.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* IEEE 802.3 Standard MII Registers (Page 0) */
#define MRVL_REG_CONTROL                0x00
#define MRVL_REG_STATUS                 0x01
#define MRVL_REG_PHY_ID1               0x02
#define MRVL_REG_PHY_ID2               0x03
#define MRVL_REG_AUTONEG_ADV           0x04
#define MRVL_REG_LP_ABILITY            0x05
#define MRVL_REG_AUTONEG_EXPANSION     0x06
#define MRVL_REG_1000BT_CONTROL        0x09
#define MRVL_REG_1000BT_STATUS         0x0A

/* Control Register (0x00) bits */
#define MRVL_CTRL_RESET                NO_OS_BIT(15)
#define MRVL_CTRL_LOOPBACK             NO_OS_BIT(14)
#define MRVL_CTRL_SPEED_LSB            NO_OS_BIT(13)
#define MRVL_CTRL_AUTONEG_EN           NO_OS_BIT(12)
#define MRVL_CTRL_POWER_DOWN           NO_OS_BIT(11)
#define MRVL_CTRL_ISOLATE              NO_OS_BIT(10)
#define MRVL_CTRL_RESTART_AUTONEG      NO_OS_BIT(9)
#define MRVL_CTRL_DUPLEX               NO_OS_BIT(8)
#define MRVL_CTRL_SPEED_MSB            NO_OS_BIT(6)

/* Status Register (0x01) bits */
#define MRVL_STATUS_100BT4             NO_OS_BIT(15)
#define MRVL_STATUS_100FD              NO_OS_BIT(14)
#define MRVL_STATUS_100HD              NO_OS_BIT(13)
#define MRVL_STATUS_10FD               NO_OS_BIT(12)
#define MRVL_STATUS_10HD               NO_OS_BIT(11)
#define MRVL_STATUS_AUTONEG_COMPLETE   NO_OS_BIT(5)
#define MRVL_STATUS_REMOTE_FAULT       NO_OS_BIT(4)
#define MRVL_STATUS_LINK               NO_OS_BIT(2)

/* Marvell 88E1510 PHY Identifier */
#define MARVELL_PHY_ID					0x005043

/* Marvell-specific Registers (Page 0) */
#define MRVL_REG_PHY_SPEC_CTRL        0x10
#define MRVL_REG_PHY_SPEC_STATUS      0x11
#define MRVL_REG_IRQ_ENABLE           0x12
#define MRVL_REG_IRQ_STATUS           0x13
#define MRVL_REG_PAGE_SELECT          0x16

/* PHY Specific Status Register (0x11) bits */
#define MRVL_PSS_SPEED_MASK           (3U << 14)
#define MRVL_PSS_SPEED_1000           (2U << 14)
#define MRVL_PSS_SPEED_100            NO_OS_BIT(14)
#define MRVL_PSS_SPEED_10             (0U << 14)
#define MRVL_PSS_DUPLEX               NO_OS_BIT(13)
#define MRVL_PSS_LINK                 NO_OS_BIT(10)
#define MRVL_PSS_MDIX                 NO_OS_BIT(6)

/* PHY Specific Control Register (0x10) bits */
#define MRVL_PSC_DOWNSHIFT_CNT_SHIFT  12
#define MRVL_PSC_DOWNSHIFT_CNT_MASK   (7U << 12)
#define MRVL_PSC_DOWNSHIFT_EN         NO_OS_BIT(11)
#define MRVL_PSC_MDIX_AUTO            (3U << 5)
#define MRVL_PSC_MDIX_FORCE_MDI       (0U << 5)
#define MRVL_PSC_MDIX_FORCE_MDIX      NO_OS_BIT(5)
#define MRVL_PSC_MDIX_MASK            (3U << 5)

/* Auto-Negotiation Advertisement Register (0x04) bits */
#define MRVL_ADV_100FD                NO_OS_BIT(8)
#define MRVL_ADV_100HD                NO_OS_BIT(7)
#define MRVL_ADV_10FD                 NO_OS_BIT(6)
#define MRVL_ADV_10HD                 NO_OS_BIT(5)
#define MRVL_ADV_SELECTOR_IEEE        0x0001U

/* 1000BASE-T Control Register (0x09) bits */
#define MRVL_1000BT_ADV_1000FD        NO_OS_BIT(9)
#define MRVL_1000BT_ADV_1000HD        NO_OS_BIT(8)

/* Interrupt Enable/Status Register bits */
#define MRVL_IRQ_LINK_STATUS_CHANGE   NO_OS_BIT(10)
#define MRVL_IRQ_AUTONEG_COMPLETE     NO_OS_BIT(11)
#define MRVL_IRQ_SPEED_CHANGE         NO_OS_BIT(14)
#define MRVL_IRQ_DUPLEX_CHANGE        NO_OS_BIT(13)

/* Page numbers */
#define MRVL_PAGE_COPPER              0
#define MRVL_PAGE_MAC_SPEC            2
#define MRVL_PAGE_LED                 3
#define MRVL_PAGE_RGMII_DELAY         2

/* Page 2 registers */
#define MRVL_REG_MAC_SPEC_CTRL        0x15
#define MRVL_MAC_SPEC_RGMII_RX_DELAY  NO_OS_BIT(5)
#define MRVL_MAC_SPEC_RGMII_TX_DELAY  NO_OS_BIT(4)

/**
 * @brief Marvell 88E1510 RGMII timing configuration
 */
struct mrvl_88e1510_rgmii_config {
	bool rx_delay_en; /**< Enable RGMII RX clock delay (adds ~2ns) */
	bool tx_delay_en; /**< Enable RGMII TX clock delay (adds ~2ns) */
};

/**
 * @brief Marvell 88E1510 platform-specific init configuration
 *
 * Pass a pointer to this structure in capi_eth_phy_init_config.extra
 */
struct mrvl_88e1510_extra_config {
	struct mrvl_88e1510_rgmii_config rgmii; /**< RGMII delay settings */
	bool downshift_en;                       /**< Enable speed downshift on link failures */
	uint8_t downshift_retries;               /**< Number of attempts before downshift (1-8) */
};

/**
 * @brief Marvell 88E1510 driver handle
 *
 */
struct mrvl_88e1510_handle {
	struct capi_eth_phy_handle base;

	uint8_t phy_addr;
	struct capi_mdio_handle *mdio_bus;

	enum capi_eth_interface interface;
	struct mrvl_88e1510_rgmii_config rgmii;
	bool downshift_en;
	uint8_t downshift_retries;

	capi_eth_phy_event_callback callback;
	void *callback_arg;
	uint32_t irq_mask;
};

/**
 * @brief Marvell 88E1510 ops table — assign to capi_eth_phy_init_config.ops
 */
extern const struct capi_eth_phy_ops mrvl_88e1510_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CAPI_MARVELL_88E1510_H_ */
