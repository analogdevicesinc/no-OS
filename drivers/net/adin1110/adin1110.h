/******************************************************************************
 *   @file   adin1110.h
 *   @brief  Header file of the ADIN1110 driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _ADIN1110_H
#define _ADIN1110_H

#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

#define ADIN1110_BUFF_LEN			1530
#define ADIN1110_ETH_ALEN			6
#define ADIN1110_ETHERTYPE_LEN			2
#define ADIN1110_ETH_HDR_LEN			14
#define ADIN1110_ADDR_FILT_LEN			16

#define ADIN1110_FCS_LEN			4
#define ADIN1110_MAC_LEN			6

#define ADIN1110_ADDR_MASK			NO_OS_GENMASK(12, 0)
#define ADIN1110_RD_FRAME_SIZE			7
#define ADIN1110_WR_FRAME_SIZE			6
#define ADIN1110_RD_HDR_SIZE			3
#define ADIN1110_WR_HDR_SIZE			2
#define ADIN1110_PHY_ID_REG			1

#define ADIN1110_PHY_ID				0x0283BC91
#define ADIN2111_PHY_ID				0x0283BCA1

#define ADIN1110_PORTS				1
#define ADIN2111_PORTS				2

#define ADIN1110_CD_MASK			NO_OS_BIT(15)
#define ADIN1110_RW_MASK			NO_OS_BIT(13)

#define ADIN1110_SOFT_RST_REG			0x3C
#define ADIN1110_RESET_REG			0x03
#define ADIN1110_SWRESET			NO_OS_BIT(0)
#define ADIN1110_SWRESET_KEY1			0x4F1C
#define ADIN1110_SWRESET_KEY2			0xC1F4
#define ADIN1110_SWRELEASE_KEY1			0x6F1A
#define ADIN1110_SWRELEASE_KEY2			0xA1F6

#define ADIN1110_SPI_CD				NO_OS_BIT(7)
#define ADIN1110_SPI_RW             		NO_OS_BIT(5)

#define ADIN1110_CONFIG1_REG			0x04
#define ADIN1110_CONFIG1_SYNC			NO_OS_BIT(15)

#define ADIN1110_CONFIG2_REG			0x06
#define ADIN2111_P2_FWD_UNK2HOST_MASK		NO_OS_BIT(12)
#define ADIN2111_PORT_CUT_THRU_EN		NO_OS_BIT(11)
#define ADIN1110_CRC_APPEND			NO_OS_BIT(5)
#define ADIN1110_FWD_UNK2HOST_MASK		NO_OS_BIT(2)

#define ADIN1110_STATUS0_REG			0x08
#define ADIN1110_STATUS0_TXPE_MASK		NO_OS_BIT(0)
#define ADIN1110_RESETC_MASK			NO_OS_BIT(6)

#define ADIN1110_STATUS1_REG			0x09
#define ADIN1110_LINK_STATE_MASK		NO_OS_BIT(0)
#define ADIN2111_P2_RX_RDY			NO_OS_BIT(17)
#define ADIN1110_SPI_ERR			NO_OS_BIT(10)
#define ADIN1110_RX_RDY				NO_OS_BIT(4)

#define ADIN1110_IMASK1_REG			0x0D
#define ADIN2111_RX_RDY_IRQ			NO_OS_BIT(17)
#define ADIN1110_SPI_ERR_IRQ			NO_OS_BIT(10)
#define ADIN1110_RX_RDY_IRQ			NO_OS_BIT(4)
#define ADIN1110_TX_RDY_IRQ			NO_OS_BIT(3)

#define ADIN1110_MDIOACC(x)			(0x20 + (x))
#define ADIN1110_MDIO_TRDONE			NO_OS_BIT(31)
#define ADIN1110_MDIO_TAERR			NO_OS_BIT(30)
#define ADIN1110_MDIO_ST			NO_OS_GENMASK(29, 28)
#define ADIN1110_MDIO_OP			NO_OS_GENMASK(27, 26)
#define ADIN1110_MDIO_PRTAD			NO_OS_GENMASK(25, 21)
#define ADIN1110_MDIO_DEVAD			NO_OS_GENMASK(20, 16)
#define ADIN1110_MDIO_DATA			NO_OS_GENMASK(15, 0)

#define ADIN1110_MMD_ACR_DEVAD_MASK		NO_OS_GENMASK(4, 0)
#define ADIN1110_MMD_ACR_FUNCTION_MASK		NO_OS_GENMASK(15, 14)
#define ADIN1110_MMD_ACCESS_MASK		NO_OS_GENMASK(15, 0)
#define ADIN1110_MMD_ACCESS_CTRL_REG		0x0D
#define ADIN1110_MMD_ACCESS_REG			0x0E

#define ADIN1110_MI_SFT_PD_MASK			NO_OS_BIT(11)
#define ADIN1110_MDIO_PHY_ID(x)			((x) + 1)
#define ADIN1110_MI_CONTROL_REG			0x0

#define ADIN1110_TX_FSIZE_REG			0x30
#define ADIN1110_TX_REG				0x31
#define ADIN1110_TX_SPACE_REG			0x32

#define ADIN1110_FIFO_CLR_REG			0x36
#define ADIN1110_FIFO_CLR_RX_MASK		NO_OS_BIT(0)
#define ADIN1110_FIFO_CLR_TX_MASK		NO_OS_BIT(1)

#define ADIN1110_MAC_RST_STATUS_REG		0x3B

#define ADIN2111_MAC_ADDR_APPLY2PORT2		NO_OS_BIT(31)
#define ADIN1110_MAC_ADDR_APPLY2PORT		NO_OS_BIT(30)
#define ADIN2111_MAC_ADDR_TO_OTHER_PORT		NO_OS_BIT(17)
#define ADIN1110_MAC_ADDR_TO_HOST		NO_OS_BIT(16)

#define ADIN1110_MAC_ADDR_FILT_UPR_REG(x)	(0x50 + 2 * (x))
#define ADIN1110_MAC_ADDR_FILT_LWR_REG(x)	(0x51 + 2 * (x))

#define ADIN1110_MAC_ADDR_UPR_MASK		NO_OS_GENMASK(15, 0)
#define ADIN1110_MAC_ADDR_LWR_MASK		NO_OS_GENMASK(31, 0)

#define ADIN1110_MAC_ADDR_MASK_UPR_REG		0x70
#define ADIN1110_MAC_ADDR_MASK_LWR_REG		0x71

#define ADIN1110_RX_FRM_CNT_REG			0xA0
#define ADIN1110_RX_CRC_ERR_CNT_REG		0xA4
#define ADIN1110_RX_ALGN_ERR_CNT_REG		0xA5
#define ADIN1110_RX_LS_ERR_CNT_REG		0xA6
#define ADIN1110_RX_PHY_ERR_CNT_REG		0xA7
#define ADIN1110_TX_FRM_CNT_REG			0xA8
#define ADIN1110_TX_BCAST_CNT_REG		0xA9
#define ADIN1110_TX_MCAST_CNT_REG		0xAA
#define ADIN1110_TX_UCAST_CNT_REG		0xAB
#define ADIN1110_RX_BCAST_CNT_REG		0xA1
#define ADIN1110_RX_MCAST_CNT_REG		0xA2
#define ADIN1110_RX_UCAST_CNT_REG		0xA3

#define ADIN1110_RX_DROP_FULL_CNT_REG		0xAC
#define ADIN1110_RX_DROP_FILT_CNT_REG		0xAD

#define ADIN1110_RX_FSIZE_REG			0x90
#define ADIN1110_RX_REG				0x91

#define ADIN2111_RX_P2_FSIZE_REG		0xC0
#define ADIN2111_RX_P2_REG			0xC1

#define ADIN1110_CLEAR_STATUS0			0xFFF

/* MDIO_OP codes */
#define ADIN1110_MDIO_OP_ADDR			0x0
#define ADIN1110_MDIO_OP_WR			0x1
#define ADIN1110_MDIO_OP_RD			0x3

#define ADIN1110_WR_HEADER_LEN			2
#define ADIN1110_FRAME_HEADER_LEN		2
#define ADIN1110_RD_HEADER_LEN			3
#define ADIN1110_REG_LEN			4
#define ADIN1110_CRC_LEN			1
#define ADIN1110_FEC_LEN			4

#define ADIN_MAC_MULTICAST_ADDR_SLOT		0
#define ADIN_MAC_BROADCAST_ADDR_SLOT		1
#define ADIN_MAC_P1_ADDR_SLOT			2
#define ADIN_MAC_P2_ADDR_SLOT			3
#define ADIN_MAC_FDB_ADDR_SLOT			4

/**
 * @brief The chips supported by this driver.
 */
enum adin1110_chip_id {
	ADIN1110,
	ADIN2111,
};

/**
 * @brief ADIN1110 device descriptor.
 */
struct adin1110_desc {
	enum adin1110_chip_id chip_type;
	struct no_os_spi_desc *comm_desc;
	uint8_t mac_address[ADIN1110_ETH_ALEN];
	uint8_t data[ADIN1110_BUFF_LEN];
	struct no_os_gpio_desc *reset_gpio;
	bool append_crc;
};

/**
 * @brief Initialization parameter for the device descriptor.
 */
struct adin1110_init_param {
	enum adin1110_chip_id chip_type;
	struct no_os_spi_init_param comm_param;
	struct no_os_gpio_init_param reset_param;
	uint8_t mac_address[ADIN1110_ETH_ALEN];
	bool append_crc;
};

/**
 * @brief Buffer structure used for frame RX and TX transactions.
 */
struct adin1110_eth_buff {
	uint32_t len;
	uint8_t mac_dest[ADIN1110_ETH_ALEN];
	uint8_t mac_source[ADIN1110_ETH_ALEN];
	uint8_t ethertype[2];
	uint8_t *payload;
};

/* Reset both the MAC and PHY. */
int adin1110_sw_reset(struct adin1110_desc *);

/* Update a register's value based on a mask */
int adin1110_reg_update(struct adin1110_desc *, uint16_t, uint32_t, uint32_t);

/* Write a register's value */
int adin1110_reg_write(struct adin1110_desc *, uint16_t, uint32_t);

/* Read a register's value */
int adin1110_reg_read(struct adin1110_desc *, uint16_t, uint32_t *);

/* Write a frame to the TX FIFO */
int adin1110_write_fifo(struct adin1110_desc *, uint32_t,
			struct adin1110_eth_buff *);

/* Read a frame from the RX FIFO */
int adin1110_read_fifo(struct adin1110_desc *, uint32_t,
		       struct adin1110_eth_buff *);

/* Write a PHY register using clause 22 */
int adin1110_mdio_write(struct adin1110_desc *, uint32_t, uint32_t, uint16_t);

/* Read a PHY register using clause 22 */
int adin1110_mdio_read(struct adin1110_desc *, uint32_t, uint32_t, uint16_t *);

/* Write a PHY register using clause 45 */
int adin1110_mdio_write_c45(struct adin1110_desc *, uint32_t, uint32_t,
			    uint32_t,
			    uint16_t);

/* Read a PHY register using clause 45 */
int adin1110_mdio_read_c45(struct adin1110_desc *, uint32_t, uint32_t, uint16_t,
			   uint16_t *);

/* Get the link state for a given port */
int adin1110_link_state(struct adin1110_desc *, uint32_t *);

/* Set a port in promiscuous mode. All MAC filters are dropped */
int adin1110_set_promisc(struct adin1110_desc *, uint32_t, bool);

/*
 * Set a MAC filter. The frames with destination MAC addresses matching this will be
 * forwarded to the host.
 */
int adin1110_set_mac_addr(struct adin1110_desc *desc,
			  uint8_t mac_address[ADIN1110_ETH_ALEN]);

/* Enable/disable the forwarding (to host) of broadcast frames */
int adin1110_broadcast_filter(struct adin1110_desc *, bool);

/* Reset the MAC device */
int adin1110_mac_reset(struct adin1110_desc *);

/* Reset the PHY device */
int adin1110_phy_reset(struct adin1110_desc *);

/* Initialize the device */
int adin1110_init(struct adin1110_desc **, struct adin1110_init_param *);

/* Free a device descriptor */
int adin1110_remove(struct adin1110_desc *);

#endif