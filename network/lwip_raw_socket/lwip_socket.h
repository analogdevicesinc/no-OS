/***************************************************************************//**
 *   @file   lwip_socket.c
 *   @brief  Source file for the LWIP socket layer
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
