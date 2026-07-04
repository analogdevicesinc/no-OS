/***************************************************************************//**
 *   @file   no_os_socket.h
 *   @brief  Header file of the socket interface.
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
#ifndef _NO_OS_SOCKET_H_
#define _NO_OS_SOCKET_H_

#include <stdint.h>

/** Maximum backlog for no_os_socket_listen(). */
#define NO_OS_SOCKET_MAX_BACKLOG	0xFFFFFFFF

/* Network interface descriptor (defined in no_os_net.h). */
struct no_os_net_desc;

/**
 * @enum no_os_socket_protocol
 * @brief Transport protocol used by a socket.
 */
enum no_os_socket_protocol {
	/** TCP Protocol */
	NO_OS_SOCKET_TCP,
	/** UDP Protocol */
	NO_OS_SOCKET_UDP
};

/**
 * @struct no_os_sockaddr
 * @brief Represents an endpoint of a connection.
 */
struct no_os_sockaddr {
	/** Address (numeric string, e.g. "192.168.0.1") */
	char		*addr;
	/** Port */
	uint16_t	port;
};

/**
 * @struct no_os_socket_ops
 * @brief Structure holding socket function pointers that point to the backend
 * specific implementation.
 */
struct no_os_socket_ops;

/**
 * @struct no_os_socket_init_param
 * @brief Structure holding the parameters for socket initialization.
 */
struct no_os_socket_init_param {
	/** Network interface that will create (own) the socket */
	struct no_os_net_desc		*net;
	/** Transport protocol */
	enum no_os_socket_protocol	proto;
	/**
	 * Max buffer size for incoming data. If set to 0, the backend default
	 * is used.
	 */
	uint32_t			buff_size;
};

/**
 * @struct no_os_socket_desc
 * @brief Structure holding a socket endpoint descriptor.
 */
struct no_os_socket_desc {
	/** Network interface that owns this socket */
	struct no_os_net_desc		*net;
	/** Backend specific socket operations (const, resides in flash) */
	const struct no_os_socket_ops	*ops;
	/** Backend socket identifier (e.g. index into a hardware socket pool) */
	uint32_t			id;
	/** Socket extra parameters (backend specific) */
	void				*extra;
};

/**
 * @struct no_os_socket_ops
 * @brief Structure holding socket function pointers that point to the backend
 * specific implementation.
 */
struct no_os_socket_ops {
	/** Close the socket and release its slot */
	int32_t (*close)(struct no_os_socket_desc *);
	/** Connect a socket to a remote host */
	int32_t (*connect)(struct no_os_socket_desc *, struct no_os_sockaddr *);
	/** Disconnect a socket from a remote host */
	int32_t (*disconnect)(struct no_os_socket_desc *);
	/** Send data over a connected socket */
	int32_t (*send)(struct no_os_socket_desc *, const void *, uint32_t);
	/** Receive data over a connected socket */
	int32_t (*recv)(struct no_os_socket_desc *, void *, uint32_t);
	/** Send a datagram to a remote host */
	int32_t (*sendto)(struct no_os_socket_desc *, const void *, uint32_t,
			  const struct no_os_sockaddr *);
	/** Receive a datagram, optionally reporting the source address */
	int32_t (*recvfrom)(struct no_os_socket_desc *, void *, uint32_t,
			    struct no_os_sockaddr *);
	/** Bind a socket to a local port */
	int32_t (*bind)(struct no_os_socket_desc *, uint16_t);
	/** Start listening for incoming connections */
	int32_t (*listen)(struct no_os_socket_desc *, uint32_t);
	/** Accept an incoming connection, returning a new socket */
	int32_t (*accept)(struct no_os_socket_desc *,
			  struct no_os_socket_desc **);
};

/*
 * Allocate and stamp a socket descriptor. Backend socket_open()/accept()
 * helper: allocates a no_os_socket_desc and fills in net/ops/id. Returns NULL
 * on allocation failure.
 */
struct no_os_socket_desc *no_os_socket_alloc(struct no_os_net_desc *net,
		const struct no_os_socket_ops *ops, uint32_t id);

/* Open (create) a socket on a network interface. */
int32_t no_os_socket_init(struct no_os_socket_desc **desc,
			  const struct no_os_socket_init_param *param);

/* Close a socket and free the resources allocated by no_os_socket_init(). */
int32_t no_os_socket_remove(struct no_os_socket_desc *desc);

/* Connect a socket to a remote host. */
int32_t no_os_socket_connect(struct no_os_socket_desc *desc,
			     struct no_os_sockaddr *addr);

/* Disconnect a socket from a remote host. */
int32_t no_os_socket_disconnect(struct no_os_socket_desc *desc);

/* Send data over a connected socket. Returns the number of bytes sent. */
int32_t no_os_socket_send(struct no_os_socket_desc *desc, const void *data,
			  uint32_t size);

/* Receive data over a connected socket. Returns the number of bytes received. */
int32_t no_os_socket_recv(struct no_os_socket_desc *desc, void *data,
			  uint32_t size);

/* Send a datagram to a remote host. */
int32_t no_os_socket_sendto(struct no_os_socket_desc *desc, const void *data,
			    uint32_t size, const struct no_os_sockaddr *to);

/* Receive a datagram, optionally reporting the source address. */
int32_t no_os_socket_recvfrom(struct no_os_socket_desc *desc, void *data,
			      uint32_t size, struct no_os_sockaddr *from);

/* Bind a socket to a local port. */
int32_t no_os_socket_bind(struct no_os_socket_desc *desc, uint16_t port);

/* Start listening for incoming connections. */
int32_t no_os_socket_listen(struct no_os_socket_desc *desc, uint32_t backlog);

/* Accept an incoming connection, returning a new socket. */
int32_t no_os_socket_accept(struct no_os_socket_desc *desc,
			    struct no_os_socket_desc **client);

#endif // _NO_OS_SOCKET_H_
