/***************************************************************************//**
 *   @file   w5500_net.h
 *   @brief  W5500 adapter for the no_os_net / no_os_socket interfaces.
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
#ifndef _W5500_NET_H_
#define _W5500_NET_H_

#ifdef NO_OS_NET

#include <stdint.h>
#include "no_os_net.h"
#include "no_os_socket.h"
#include "w5500.h"

#ifndef W5500_NET_CONNECT_TIMEOUT_MS
#define W5500_NET_CONNECT_TIMEOUT_MS	5000
#endif

struct w5500_net_desc;

/**
 * @struct w5500_socket_desc
 * @brief W5500 per-connection state (one pool entry per hardware socket).
 *
 * The W5500 owns 8 hardware sockets that each run TCP/UDP internally. The pool
 * lives inside struct w5500_net_desc; a no_os_socket_desc references its entry
 * through no_os_socket_desc::extra, and its physical socket index through
 * no_os_socket_desc::id.
 */
struct w5500_socket_desc {
	/** Physical W5500 socket index (0..W5500_MAX_SOCK_NUMBER) */
	uint32_t			id;
	/** Socket state */
	enum {
		/** Available to be used */
		W5500_SOCK_CLOSED,
		/** Bound and listening for incoming connections (TCP server) */
		W5500_SOCK_LISTENING,
		/** Connected to a remote (TCP), or datagram-ready (UDP) */
		W5500_SOCK_CONNECTED,
	} state;
	/** Transport protocol of this socket */
	enum no_os_socket_protocol	proto;
	/** Local port (remembered so accept() can re-listen on it) */
	uint16_t			local_port;
	/** Socket buffer size in KB, as programmed into the W5500 */
	uint8_t				buff_kb;
	/** Reference to the parent network descriptor */
	struct w5500_net_desc		*net;
};

/**
 * @struct w5500_net_desc
 * @brief W5500 backend state, stored in no_os_net_desc::extra.
 */
struct w5500_net_desc {
	/** W5500 MAC/TCP-offload device descriptor */
	struct w5500_dev		*mac_dev;
	/** Whether mac_dev was created (and thus owned) by this adapter */
	bool				mac_owned;
	/** Socket pool (one slot per hardware socket) */
	struct w5500_socket_desc	sockets[W5500_MAX_SOCK_NUMBER + 1];
	/** Static IP address of the interface */
	uint8_t				ip[4];
	/** Static subnet mask of the interface */
	uint8_t				netmask[4];
	/** Static gateway address of the interface */
	uint8_t				gateway[4];
};

/**
 * @struct w5500_net_param
 * @brief W5500 adapter parameters, passed via no_os_net_init_param::extra.
 *
 * Provide either an already-initialized mac_dev (adopted, not owned) or a
 * w5500_ip init parameter for the adapter to build one.
 */
struct w5500_net_param {
	/** Pre-initialized W5500 device to adopt, or NULL */
	struct w5500_dev		*mac_dev;
	/** W5500 init parameter used when mac_dev is NULL */
	struct w5500_init_param		*w5500_ip;
};

/** Network platform operations implementing the W5500 adapter. */
extern const struct no_os_net_platform_ops w5500_net_ops;

#endif /* NO_OS_NET */
#endif /* _W5500_NET_H_ */
