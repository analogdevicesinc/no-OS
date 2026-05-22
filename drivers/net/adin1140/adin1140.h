/******************************************************************************
 *   @file   adin1140.h
 *   @brief  Header file of the ADIN1140 driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#ifndef _ADIN1140_H
#define _ADIN1140_H

#include <stdbool.h>
#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_util.h"
#include "oa_tc6.h"

#define ADIN1140_ETH_ALEN		6
#define ADIN1140_ETH_HDR_LEN		14

/* MACPHY identification */
#define ADIN1140_PHY_ID			0x0283BE00

/* MMS 0: Standard OA TC6 registers */
#define ADIN1140_MACPHY_ID_REG		OA_MMS_REG(0x0, 0x0001)
#define ADIN1140_CONFIG0_REG		OA_MMS_REG(0x0, 0x0004)
#define ADIN1140_CONFIG2_REG		OA_MMS_REG(0x0, 0x0006)
#define ADIN1140_STATUS0_REG		OA_MMS_REG(0x0, 0x0008)
#define ADIN1140_STATUS1_REG		OA_MMS_REG(0x0, 0x0009)
#define ADIN1140_IMASK1_REG		OA_MMS_REG(0x0, 0x000D)

/* CONFIG0 bits */
#define ADIN1140_CONFIG0_SYNC		NO_OS_BIT(15)
#define ADIN1140_CONFIG0_TXFCSVE	NO_OS_BIT(14)
#define ADIN1140_CONFIG0_CSARFE		NO_OS_BIT(13)
#define ADIN1140_CONFIG0_ZARFE		NO_OS_BIT(12)
#define ADIN1140_CONFIG0_TXCTHRESH_MASK	NO_OS_GENMASK(11, 10)
#define ADIN1140_CONFIG0_TXCTE		NO_OS_BIT(9)
#define ADIN1140_CONFIG0_RXCTE		NO_OS_BIT(8)
#define ADIN1140_CONFIG0_FTSE		NO_OS_BIT(7)
#define ADIN1140_CONFIG0_FTSS		NO_OS_BIT(6)
#define ADIN1140_CONFIG0_CPS_MASK	NO_OS_GENMASK(2, 0)
#define ADIN1140_CONFIG0_CPS_64		NO_OS_GENMASK(2, 1)

/* CONFIG2 bits */
#define ADIN1140_CONFIG2_FWD_UNK2HOST	NO_OS_BIT(2)

/* STATUS0 bits */
#define ADIN1140_STATUS0_RESETC		NO_OS_BIT(6)

/* STATUS1 bits */
#define ADIN1140_STATUS1_LINK_STATE	NO_OS_BIT(0)

/* MMS 1: MAC registers */
#define ADIN1140_MAC_ADDR_FILT_UPR_REG(x)	OA_MMS_REG(0x1, 0x50 + 2 * (x))
#define ADIN1140_MAC_ADDR_FILT_LWR_REG(x)	OA_MMS_REG(0x1, 0x51 + 2 * (x))
#define ADIN1140_MAC_ADDR_MASK_UPR_REG(x)	OA_MMS_REG(0x1, 0x70 + 2 * (x))
#define ADIN1140_MAC_ADDR_MASK_LWR_REG(x)	OA_MMS_REG(0x1, 0x71 + 2 * (x))
#define ADIN1140_MAC_P1_LOOP_REG		OA_MMS_REG(0x1, 0xC4)

/* MAC address filter flags */
#define ADIN1140_MAC_ADDR_APPLY2PORT1	NO_OS_BIT(30)
#define ADIN1140_MAC_ADDR_TO_HOST	NO_OS_BIT(16)

/* Filter slot assignments */
#define ADIN1140_MAC_FILT_MULTICAST_SLOT	0
#define ADIN1140_MAC_FILT_BROADCAST_SLOT	1
#define ADIN1140_MAC_FILT_UNICAST_SLOT		2
#define ADIN1140_MAC_FILT_MAX_SLOT		16

/* PHY C45 MMS mapping */
#define ADIN1140_PHY_C45_PCS_MMS	2
#define ADIN1140_PHY_C45_PMA_PMD_MMS	3
#define ADIN1140_PHY_C45_VS_PLCA_MMS	4

/* C45 MMD device numbers */
#define ADIN1140_MMD_PMA_PMD		1
#define ADIN1140_MMD_PCS		3
#define ADIN1140_MMD_VEND2		31

/* PLCA registers (MMS 4 = MMD 31) */
#define ADIN1140_PLCA_IDVER_REG		OA_MMS_REG(0x4, 0xCA00)
#define ADIN1140_PLCA_CTRL0_REG		OA_MMS_REG(0x4, 0xCA01)
#define ADIN1140_PLCA_CTRL1_REG		OA_MMS_REG(0x4, 0xCA02)
#define ADIN1140_PLCA_STATUS_REG	OA_MMS_REG(0x4, 0xCA03)
#define ADIN1140_PLCA_TOTMR_REG		OA_MMS_REG(0x4, 0xCA04)
#define ADIN1140_PLCA_BURST_REG		OA_MMS_REG(0x4, 0xCA05)

/* PLCA CTRL0 bits */
#define ADIN1140_PLCA_CTRL0_EN		NO_OS_BIT(15)
#define ADIN1140_PLCA_CTRL0_RST	NO_OS_BIT(14)

/* PLCA CTRL1 bits */
#define ADIN1140_PLCA_CTRL1_NCNT_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1140_PLCA_CTRL1_ID_MASK	NO_OS_GENMASK(7, 0)

/* PLCA STATUS bits */
#define ADIN1140_PLCA_STATUS_PST	NO_OS_BIT(15)

/* PLCA TOTMR bits */
#define ADIN1140_PLCA_TOTMR_MASK	NO_OS_GENMASK(7, 0)

/* PLCA BURST bits */
#define ADIN1140_PLCA_BURST_MAXBC_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1140_PLCA_BURST_BTMR_MASK	NO_OS_GENMASK(7, 0)

/* MMS 1: Statistics registers */
#define ADIN1140_RX_FRAME_CNT_REG	OA_MMS_REG(0x1, 0xA1)
#define ADIN1140_RX_BCAST_CNT_REG	OA_MMS_REG(0x1, 0xA2)
#define ADIN1140_RX_MCAST_CNT_REG	OA_MMS_REG(0x1, 0xA3)
#define ADIN1140_RX_UCAST_CNT_REG	OA_MMS_REG(0x1, 0xA4)
#define ADIN1140_RX_CRC_ERR_CNT_REG	OA_MMS_REG(0x1, 0xA5)
#define ADIN1140_RX_ALGN_ERR_CNT_REG	OA_MMS_REG(0x1, 0xA6)
#define ADIN1140_TX_FRAME_CNT_REG	OA_MMS_REG(0x1, 0xB1)
#define ADIN1140_TX_BCAST_CNT_REG	OA_MMS_REG(0x1, 0xB2)
#define ADIN1140_TX_MCAST_CNT_REG	OA_MMS_REG(0x1, 0xB3)
#define ADIN1140_TX_UCAST_CNT_REG	OA_MMS_REG(0x1, 0xB4)

/**
 * @brief ADIN1140 device descriptor.
 */
struct adin1140_desc {
	struct no_os_spi_desc *comm_desc;
	uint8_t mac_address[ADIN1140_ETH_ALEN];
	struct oa_tc6_desc *oa_desc;
	volatile bool irq_pending;
};

/**
 * @brief MAC CONFIG0 user-configurable bitfields.
 *        TXFCSVE is always cleared so the MAC appends the FCS itself; SYNC,
 *        PROTE, SEQE are managed internally and not exposed here.
 */
struct adin1140_mac_cfg {
	uint8_t cps;       /**< Chunk Payload Size (3-bit raw value) */
	uint8_t txcthresh; /**< TX cut-through threshold (2-bit raw value) */
	bool    txcte;     /**< TX cut-through enable */
	bool    rxcte;     /**< RX cut-through enable */
	bool    csarfe;    /**< Cut-through Status Append RX Frame Enable */
	bool    zarfe;     /**< Zero-align RX Frame Enable */
	bool    ftse;      /**< Frame Timestamp Enable */
	bool    ftss;      /**< Frame Timestamp Size */
};

/**
 * @brief PLCA (Physical Layer Collision Avoidance) configuration.
 */
struct adin1140_plca_cfg {
	bool enabled;
	uint8_t node_id;
	uint8_t node_cnt;
	uint8_t to_tmr;
	uint8_t burst_cnt;
	uint8_t burst_tmr;
};

/**
 * @brief Initialization parameter for the device descriptor.
 */
struct adin1140_init_param {
	struct no_os_spi_init_param comm_param;
	struct adin1140_mac_cfg mac_cfg;
	struct adin1140_plca_cfg plca_cfg;
	uint8_t mac_address[ADIN1140_ETH_ALEN];
};

/**
 * @brief Buffer structure used for frame RX and TX transactions.
 */
struct adin1140_eth_buff {
	uint32_t len;
	uint8_t mac_dest[ADIN1140_ETH_ALEN];
	uint8_t mac_source[ADIN1140_ETH_ALEN];
	uint8_t ethertype[2];
	uint8_t *payload;
};

static bool adin1140_irq_triggered(struct adin1140_desc *desc)
{
	return desc->irq_pending;
}

/* Write a register */
int adin1140_reg_write(struct adin1140_desc *desc, uint32_t addr, uint32_t data);

/* Read a register */
int adin1140_reg_read(struct adin1140_desc *desc, uint32_t addr, uint32_t *data);

/* Update a register's value based on a mask */
int adin1140_reg_update(struct adin1140_desc *desc, uint32_t addr,
			uint32_t mask, uint32_t data);

/* Read a PHY register using clause 45 via MMS */
int adin1140_mdio_read_c45(struct adin1140_desc *desc, uint32_t devnum,
			   uint16_t reg, uint16_t *data);

/* Write a PHY register using clause 45 via MMS */
int adin1140_mdio_write_c45(struct adin1140_desc *desc, uint32_t devnum,
			    uint16_t reg, uint16_t data);

/* Set the unicast MAC address filter */
int adin1140_set_mac_addr(struct adin1140_desc *desc,
			  uint8_t mac_address[ADIN1140_ETH_ALEN]);

/* Enable/disable the broadcast filter */
int adin1140_broadcast_filter(struct adin1140_desc *desc, bool enabled);

/* Enable/disable the multicast filter */
int adin1140_multicast_filter(struct adin1140_desc *desc, bool enabled);

/* Set/clear promiscuous mode */
int adin1140_set_promisc(struct adin1140_desc *desc, bool promisc);

/* Set the MAC CONFIG0 bitfields exposed via struct adin1140_mac_cfg */
int adin1140_mac_set_cfg(struct adin1140_desc *desc,
			 const struct adin1140_mac_cfg *cfg);

/* Get the MAC CONFIG0 bitfields */
int adin1140_mac_get_cfg(struct adin1140_desc *desc,
			 struct adin1140_mac_cfg *cfg);

/* Trigger a MAC-PHY soft reset and re-assert SYNC */
int adin1140_sw_reset(struct adin1140_desc *desc);

/* Write a frame to the TX FIFO */
int adin1140_write_fifo(struct adin1140_desc *desc,
			struct adin1140_eth_buff *eth_buff);

/* Read a frame from the RX FIFO */
int adin1140_read_fifo(struct adin1140_desc *desc,
		       struct adin1140_eth_buff *eth_buff);

/* Set the PLCA configuration */
int adin1140_plca_set_cfg(struct adin1140_desc *desc,
			  const struct adin1140_plca_cfg *cfg);

/* Get the PLCA configuration */
int adin1140_plca_get_cfg(struct adin1140_desc *desc,
			  struct adin1140_plca_cfg *cfg);

/* Get the PLCA status */
int adin1140_plca_get_status(struct adin1140_desc *desc, bool *active);

/* Reset the PLCA logic via CTRL0.PLCARST */
int adin1140_plca_reset(struct adin1140_desc *desc);

/* Signal that an interrupt has occurred (call from ISR context) */
void adin1140_set_irq_flag(struct adin1140_desc *desc);

/* Process pending data if an interrupt has fired */
int adin1140_poll(struct adin1140_desc *desc);

/* Get the link state */
int adin1140_link_state(struct adin1140_desc *desc, uint32_t *state);

/* Initialize the device */
int adin1140_init(struct adin1140_desc **desc,
		  struct adin1140_init_param *param);

/* Free a device descriptor */
int adin1140_remove(struct adin1140_desc *desc);

#endif
