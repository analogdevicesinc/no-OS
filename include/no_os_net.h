/***************************************************************************//**
 *   @file   no_os_net.h
 *   @brief  Header file of Network Interface
 *   @author Alisa-Dariana Roman <alisa.roman@analog.com>
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

struct network_interface;

/**
 * @struct no_os_net_ip_config
 * @brief IPv4 address configuration.
 *
 * When passed as non-NULL to no_os_net_init(), the backend uses these values
 * for static IP configuration. A NULL pointer means "use DHCP" (or "not
 * applicable" for backends that do not manage IP themselves).
 */
struct no_os_net_ip_config {
	/** IPv4 address */
	uint8_t ip[4];
	/** Subnet mask */
	uint8_t netmask[4];
	/** Default gateway */
	uint8_t gateway[4];
};

/**
 * @struct no_os_net_platform_ops
 * @brief Structure holding network device function pointers that point to the
 * platform specific function
 */
struct no_os_net_platform_ops ;

/**
 * @struct no_os_net_init_param
 * @brief Structure holding the parameters for network device initialization
 */
struct no_os_net_init_param {
	/** Network device platform ops */
	const struct no_os_net_platform_ops	*platform_ops;
	/** Network device extra parameters (device specific) */
	void					*extra;
	/** IPv4 configuration (NULL = DHCP / backend default) */
	struct no_os_net_ip_config		*ip_config;
	/** MAC address (NULL = use backend/driver default) */
	uint8_t					*hwaddr;
};

/**
 * @struct no_os_net_desc
 * @brief Structure holding network device descriptor.
 */
struct no_os_net_desc {
	/** Network device platform ops */
	const struct no_os_net_platform_ops	*platform_ops;
	/** Network device extra parameters (device specific) */
	void					*extra;
	struct network_interface		*net_if;
};

/**
 * @struct no_os_net_platform_ops
 * @brief Structure holding network device function pointers that point to the
 * platform specific function
 */
struct no_os_net_platform_ops {
	/** Network device initialization function pointer */
	int32_t (*init)(struct no_os_net_desc **, const struct no_os_net_init_param *);
	/** Network device remove function pointer */
	int32_t (*remove)(struct no_os_net_desc *);
	/** Get current IP configuration */
	int32_t (*get_ip)(struct no_os_net_desc *, struct no_os_net_ip_config *);
	/** Process pending network events (polling, timers) */
	int32_t (*step)(struct no_os_net_desc *);
};

/* Initialize the Network device */
int32_t no_os_net_init(struct no_os_net_desc **desc,
		       const struct no_os_net_init_param *param);

/* Free the resources allocated by no_os_net_init(). */
int32_t no_os_net_remove(struct no_os_net_desc *desc);

/* Process pending network events (polling, timers). No-op if not needed. */
int32_t no_os_net_step(struct no_os_net_desc *desc);

/* Get the current IP/netmask/gateway configuration. */
int32_t no_os_net_get_ip(struct no_os_net_desc *desc,
			 struct no_os_net_ip_config *ip);

#endif // _NO_OS_NET_H_
