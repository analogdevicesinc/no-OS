/***************************************************************************//**
*   @file   wiz850io.h
*   @brief  Header file WIZ Driver
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

#ifndef __WIZ850IO_H__
#define __WIZ850IO_H__

#include "no_os_alloc.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/* W5500 Block Select Bits */
#define WIZ850IO_COMMON_REG 0x00
#define WIZ850IO_SOCKET0_REG 0x01
#define WIZ850IO_SOCKET0_TX_BUF 0x02
#define WIZ850IO_SOCKET0_RX_BUF 0x03
#define WIZ850IO_SOCKET1_REG 0x05
#define WIZ850IO_SOCKET1_TX_BUF 0x06
#define WIZ850IO_SOCKET1_RX_BUF 0x07
#define WIZ850IO_SOCKET2_REG 0x09
#define WIZ850IO_SOCKET2_TX_BUF 0x0A
#define WIZ850IO_SOCKET2_RX_BUF 0x0B
#define WIZ850IO_SOCKET3_REG 0x0D
#define WIZ850IO_SOCKET3_TX_BUF 0x0E
#define WIZ850IO_SOCKET3_RX_BUF 0x0F
#define WIZ850IO_SOCKET4_REG 0x11
#define WIZ850IO_SOCKET4_TX_BUF 0x12
#define WIZ850IO_SOCKET4_RX_BUF 0x13
#define WIZ850IO_SOCKET5_REG 0x15
#define WIZ850IO_SOCKET5_TX_BUF 0x16
#define WIZ850IO_SOCKET5_RX_BUF 0x17
#define WIZ850IO_SOCKET6_REG 0x19
#define WIZ850IO_SOCKET6_TX_BUF 0x1A
#define WIZ850IO_SOCKET6_RX_BUF 0x1B
#define WIZ850IO_SOCKET7_REG 0x1D
#define WIZ850IO_SOCKET7_TX_BUF 0x1E
#define WIZ850IO_SOCKET7_RX_BUF 0x1F

/* Common Register Addresses */
#define WIZ850IO_MR 0x0000       /* Mode Register */
#define WIZ850IO_GAR 0x0001      /* Gateway Address Register (4 bytes) */
#define WIZ850IO_SUBR 0x0005     /* Subnet Mask Register (4 bytes) */
#define WIZ850IO_SHAR 0x0009     /* Source Hardware Address Register (6 bytes) */
#define WIZ850IO_SIPR 0x000F     /* Source IP Address Register (4 bytes) */
#define WIZ850IO_INTLEVEL 0x0013 /* Interrupt Low Level Timer Register (2 bytes) */
#define WIZ850IO_IR 0x0015       /* Interrupt Register */
#define WIZ850IO_IMR 0x0016      /* Interrupt Mask Register */
#define WIZ850IO_SIR 0x0017      /* Socket Interrupt Register */
#define WIZ850IO_SIMR 0x0018     /* Socket Interrupt Mask Register */
#define WIZ850IO_RTR 0x0019      /* Retry Time Register (2 bytes) */
#define WIZ850IO_RCR 0x001B      /* Retry Count Register */
#define WIZ850IO_PTIMER 0x001C   /* PPP LCP Request Timer Register */
#define WIZ850IO_PMAGIC 0x001D   /* PPP LCP Magic Number Register */
#define WIZ850IO_PHAR 0x001E     /* PPP Destination MAC Register (6 bytes) */
#define WIZ850IO_PSID 0x0024     /* PPP Session ID Register (2 bytes) */
#define WIZ850IO_PMRU 0x0026     /* PPP Maximum Segment Size (2 bytes) */
#define WIZ850IO_UIPR 0x0028     /* Unreachable IP Address Register (4 bytes) */
#define WIZ850IO_UPORTR 0x002C   /* Unreachable Port Register (2 bytes) */
#define WIZ850IO_PHYCFGR 0x002E  /* PHY Configuration Register */
#define WIZ850IO_VERSIONR 0x0039 /* Chip Version Register */

/* Socket Register Addresses - Base address constants */
#define WIZ850IO_SOCKET_BASE(n)      (n * 0x40)
#define WIZ850IO_Sn_MR(n)            (0x0000 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_CR(n)            (0x0001 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_IR(n)            (0x0002 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_SR(n)            (0x0003 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_PORT(n)          (0x0004 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_DHAR(n)          (0x0006 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_DIPR(n)          (0x000C + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_DPORT(n)         (0x0010 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_MSSR(n)          (0x0012 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_TOS(n)           (0x0015 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_TTL(n)           (0x0016 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_RXBUF_SIZE(n)    (0x001E + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_TXBUF_SIZE(n)    (0x001F + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_TX_FSR(n)        (0x0020 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_TX_RD(n)         (0x0022 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_TX_WR(n)         (0x0024 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_RX_RSR(n)        (0x0026 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_RX_RD(n)         (0x0028 + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_RX_WR(n)         (0x002A + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_IMR(n)           (0x002C + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_FRAG(n)          (0x002D + WIZ850IO_SOCKET_BASE(n))
#define WIZ850IO_Sn_KPALVTR(n)       (0x002F + WIZ850IO_SOCKET_BASE(n))

/* Mode Register (MR) values */
#define WIZ850IO_MR_RST       0x80 /* Reset */
#define WIZ850IO_MR_WOL       0x20 /* Wake on LAN */
#define WIZ850IO_MR_PB        0x10 /* Ping Block */
#define WIZ850IO_MR_PPPOE     0x08 /* PPPoE Mode */
#define WIZ850IO_MR_FARP      0x02 /* Force ARP */

/* Socket Mode Register (Sn_MR) values */
#define WIZ850IO_Sn_MR_CLOSE  0x00 /* Closed */
#define WIZ850IO_Sn_MR_TCP    0x01 /* TCP */
#define WIZ850IO_Sn_MR_UDP    0x02 /* UDP */
#define WIZ850IO_Sn_MR_MACRAW 0x04 /* MAC Raw mode */
#define WIZ850IO_Sn_MR_NDMC   0x20 /* No Delayed ACK */
#define WIZ850IO_Sn_MR_MULTI  0x80 /* Multicasting */

/* Socket Command Register (Sn_CR) values */
#define WIZ850IO_Sn_CR_OPEN      0x01 /* OPEN command */
#define WIZ850IO_Sn_CR_LISTEN    0x02 /* LISTEN command */
#define WIZ850IO_Sn_CR_CONNECT   0x04 /* CONNECT command */
#define WIZ850IO_Sn_CR_DISCON    0x08 /* DISCONNECT command */
#define WIZ850IO_Sn_CR_CLOSE     0x10 /* CLOSE command */
#define WIZ850IO_Sn_CR_SEND      0x20 /* SEND command */
#define WIZ850IO_Sn_CR_SEND_MAC  0x21 /* SEND_MAC command */
#define WIZ850IO_Sn_CR_SEND_KEEP 0x22 /* SEND_KEEP command */
#define WIZ850IO_Sn_CR_RECV      0x40 /* RECV command */

/* Socket Status Register (Sn_SR) values */
#define WIZ850IO_Sn_SR_CLOSED      0x00 /* CLOSED */
#define WIZ850IO_Sn_SR_INIT        0x13 /* INIT */
#define WIZ850IO_Sn_SR_LISTEN      0x14 /* LISTEN */
#define WIZ850IO_Sn_SR_ESTABLISHED 0x17 /* ESTABLISHED */
#define WIZ850IO_Sn_SR_CLOSE_WAIT  0x1C /* CLOSE_WAIT */
#define WIZ850IO_Sn_SR_UDP         0x22 /* UDP */
#define WIZ850IO_Sn_SR_MACRAW      0x42 /* MACRAW */

/* Socket Interrupt Register (Sn_IR) values */
#define WIZ850IO_Sn_IR_SEND_OK     0x10 /* SEND_OK */
#define WIZ850IO_Sn_IR_TIMEOUT     0x08 /* TIMEOUT */
#define WIZ850IO_Sn_IR_RECV        0x04 /* RECV */
#define WIZ850IO_Sn_IR_DISCON      0x02 /* DISCON */
#define WIZ850IO_Sn_IR_CON         0x01 /* CON */

/* PHY Configuration Register (PHYCFGR) values */
#define WIZ850IO_PHYCFGR_RST     0x80 /* Reset */
#define WIZ850IO_PHYCFGR_OPMD    0x40 /* Operation Mode */
#define WIZ850IO_PHYCFGR_OPMDC   0x38 /* Operation Mode Configuration */
#define WIZ850IO_PHYCFGR_DPX     0x04 /* Duplex Status (1=Full, 0=Half) */
#define WIZ850IO_PHYCFGR_SPD     0x02 /* Speed Status (1=100Mbps, 0=10Mbps) */
#define WIZ850IO_PHYCFGR_LNK     0x01 /* Link Status (1=Up, 0=Down) */

struct wiz850io_dev
{
        struct no_os_gpio_desc *gpio_reset;
        struct no_os_spi_desc *spi;
        uint8_t mac_addr[6];
        uint8_t ip_addr[4];
        uint8_t subnet_mask[4];
        uint8_t gateway[4];
};

struct wiz850io_init_param
{
        struct no_os_gpio_init_param gpio_reset;
        struct no_os_spi_init_param spi_init;
        uint8_t mac_addr[6];
        uint8_t ip_addr[4];
        uint8_t subnet_mask[4];
        uint8_t gateway[4];
};

int wiz850io_reg_write(struct wiz850io_dev *dev, uint8_t block,
                       uint16_t addr, const uint8_t *data, uint16_t len);

int wiz850io_reg_read(struct wiz850io_dev *dev, uint8_t block,
                      uint16_t addr, uint8_t *data, uint16_t len);

/* Network configuration functions */
int wiz850io_set_mac(struct wiz850io_dev *dev, const uint8_t mac[6]);
int wiz850io_set_ip(struct wiz850io_dev *dev, const uint8_t ip[4]);
int wiz850io_set_subnet(struct wiz850io_dev *dev, const uint8_t subnet[4]);
int wiz850io_set_gateway(struct wiz850io_dev *dev, const uint8_t gateway[4]);

int wiz850io_get_mac(struct wiz850io_dev *dev, uint8_t mac[6]);
int wiz850io_get_ip(struct wiz850io_dev *dev, uint8_t ip[4]);
int wiz850io_get_subnet(struct wiz850io_dev *dev, uint8_t subnet[4]);
int wiz850io_get_gateway(struct wiz850io_dev *dev, uint8_t gateway[4]);

int wiz850io_setup(struct wiz850io_dev *dev);

/* Initialize the device */
int wiz850io_init(struct wiz850io_dev **, struct wiz850io_init_param *);

/* Free a device descriptor */
int wiz850io_remove(struct wiz850io_dev *);

#endif
