/***************************************************************************//**
 *   @file   no_os_net.h
 *   @brief  Header file of the network interface.
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
#ifndef _NO_OS_NET_H_
#define _NO_OS_NET_H_

#include <stdint.h>
#include "no_os_socket.h"

/**
 * @struct no_os_net_platform_ops
 * @brief Structure holding network interface function pointers that point to
 * the backend specific implementation.
 */
struct no_os_net_platform_ops;

/**
 * @struct no_os_net_init_param
 * @brief Structure holding the parameters for network interface initialization.
 */
struct no_os_net_init_param {
	/** Backend specific network operations (const, resides in flash) */
	const struct no_os_net_platform_ops	*platform_ops;
	/** Network interface extra parameters (backend specific) */
	void					*extra;
};

/**
 * @struct no_os_net_desc
 * @brief Structure holding the network interface descriptor.
 */
struct no_os_net_desc {
	/** Backend specific network operations (const, resides in flash) */
	const struct no_os_net_platform_ops	*platform_ops;
	/** Network interface extra parameters (backend specific) */
	void					*extra;
};

/**
 * @struct no_os_net_platform_ops
 * @brief Structure holding network interface function pointers that point to
 * the backend specific implementation.
 */
struct no_os_net_platform_ops {
	/** Network interface initialization function pointer */
	int32_t (*init)(struct no_os_net_desc **,
			const struct no_os_net_init_param *);
	/** Network interface remove function pointer */
	int32_t (*remove)(struct no_os_net_desc *);
	/**
	 * Service the network interface. Must be called periodically from the
	 * application loop to drive stack timers and poll the underlying MAC.
	 */
	int32_t (*step)(struct no_os_net_desc *);
	/** Get the current IP address as a numeric string */
	int32_t (*get_ip)(struct no_os_net_desc *, char *buf, uint32_t size);
	/** Resolve a hostname to an address */
	int32_t (*resolve)(struct no_os_net_desc *, const char *host,
			   struct no_os_sockaddr *addr);
	/** Open (create) a socket on this network interface */
	int32_t (*socket_open)(struct no_os_net_desc *,
			       struct no_os_socket_desc **,
			       enum no_os_socket_protocol proto,
			       uint32_t buff_size);
};

/* Initialize a network interface. */
int32_t no_os_net_init(struct no_os_net_desc **desc,
		       const struct no_os_net_init_param *param);

/* Free the resources allocated by no_os_net_init(). */
int32_t no_os_net_remove(struct no_os_net_desc *desc);

/* Service the network interface. Call periodically from the application loop. */
int32_t no_os_net_step(struct no_os_net_desc *desc);

/* Get the current IP address as a numeric string. */
int32_t no_os_net_get_ip(struct no_os_net_desc *desc, char *buf, uint32_t size);

/* Resolve a hostname to an address. */
int32_t no_os_net_resolve(struct no_os_net_desc *desc, const char *host,
			  struct no_os_sockaddr *addr);

/* Parse a dotted-decimal IPv4 string ("a.b.c.d") into 4 bytes. */
int32_t no_os_net_ipv4_to_bytes(const char *str, uint8_t ip[4]);

/* Format 4 bytes as a dotted-decimal IPv4 string into buf. */
int32_t no_os_net_ipv4_to_str(const uint8_t ip[4], char *buf, uint32_t size);

#endif // _NO_OS_NET_H_
