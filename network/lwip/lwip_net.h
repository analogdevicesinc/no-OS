/***************************************************************************//**
 *   @file   lwip_net.h
 *   @brief  lwIP adapter for the no_os_net / no_os_socket interfaces.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
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
#ifndef _LWIP_NET_H_
#define _LWIP_NET_H_

#ifdef NO_OS_NET

#include <stdint.h>
#include "lwip/netif.h"
#include "no_os_net.h"
#include "no_os_socket.h"

#define NO_OS_LWIP_BUFF_SIZE	1530
#define NO_OS_MTU_SIZE		1500
#define NO_OS_MAX_SOCKETS	10

#ifndef CONFIG_NO_OS_DOMAIN_NAME
#define CONFIG_NO_OS_DOMAIN_NAME	"analog"
#endif

#ifndef CONFIG_NO_OS_LWIP_INIT_ONETIME
#define CONFIG_NO_OS_LWIP_INIT_ONETIME		0
#endif

struct lwip_net_desc;

/**
 * @struct lwip_socket_desc
 * @brief lwIP per-connection state (one pool entry per socket).
 *
 * The pool lives inside struct lwip_net_desc; a no_os_socket_desc references
 * its entry through no_os_socket_desc::extra, and its pool index through
 * no_os_socket_desc::id.
 */
struct lwip_socket_desc {
	/** Unique identifier (index into the owning net's socket pool) */
	uint32_t id;
	/** Socket state (these are different from the TCP standard ones) */
	enum {
		/** Available to be used */
		SOCKET_CLOSED,
		/** Connection set as server bound to a port */
		SOCKET_LISTENING,
		/** Connection set as server and accepting incomming conns */
		SOCKET_ACCEPTING,
		/** Accept callback received, waiting call to accept function */
		SOCKET_WAITING_ACCEPT,
		/** Socket is connected to remote */
		SOCKET_CONNECTED,
	} state;
	/** Lwip specific descriptor for each connection. */
	struct tcp_pcb *pcb;
	/** Either a packet buffer chain or queue containing the received frames */
	struct pbuf *p;
	/** Index of the current read byte in the first pbuf of the chain */
	uint32_t p_idx;
	/** Reference to the parent network descriptor. */
	struct lwip_net_desc *net;
};

/**
 * @struct lwip_mac_ops
 * @brief MAC/PHY backend operations for the lwIP adapter.
 *
 * Provided by a netdev glue layer (e.g. adin1110) and passed to the adapter
 * through struct lwip_net_param.
 */
struct lwip_mac_ops {
	/** MAC/PHY init */
	int32_t (*init)(void **, void *);
	/** MAC/PHY remove */
	int32_t (*remove)(void *);
	/** Push a pbuf on the wire */
	int32_t (*netif_output)(struct netif *, struct pbuf *);
	/** Poll the MAC and feed received frames into lwIP */
	int32_t (*step)(struct lwip_net_desc *desc, void *);
};

/**
 * @struct lwip_net_desc
 * @brief lwIP backend state, stored in no_os_net_desc::extra.
 */
struct lwip_net_desc {
	/** MAC/PHY backend descriptor */
	void *mac_desc;
	/** lwIP network interface */
	struct netif *lwip_netif;
	/** MAC/PHY backend operations */
	const struct lwip_mac_ops *mac_ops;
	/** MAC address of the interface */
	uint8_t hwaddr[6];
	/** Socket pool */
	struct lwip_socket_desc sockets[NO_OS_MAX_SOCKETS];
	/** Backend extra parameters */
	void *extra;
};

/**
 * @struct lwip_net_param
 * @brief lwIP adapter parameters, passed via no_os_net_init_param::extra.
 */
struct lwip_net_param {
	/** The MAC address of the interface */
	uint8_t hwaddr[6];
	/** MAC/PHY backend operations */
	const struct lwip_mac_ops *mac_ops;
	/** MAC/PHY backend init parameter */
	void *mac_param;
	/** Backend extra parameters */
	void *extra;
};

/** Network platform operations implementing the lwIP adapter. */
extern const struct no_os_net_platform_ops lwip_net_ops;

#endif /* NO_OS_NET */
#endif /* _LWIP_NET_H_ */
