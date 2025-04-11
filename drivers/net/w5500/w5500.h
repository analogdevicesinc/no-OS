/***************************************************************************//**
*   @file   w5500.h
*   @brief  Header file W5500 Driver
*   @author Alisa-Dariana Roman (alisa.roman@analog.com)
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#ifndef __W5500_H__
#define __W5500_H__

#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

#define W5500_CHIP_VERSION   0x04

/* Byte manipulation helpers */
#define W5500_BYTE_HIGH(value)       (((value) >> 8) & 0xFF)
#define W5500_BYTE_LOW(value)        ((value) & 0xFF)

/* SPI Frame Structure Masks */
#define W5500_BSB_MASK               NO_OS_GENMASK(7, 3)   /* Block Select Bits */
#define W5500_RWB_MASK               NO_OS_BIT(2)          /* Read/Write Bit */
#define W5500_OM_MASK                NO_OS_GENMASK(1, 0)   /* Operation Mode Bits */

/* Control Byte Construction Helpers */
#define W5500_BSB(block)             no_os_field_prep(W5500_BSB_MASK, block)
#define W5500_RWB_READ               0x00
#define W5500_RWB_WRITE              NO_OS_BIT(2)
#define W5500_OM_VDM                 0x00    /* Variable Data Length Mode */
#define W5500_OM_FDM_1               0x01    /* Fixed Data Length Mode 1 byte */
#define W5500_OM_FDM_2               0x02    /* Fixed Data Length Mode 2 bytes */
#define W5500_OM_FDM_4               0x03    /* Fixed Data Length Mode 4 bytes */

/* Block Select Bit values */
#define W5500_COMMON_REG             0x00    /* Common Register Block */
#define W5500_SOCKET_REG_BLOCK(n)    (0x01 + (n) * 4)  /* Socket n Register Block */
#define W5500_SOCKET_TX_BUF_BLOCK(n) (0x02 + (n) * 4)  /* Socket n TX Buffer Block */
#define W5500_SOCKET_RX_BUF_BLOCK(n) (0x03 + (n) * 4)  /* Socket n RX Buffer Block */

/* Common Register Offsets */
#define W5500_MR             0x0000  /* Mode Register */
#define W5500_GAR            0x0001  /* Gateway Address Register (4 bytes) */
#define W5500_SUBR           0x0005  /* Subnet Mask Register (4 bytes) */
#define W5500_SHAR           0x0009  /* Source Hardware Address Register (6 bytes) */
#define W5500_SIPR           0x000F  /* Source IP Address Register (4 bytes) */
#define W5500_INTLEVEL       0x0013  /* Interrupt Low Level Timer Register (2 bytes) */
#define W5500_IR             0x0015  /* Interrupt Register */
#define W5500_IMR            0x0016  /* Interrupt Mask Register */
#define W5500_SIR            0x0017  /* Socket Interrupt Register */
#define W5500_SIMR           0x0018  /* Socket Interrupt Mask Register */
#define W5500_RTR            0x0019  /* Retry Time Register (2 bytes) */
#define W5500_RCR            0x001B  /* Retry Count Register */
#define W5500_PTIMER         0x001C  /* PPP LCP Request Timer Register */
#define W5500_PMAGIC         0x001D  /* PPP LCP Magic Number Register */
#define W5500_PHAR           0x001E  /* PPP Destination MAC Register (6 bytes) */
#define W5500_PSID           0x0024  /* PPP Session ID Register (2 bytes) */
#define W5500_PMRU           0x0026  /* PPP Maximum Segment Size (2 bytes) */
#define W5500_UIPR           0x0028  /* Unreachable IP Address Register (4 bytes) */
#define W5500_UPORTR         0x002C  /* Unreachable Port Register (2 bytes) */
#define W5500_PHYCFGR        0x002E  /* PHY Configuration Register */
#define W5500_VERSIONR       0x0039  /* Chip Version Register */

/* Mode Register (MR) bit values */
#define W5500_MR_RST         NO_OS_BIT(7)    /* Reset */
#define W5500_MR_WOL         NO_OS_BIT(5)    /* Wake on LAN */
#define W5500_MR_PB          NO_OS_BIT(4)    /* Ping Block */
#define W5500_MR_PPPOE       NO_OS_BIT(3)    /* PPPoE Mode */
#define W5500_MR_FARP        NO_OS_BIT(1)    /* Force ARP */

/* Socket Register Offsets */
#define W5500_Sn_MR          0x0000  /* Socket n Mode Register */
#define W5500_Sn_CR          0x0001  /* Socket n Command Register */
#define W5500_Sn_IR          0x0002  /* Socket n Interrupt Register */
#define W5500_Sn_SR          0x0003  /* Socket n Status Register */
#define W5500_Sn_PORT        0x0004  /* Socket n Source Port Register (2 bytes) */
#define W5500_Sn_DHAR        0x0006  /* Socket n Destination Hardware Address Register (6 bytes) */
#define W5500_Sn_DIPR        0x000C  /* Socket n Destination IP Address Register (4 bytes) */
#define W5500_Sn_DPORT       0x0010  /* Socket n Destination Port Register (2 bytes) */
#define W5500_Sn_MSSR        0x0012  /* Socket n Maximum Segment Size Register (2 bytes) */
#define W5500_Sn_TOS         0x0015  /* Socket n Type of Service Register */
#define W5500_Sn_TTL         0x0016  /* Socket n Time to Live Register */
#define W5500_Sn_RXBUF_SIZE  0x001E  /* Socket n RX Buffer Size Register */
#define W5500_Sn_TXBUF_SIZE  0x001F  /* Socket n TX Buffer Size Register */
#define W5500_Sn_TX_FSR      0x0020  /* Socket n TX Free Size Register (2 bytes) */
#define W5500_Sn_TX_RD       0x0022  /* Socket n TX Read Pointer Register (2 bytes) */
#define W5500_Sn_TX_WR       0x0024  /* Socket n TX Write Pointer Register (2 bytes) */
#define W5500_Sn_RX_RSR      0x0026  /* Socket n RX Received Size Register (2 bytes) */
#define W5500_Sn_RX_RD       0x0028  /* Socket n RX Read Pointer Register (2 bytes) */
#define W5500_Sn_RX_WR       0x002A  /* Socket n RX Write Pointer Register (2 bytes) */
#define W5500_Sn_IMR         0x002C  /* Socket n Interrupt Mask Register */
#define W5500_Sn_FRAG        0x002D  /* Socket n Fragment Offset in IP Header (2 bytes) */
#define W5500_Sn_KPALVTR     0x002F  /* Socket n Keep Alive Timer Register */

/* Socket Mode Register (Sn_MR) values */
#define W5500_Sn_MR_CLOSE    0x00    /* Closed */
#define W5500_Sn_MR_TCP      0x01    /* TCP */
#define W5500_Sn_MR_UDP      0x02    /* UDP */
#define W5500_Sn_MR_MACRAW   0x04    /* MAC Raw mode */
#define W5500_Sn_MR_NDMC     NO_OS_BIT(5)    /* No Delayed ACK */
#define W5500_Sn_MR_MULTI    NO_OS_BIT(7)    /* Multicasting */
#define W5500_Sn_MR_BCASTB   NO_OS_BIT(6)    /* Broadcast Blocking */
#define W5500_Sn_MR_UCASTB   NO_OS_BIT(4)    /* Unicast Blocking (UDP mode) */
#define W5500_Sn_MR_MIP6B    NO_OS_BIT(4)    /* IPv6 Blocking (MACRAW mode) */

/* Socket Command Register (Sn_CR) values */
#define W5500_Sn_CR_OPEN             0x01    /* Initialize and open socket */
#define W5500_Sn_CR_LISTEN           0x02    /* Wait connection request in TCP mode (Server mode) */
#define W5500_Sn_CR_CONNECT          0x04    /* Send connection request in TCP mode (Client mode) */
#define W5500_Sn_CR_DISCON           0x08    /* Send closing request in TCP mode */
#define W5500_Sn_CR_CLOSE            0x10    /* Close socket */
#define W5500_Sn_CR_SEND             0x20    /* Send data */
#define W5500_Sn_CR_SEND_MAC         0x21    /* Send data with MAC address (UDP mode) */
#define W5500_Sn_CR_SEND_KEEP        0x22    /* Send keep-alive packet (TCP mode) */
#define W5500_Sn_CR_RECV             0x40    /* Receive data */

/* Socket Status Register (Sn_SR) values */
#define W5500_Sn_SR_CLOSED           0x00    /* Socket is closed */
#define W5500_Sn_SR_INIT             0x13    /* Socket is initialized */
#define W5500_Sn_SR_LISTEN           0x14    /* Socket is in listen state */
#define W5500_Sn_SR_ESTABLISHED      0x17    /* Connection is established */
#define W5500_Sn_SR_CLOSE_WAIT       0x1C    /* Closing state */
#define W5500_Sn_SR_UDP              0x22    /* Socket is in UDP mode */
#define W5500_Sn_SR_MACRAW           0x42    /* Socket is in MACRAW mode */
#define W5500_Sn_SR_SYNSENT          0x15    /* SYN packet sent (TCP client) */
#define W5500_Sn_SR_SYNRECV          0x16    /* SYN packet received (TCP server) */
#define W5500_Sn_SR_FIN_WAIT         0x18    /* Socket closing (FIN sent) */
#define W5500_Sn_SR_CLOSING          0x1A    /* Socket closing (FIN exchanged) */
#define W5500_Sn_SR_TIME_WAIT        0x1B    /* Socket closing (2MSL timer active) */
#define W5500_Sn_SR_LAST_ACK         0x1D    /* Socket closing (Last ACK) */

/* Socket Interrupt Register (Sn_IR) values */
#define W5500_Sn_IR_SEND_OK          NO_OS_BIT(4)    /* Send operation completed */
#define W5500_Sn_IR_TIMEOUT          NO_OS_BIT(3)    /* Timeout occurred */
#define W5500_Sn_IR_RECV             NO_OS_BIT(2)    /* Data received */
#define W5500_Sn_IR_DISCON           NO_OS_BIT(1)    /* Connection termination requested/completed */
#define W5500_Sn_IR_CON              NO_OS_BIT(0)    /* Connection established */

/* Interrupt Register (IR) values */
#define W5500_IR_CONFLICT            NO_OS_BIT(7)    /* IP address conflict */
#define W5500_IR_UNREACH             NO_OS_BIT(6)    /* Destination unreachable */
#define W5500_IR_PPPOE               NO_OS_BIT(5)    /* PPPoE connection closed */
#define W5500_IR_MP                  NO_OS_BIT(4)    /* Magic packet received */

/* PHY Configuration Register (PHYCFGR) values */
#define W5500_PHYCFGR_RST            NO_OS_BIT(7)    /* PHY Reset */
#define W5500_PHYCFGR_OPMD           NO_OS_BIT(6)    /* Operation Mode Select */
#define W5500_PHYCFGR_OPMDC          NO_OS_GENMASK(5, 3)    /* Operation Mode Configuration */
#define W5500_PHYCFGR_DPX            NO_OS_BIT(2)    /* Duplex Status (1=Full, 0=Half) */
#define W5500_PHYCFGR_SPD            NO_OS_BIT(1)    /* Speed Status (1=100Mbps, 0=10Mbps) */
#define W5500_PHYCFGR_LNK            NO_OS_BIT(0)    /* Link Status (1=Up, 0=Down) */

/* PHYCFGR Operation Mode Configuration values */
#define W5500_PHYCFGR_OPMDC_10BT_HD      no_os_field_prep(W5500_PHYCFGR_OPMDC, 0)
/* 10BT Half-duplex, no auto-nego */
#define W5500_PHYCFGR_OPMDC_10BT_FD      no_os_field_prep(W5500_PHYCFGR_OPMDC, 1)
/* 10BT Full-duplex, no auto-nego */
#define W5500_PHYCFGR_OPMDC_100BT_HD     no_os_field_prep(W5500_PHYCFGR_OPMDC, 2)
/* 100BT Half-duplex, no auto-nego */
#define W5500_PHYCFGR_OPMDC_100BT_FD     no_os_field_prep(W5500_PHYCFGR_OPMDC, 3)
/* 100BT Full-duplex, no auto-nego */
#define W5500_PHYCFGR_OPMDC_100BT_HD_AN  no_os_field_prep(W5500_PHYCFGR_OPMDC, 4)
/* 100BT Half-duplex, with auto-nego */
#define W5500_PHYCFGR_OPMDC_POWER_DOWN   no_os_field_prep(W5500_PHYCFGR_OPMDC, 6)
/* Power down mode */
#define W5500_PHYCFGR_OPMDC_ALL_AN       no_os_field_prep(W5500_PHYCFGR_OPMDC, 7)
/* All capable, with auto-nego */

/* Socket Buffer Size values (2^n KB) */
#define W5500_SOCK_BUF_SIZE_0K       0x00    /* 0KB buffer size */
#define W5500_SOCK_BUF_SIZE_1K       0x01    /* 1KB buffer size */
#define W5500_SOCK_BUF_SIZE_2K       0x02    /* 2KB buffer size (default) */
#define W5500_SOCK_BUF_SIZE_4K       0x04    /* 4KB buffer size */
#define W5500_SOCK_BUF_SIZE_8K       0x08    /* 8KB buffer size */
#define W5500_SOCK_BUF_SIZE_16K      0x10    /* 16KB buffer size */

/* Maximum socket number */
#define W5500_MAX_SOCK_NUMBER        7       /* 8 sockets (0-7) */

struct w5500_socket_address {
	uint8_t ip[4];
	uint8_t port[2];
};

struct w5500_socket {
	uint8_t protocol;
	uint16_t mss;
	uint8_t tx_buf_size;
	uint8_t rx_buf_size;
};

struct w5500_dev {
	struct no_os_gpio_desc *gpio_reset;
	struct no_os_gpio_desc *gpio_int;
	struct no_os_spi_desc *spi;
	uint8_t mac_addr[6];
	uint16_t retry_time;
	uint8_t retry_count;
	struct w5500_socket sockets[W5500_MAX_SOCK_NUMBER + 1];
};

struct w5500_init_param {
	const struct no_os_gpio_init_param *gpio_reset;
	const struct no_os_gpio_init_param *gpio_int;
	const struct no_os_spi_init_param *spi_init;
	uint8_t mac_addr[6];
	uint16_t retry_time;
	uint8_t retry_count;
};

/** Write data to a W5500 register */
int w5500_reg_write(struct w5500_dev *dev, uint8_t block,
		    uint16_t addr, const uint8_t *data, uint16_t len);

/** Read data from a W5500 register */
int w5500_reg_read(struct w5500_dev *dev, uint8_t block,
		   uint16_t addr, uint8_t *data, uint16_t len);

/** Read a 16-bit register reliably by reading until consecutive values match */
int w5500_read_16bit_reg(struct w5500_dev *dev, uint8_t block,
			 uint16_t addr, uint16_t *value);

/** Set MAC address */
int w5500_set_mac(struct w5500_dev *dev, const uint8_t mac[6]);

/** Set IP address */
int w5500_set_ip(struct w5500_dev *dev, const uint8_t ip[4]);

/** Set subnet mask */
int w5500_set_subnet(struct w5500_dev *dev, const uint8_t subnet[4]);

/** Set gateway address */
int w5500_set_gateway(struct w5500_dev *dev, const uint8_t gateway[4]);

/** Get MAC address */
int w5500_get_mac(struct w5500_dev *dev, uint8_t mac[6]);

/** Get IP address */
int w5500_get_ip(struct w5500_dev *dev, uint8_t ip[4]);

/** Get subnet mask */
int w5500_get_subnet(struct w5500_dev *dev, uint8_t subnet[4]);

/** Get gateway address */
int w5500_get_gateway(struct w5500_dev *dev, uint8_t gateway[4]);

/** Check the Ethernet link status */
int w5500_check_link_status(struct w5500_dev *dev);

/** Configure W5500 TCP parameters (RTR and RCR) */
int w5500_tcp_config(struct w5500_dev *dev, uint16_t retry_time,
		     uint8_t retry_count);

/** Read status of a socket */
int w5500_socket_read_status(struct w5500_dev *dev, uint8_t sock_id,
			     uint8_t *status);

/** Write a command to a socket's command register */
int w5500_socket_command_write(struct w5500_dev *dev, uint8_t sock_id,
			       uint8_t val);

/** Clear specific socket interrupt flags */
int w5500_socket_clear_interrupt(struct w5500_dev *dev, uint8_t sock_id,
				 uint8_t flags);

/** Initialize a socket */
int w5500_socket_init(struct w5500_dev *dev, uint8_t sock_id);

/** Open a socket with specified protocol */
int w5500_socket_open(struct w5500_dev *dev, uint8_t sock_id, uint8_t protocol,
		      uint8_t buff_size);

/** Close a socket */
int w5500_socket_close(struct w5500_dev *dev, uint8_t sock_id);

/** Connect to a remote host (TCP client mode) */
int w5500_socket_connect(struct w5500_dev *dev, uint8_t sock_id,
			 struct w5500_socket_address *addr);

/** Disconnect a TCP connection */
int w5500_socket_disconnect(struct w5500_dev *dev, uint8_t sock_id);

/** Send data through a socket */
int w5500_socket_send(struct w5500_dev *dev, uint8_t sock_id, const void *buf,
		      uint16_t len);

/** Receive data from a socket */
int w5500_socket_recv(struct w5500_dev *dev, uint8_t sock_id, void *buf,
		      uint16_t len);

/** Send data to a specific destination (UDP) */
int w5500_socket_sendto(struct w5500_dev *dev, uint8_t sock_id, const void *buf,
			uint16_t len, struct w5500_socket_address *to);

/** Receive data from a socket with source address information (UDP) */
int w5500_socket_recvfrom(struct w5500_dev *dev, uint8_t sock_id,
			  void *buf, uint16_t len,
			  struct w5500_socket_address *from);

/** Bind a socket to a specific port (server mode) */
int w5500_socket_bind(struct w5500_dev *dev, uint8_t sock_id, uint16_t port);

/** Set socket to listen mode (TCP server) */
int w5500_socket_listen(struct w5500_dev *dev, uint8_t sock_id);

/** Reset the W5500 chip using hardware or software method */
int w5500_reset(struct w5500_dev *dev);

/** Setup the W5500 chip with basic configuration */
int w5500_setup(struct w5500_dev *dev);

/** Initialize the device */
int w5500_init(struct w5500_dev **device,
	       struct w5500_init_param *init_param);

/** Free a device descriptor and release resources */
int w5500_remove(struct w5500_dev *dev);

#endif
