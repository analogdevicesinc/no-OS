/***************************************************************************//**
 *   @file   wifi_net.h
 *   @brief  WiFi (ESP8266) adapter for the no_os_net / no_os_socket interfaces.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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
#ifndef _WIFI_NET_H_
#define _WIFI_NET_H_

#ifdef NO_OS_NET

#include <stdint.h>
#include <stdbool.h>
#include "no_os_net.h"
#include "no_os_socket.h"
#include "no_os_uart.h"
#include "no_os_irq.h"
#include "at_parser.h"

/** One extra socket slot beyond MAX_CONNECTIONS is reserved for the server. */
#define WIFI_NB_SOCKETS		(MAX_CONNECTIONS + 1)
/** Maximum number of client sockets a server can back-log. */
#define WIFI_NB_CLI_SOCKETS	MAX_CONNECTIONS

struct wifi_net_desc;

/**
 * @struct wifi_socket_desc
 * @brief WiFi per-connection state (one pool entry per socket).
 *
 * The ESP8266 runs the TCP/IP stack; the AT parser fills a per-socket circular
 * buffer from the UART interrupt. The pool lives inside struct wifi_net_desc; a
 * no_os_socket_desc references its entry through no_os_socket_desc::extra, and
 * its pool index through no_os_socket_desc::id.
 */
struct wifi_socket_desc {
	/** Buffer given to at_parser for incoming data */
	struct no_os_circular_buffer	*cb;
	/** Circular buffer size */
	uint32_t			cb_size;
	/** Transport protocol of this socket */
	enum no_os_socket_protocol	proto;
	/** ESP8266 connection id */
	uint32_t			conn_id;
	/** Pool index (mirrors no_os_socket_desc::id) */
	uint32_t			id;
	/** Socket state */
	enum {
		/** The socket structure is unused */
		WIFI_SOCKET_UNUSED,
		/** Socket is waiting for accept */
		WIFI_SOCKET_WAITING_ACCEPT,
		/** Socket structure has been initialized */
		WIFI_SOCKET_DISCONNECTED,
		/** Socket connected to a server or to a client */
		WIFI_SOCKET_CONNECTED,
		/** State of server socket when it listens to new connections */
		WIFI_SOCKET_LISTENING
	}				state;
	/** Reference to the parent network descriptor */
	struct wifi_net_desc		*net;
};

/**
 * @struct wifi_server_desc
 * @brief State of the single ESP8266 server socket.
 */
struct wifi_server_desc {
	/** Server socket pool index */
	uint32_t	id;
	/** Server port */
	uint16_t	port;
	/** Pool indices of the pre-allocated client sockets */
	uint32_t	client_ids[WIFI_NB_CLI_SOCKETS];
	/** Number of pre-allocated client sockets */
	uint32_t	back_log_clients;
};

/**
 * @struct wifi_net_desc
 * @brief WiFi backend state, stored in no_os_net_desc::extra.
 */
struct wifi_net_desc {
	/** Socket pool (the last slot is used by the server) */
	struct wifi_socket_desc		sockets[WIFI_NB_SOCKETS];
	/** Server descriptor */
	struct wifi_server_desc		server;
	/** Reference to the AT parser */
	struct at_desc			*at;
	/** Maps an ESP8266 connection id to a socket pool index */
	int32_t				conn_id_to_sock_id[MAX_CONNECTIONS];
};

/**
 * @struct wifi_net_param
 * @brief WiFi adapter parameters, passed via no_os_net_init_param::extra.
 *
 * The adapter associates with the access point during init using ssid/pwd, so
 * consumers going through the generic no_os_net path stay backend-agnostic.
 */
struct wifi_net_param {
	/** UART descriptor the ESP8266 is connected to */
	struct no_os_uart_desc		*uart_desc;
	/** IRQ controller descriptor */
	struct no_os_irq_ctrl_desc	*irq_desc;
	/** Id of the UART interrupt */
	uint32_t			uart_irq_id;
	/** Configuration param for registering the UART callback */
	void				*uart_irq_conf;
	/** ESP8266 software reset enable */
	bool				sw_reset_en;
	/** Access point SSID to join */
	const char			*ssid;
	/** Access point password */
	const char			*pwd;
};

/** Network platform operations implementing the WiFi adapter. */
extern const struct no_os_net_platform_ops wifi_net_ops;

#endif /* NO_OS_NET */
#endif /* _WIFI_NET_H_ */
