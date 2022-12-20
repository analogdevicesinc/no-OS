#ifndef _ADIN1110_H
#define _ADIN1110_H

#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

#define ADIN1110_FCS_LEN			4
#define ADIN1110_MAC_LEN			6

#define ADIN1110_CTRL_FRAME_SIZE		7
#define ADIN1110_PHY_ID_REG			0x1

#define ADIN1110_PHY_ID				0x0283BC91

#define ADIN1110_CD_MASK			NO_OS_BIT(15)
#define ADIN1110_RW_MASK			NO_OS_BIT(13)

#define ADIN1110_SOFT_RST_REG			0x3C
#define ADIN1110_RESET				0x03
#define ADIN1110_SWRESET			NO_OS_BIT(0)
#define ADIN1110_SWRESET_KEY1			0x4F1C
#define ADIN1110_SWRESET_KEY2			0xC1F4
#define ADIN1110_SWRELEASE_KEY1			0x6F1A
#define ADIN1110_SWRELEASE_KEY2			0xA1F6

#define ADIN1110_SPI_CD				NO_OS_BIT(7)
#define ADIN1110_SPI_RW             NO_OS_BIT(5)

#define ADIN1110_CONFIG1			0x04
#define   ADIN1110_CONFIG1_SYNC			NO_OS_BIT(15)

#define ADIN1110_CONFIG2			0x06
#define   ADIN2111_P2_FWD_UNK2HOST		NO_OS_BIT(12)
#define   ADIN2111_PORT_CUT_THRU_EN		NO_OS_BIT(11)
#define   ADIN1110_CRC_APPEND			NO_OS_BIT(5)
#define   ADIN1110_FWD_UNK2HOST			NO_OS_BIT(2)

#define ADIN1110_STATUS0			0x08
#define ADIN1110_RESETC_MASK			NO_OS_BIT(6)

#define ADIN1110_STATUS1			0x09
#define   ADIN2111_P2_RX_RDY			NO_OS_BIT(17)
#define   ADIN1110_SPI_ERR			NO_OS_BIT(10)
#define   ADIN1110_RX_RDY			NO_OS_BIT(4)

#define ADIN1110_IMASK1				0x0D
#define   ADIN2111_RX_RDY_IRQ			NO_OS_BIT(17)
#define   ADIN1110_SPI_ERR_IRQ			NO_OS_BIT(10)
#define   ADIN1110_RX_RDY_IRQ			NO_OS_BIT(4)
#define   ADIN1110_TX_RDY_IRQ			NO_OS_BIT(3)

#define ADIN1110_MDIOACC			0x20
#define   ADIN1110_MDIO_TRDONE			NO_OS_BIT(31)
#define   ADIN1110_MDIO_ST			NO_OS_GENMASK(29, 28)
#define   ADIN1110_MDIO_OP			NO_OS_GENMASK(27, 26)
#define   ADIN1110_MDIO_PRTAD			NO_OS_GENMASK(25, 21)
#define   ADIN1110_MDIO_DEVAD			NO_OS_GENMASK(20, 16)
#define   ADIN1110_MDIO_DATA			NO_OS_GENMASK(15, 0)

#define ADIN1110_TX_FSIZE			0x30
#define ADIN1110_TX				0x31
#define ADIN1110_TX_SPACE			0x32

#define ADIN1110_MAC_RST_STATUS_REG		0x3B

#define   ADIN2111_MAC_ADDR_APPLY2PORT2		NO_OS_BIT(31)
#define   ADIN1110_MAC_ADDR_APPLY2PORT		NO_OS_BIT(30)
#define   ADIN2111_MAC_ADDR_TO_OTHER_PORT	NO_OS_BIT(17)
#define   ADIN1110_MAC_ADDR_TO_HOST		NO_OS_BIT(16)

#define ADIN1110_MAC_ADDR_FILTER_UPR		0x50
#define ADIN1110_MAC_ADDR_FILTER_LWR		0x51

#define ADIN1110_MAC_ADDR_MASK_UPR		0x70
#define ADIN1110_MAC_ADDR_MASK_LWR		0x71

#define ADIN1110_RX_FRM_CNT		0xA0
#define ADIN1110_RX_CRC_ERR_CNT		0xA4
#define ADIN1110_RX_ALGN_ERR_CNT	0xA5
#define ADIN1110_RX_LS_ERR_CNT		0xA6
#define ADIN1110_RX_PHY_ERR_CNT		0xA7

#define ADIN1110_RX_DROP_FULL_CNT   0xAC
#define ADIN1110_RX_DROP_FILT_CNT   0xAD

#define ADIN1110_RX_FSIZE			0x90
#define ADIN1110_RX				0x91

#define ADIN2111_RX_P2_FSIZE			0xC0
#define ADIN2111_RX_P2				0xC1

#define ADIN1110_CLEAR_STATUS0			0xFFF

/* MDIO_OP codes */
#define ADIN1110_MDIO_OP_WR			0x1
#define ADIN1110_MDIO_OP_RD			0x3

#define ADIN1110_MAX_BUFF			2048
#define ADIN1110_MAX_FRAMES_READ		64
#define ADIN1110_WR_HEADER_LEN			2
#define ADIN1110_FRAME_HEADER_LEN		2
#define ADIN1110_INTERNAL_SIZE_HEADER_LEN	2
#define ADIN1110_RD_HEADER_LEN			3
#define ADIN1110_REG_LEN			4
#define ADIN1110_FEC_LEN			4

#define ADIN_MAC_MAX_PORTS			2
#define ADIN_MAC_MAX_ADDR_SLOTS			16

#define ADIN_MAC_MULTICAST_ADDR_SLOT		0
#define ADIN_MAC_BROADCAST_ADDR_SLOT		1
#define ADIN_MAC_P1_ADDR_SLOT			2
#define ADIN_MAC_P2_ADDR_SLOT			3
#define ADIN_MAC_FDB_ADDR_SLOT			4

enum adin1110_ethertype {
	ADIN1110_IPV4 = 0x0800,
	ADIN1110_ARP = 0x0806,
	ADIN1110_IPV6 = 0x86DD,
};

struct adin1110_desc {
    struct no_os_spi_desc *comm_desc;
    uint8_t mac_address[6];
    uint8_t rx_buff[1500];
    uint8_t tx_buff[48];

    struct no_os_gpio_desc *reset_gpio;
    uint8_t data[ADIN1110_MAX_BUFF];
    bool spi_oa;
};

struct adin1110_init_param {
    struct no_os_spi_init_param comm_param;
    struct no_os_gpio_init_param reset_param;
    uint8_t mac_address[6];
    bool spi_oa;
};

struct adin1110_sk_buff {
	uint8_t mac_source[6];
	uint8_t mac_destination[6];
	enum adin1110_ethertype ethertype;
	uint32_t buff_len;
	uint8_t *payload;
};

int adin1110_write_fifo(struct adin1110_desc *, struct adin1110_sk_buff *);
int adin1110_read_fifo(struct adin1110_desc *, struct adin1110_sk_buff *);
int adin1110_init(struct adin1110_desc **, struct adin1110_init_param *);
int adin1110_remove(struct adin1110_desc *);

#endif