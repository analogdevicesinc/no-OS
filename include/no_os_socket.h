/***************************************************************************//**
 *   @file   no_os_socket.h
 *   @brief  Header file of Socket Interface
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

#ifndef _NO_OS_SOCKET_H_
#define _NO_OS_SOCKET_H_

#include <stdint.h>

#define NO_OS_SOCKET_MAX_BACKLOG	0xFFFFFFFF

struct no_os_net_desc;

enum no_os_socket_protocol {
	NO_OS_SOCKET_TCP,
	NO_OS_SOCKET_UDP,
};

struct no_os_socket_address {
	uint8_t			ip[4];
	uint16_t		port;
};

struct no_os_socket_platform_ops;

/**
 * @struct no_os_socket_init_param
 * @brief Structure holding the parameters for socket initialization
 */
struct no_os_socket_init_param {
	/** Socket platform ops */
	const struct no_os_socket_platform_ops	*platform_ops;
	/** Socket protocol */
	enum no_os_socket_protocol		protocol;
	/** Network device this socket operates on (NULL if not applicable) */
	struct no_os_net_desc			*net_desc;
	/** Socket extra parameters (device specific) */
	void					*extra;
};

/**
 * @struct no_os_socket_desc
 * @brief Structure holding socket descriptor.
 */
struct no_os_socket_desc {
	/** Socket platform ops */
	const struct no_os_socket_platform_ops	*platform_ops;
	/** Socket protocol */
	enum no_os_socket_protocol		protocol;
	/** Network device this socket operates on (NULL if not applicable) */
	struct no_os_net_desc			*net_desc;
	/** Socket extra parameters (device specific) */
	void					*extra;
};

/**
 * @struct no_os_socket_platform_ops
 * @brief Structure holding pointers to the backend specific functions
 */
struct no_os_socket_platform_ops {
	/** Socket initialization */
	int32_t (*init)(struct no_os_socket_desc **,
			const struct no_os_socket_init_param *);
	/** Socket remove */
	int32_t (*remove)(struct no_os_socket_desc *);
	/** Scoket connect */
	int32_t (*connect)(struct no_os_socket_desc *,
			   struct no_os_socket_address *);
	/** Socket disconnect */
	int32_t (*disconnect)(struct no_os_socket_desc *);
	/** Socket send */
	int32_t (*send)(struct no_os_socket_desc *, const void *, uint32_t);
	/** Socket recv */
	int32_t (*recv)(struct no_os_socket_desc *, void *, uint32_t);
	/** Socket sendto (UDP) */
	int32_t (*sendto)(struct no_os_socket_desc *, const void *, uint32_t,
			  const struct no_os_socket_address *);
	/** Socket recvfrom (UDP) */
	int32_t (*recvfrom)(struct no_os_socket_desc *, void *, uint32_t,
			    struct no_os_socket_address *);
	/** Socket bind */
	int32_t (*bind)(struct no_os_socket_desc *, uint16_t);
	/** Socket listen */
	int32_t (*listen)(struct no_os_socket_desc *, uint32_t);
	/** Socket accept */
	int32_t (*accept)(struct no_os_socket_desc *,
			  struct no_os_socket_desc **);
};

/* Initialize the socket */
int32_t no_os_socket_init(struct no_os_socket_desc **desc,
			  const struct no_os_socket_init_param *param);

/* Free the resources allocated by no_os_socket_init(). */
int32_t no_os_socket_remove(struct no_os_socket_desc *desc);

/* Socket connect */
int32_t no_os_socket_connect(struct no_os_socket_desc *desc,
			     struct no_os_socket_address *addr);

/* Socket disconnect */
int32_t no_os_socket_disconnect(struct no_os_socket_desc *desc);

/* Socket send */
int32_t no_os_socket_send(struct no_os_socket_desc *desc, const void *data,
			  uint32_t len);

/* Socket recv */
int32_t no_os_socket_recv(struct no_os_socket_desc *desc, void *data,
			  uint32_t len);

/* Socket sendto (UDP) */
int32_t no_os_socket_sendto(struct no_os_socket_desc *desc, const void *data,
			    uint32_t len,
			    const struct no_os_socket_address *to);

/* Socket recvfrom (UDP) */
int32_t no_os_socket_recvfrom(struct no_os_socket_desc *desc, void *data,
			      uint32_t len,
			      struct no_os_socket_address *from);

/* Socket bind */
int32_t no_os_socket_bind(struct no_os_socket_desc *desc, uint16_t port);

/* Socket listen */
int32_t no_os_socket_listen(struct no_os_socket_desc *desc, uint32_t back_log);

/* Socket accept */
int32_t no_os_socket_accept(struct no_os_socket_desc *desc,
			    struct no_os_socket_desc **new_client);

#endif // _NO_OS_SOCKET_H_
