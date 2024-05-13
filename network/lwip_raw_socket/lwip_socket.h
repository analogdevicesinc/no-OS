/***************************************************************************//**
 *   @file   lwip_socket.c
 *   @brief  Source file for the LWIP socket layer
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

#ifndef _LWIP_SOCKET_H_
#define _LWIP_SOCKET_H_

#ifdef NO_OS_LWIP_NETWORKING

#include "lwip/netif.h"
#include "network_interface.h"
#include "tcp_socket.h"

#define NO_OS_LWIP_BUFF_SIZE	1530
#define NO_OS_MTU_SIZE		1500
#define NO_OS_MAX_SOCKETS	10

#ifndef NO_OS_DOMAIN_NAME
#define NO_OS_DOMAIN_NAME	"analog"
#endif

#ifndef NO_OS_LWIP_INIT_ONETIME
#define NO_OS_LWIP_INIT_ONETIME		0
#endif

struct lwip_network_desc;

struct lwip_socket_desc {
	/* Unique identifier */
	uint32_t id;
	/* Socket state (these are different from the TCP standard ones) */
	enum {
		/* Available to be used */
		SOCKET_CLOSED,
		/* Connection set as server bound to a port */
		SOCKET_LISTENING,
		/* Connection set as server and accepting incomming conns  */
		SOCKET_ACCEPTING,
		/* Accept callback received, waiting call to accept function */
		SOCKET_WAITING_ACCEPT,
		/* Socket is connected to remote */
		SOCKET_CONNECTED,
	} state;
	/* Lwip specific descriptor for each connection. */
	struct tcp_pcb *pcb;
	/* Either a packet buffer chain or queue containing the received frames */
	struct pbuf *p;
	/* Index of the current read byte in the first pbuf of the chain */
	uint32_t p_idx;
	/* Reference to the parent network descriptor. */
	struct lwip_network_desc *desc;
};

struct lwip_network_desc {
	void *mac_desc;
	struct netif *lwip_netif;
	struct network_interface no_os_net;
	const struct no_os_lwip_ops *platform_ops;
	uint8_t hwaddr[6];
	struct lwip_socket_desc sockets[NO_OS_MAX_SOCKETS];
	void *extra;
};

struct lwip_network_param {
	/* The MAC address of the interface */
	uint8_t hwaddr[6];
	const struct no_os_lwip_ops *platform_ops;
	void *mac_param;
	void *extra;
};

struct no_os_lwip_ops {
	/* MAC/PHY init */
	int32_t (*init)(void **, void *);
	/* MAC/PHY remove */
	int32_t (*remove)(void *);
	int32_t (*netif_output)(struct netif *, struct pbuf *);
	int32_t (*step)(struct lwip_network_desc *desc, void *);
};

/* Initialize lwip stack */
int32_t no_os_lwip_init(struct lwip_network_desc **,
			struct lwip_network_param *);
/* Remove the lwip descriptor */
int32_t no_os_lwip_remove(struct lwip_network_desc *);
/*
 * Should be periodically called in the application code as oftern as possible. By default,
 * it will call the necessary lwip timers.
 */
int32_t no_os_lwip_step(struct lwip_network_desc *, void *);

extern struct network_interface lwip_socket_ops;

#endif /* NO_OS_LWIP_NETWORKING */
#endif
