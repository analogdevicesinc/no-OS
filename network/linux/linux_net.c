/***************************************************************************//**
 *   @file   linux_net.c
 *   @brief  Linux adapter for the no_os_net / no_os_socket interfaces.
 *   @author Ramona Nechita (ramona.nechita@analog.com)
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

#if defined(NO_OS_NET) && defined(LINUX_PLATFORM)

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include "linux_net.h"
#include "no_os_net.h"
#include "no_os_socket.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

static const struct no_os_socket_ops linux_socket_ops;

/**
 * @brief Map a no_os_socket_protocol to the socket() type/protocol pair.
 * @param proto - transport protocol.
 * @param type - destination for the SOCK_* type.
 * @param ipproto - destination for the IPPROTO_* protocol.
 * @return 0 on success, -EPROTONOSUPPORT if unknown.
 */
static int32_t _proto_to_linux(enum no_os_socket_protocol proto, int *type,
			       int *ipproto)
{
	switch (proto) {
	case NO_OS_SOCKET_TCP:
		*type = SOCK_STREAM;
		*ipproto = IPPROTO_TCP;
		return 0;
	case NO_OS_SOCKET_UDP:
		*type = SOCK_DGRAM;
		*ipproto = IPPROTO_UDP;
		return 0;
	default:
		return -EPROTONOSUPPORT;
	}
}

/**
 * @brief Resolve a no_os_sockaddr into a struct sockaddr_in.
 * @param addr - source address (numeric string or hostname + port).
 * @param out - destination sockaddr_in.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t _resolve_sockaddr(const struct no_os_sockaddr *addr,
				 struct sockaddr_in *out)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int ret;

	if (!addr || !addr->addr || !out)
		return -EINVAL;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(addr->addr, NULL, &hints, &res);
	if (ret || !res)
		return -EADDRNOTAVAIL;

	memcpy(out, res->ai_addr, sizeof(*out));
	out->sin_port = htons(addr->port);

	freeaddrinfo(res);

	return 0;
}

/**
 * @brief Configure the network interface.
 *
 * Nothing to bring up: the Linux kernel already owns the stack. A descriptor is
 * allocated only so the rest of the HAL has a handle to dispatch on.
 * @param desc - the network interface descriptor to be returned.
 * @param param - initialization parameter (extra is unused).
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_net_init(struct no_os_net_desc **desc,
			      const struct no_os_net_init_param *param)
{
	struct no_os_net_desc *nd;
	struct linux_net_desc *ld;

	NO_OS_UNUSED_PARAM(param);

	if (!desc)
		return -EINVAL;

	nd = no_os_calloc(1, sizeof(*nd));
	if (!nd)
		return -ENOMEM;

	ld = no_os_calloc(1, sizeof(*ld));
	if (!ld) {
		no_os_free(nd);
		return -ENOMEM;
	}

	nd->extra = ld;
	*desc = nd;

	return 0;
}

/**
 * @brief Free the resources allocated by linux_net_init().
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_net_remove(struct no_os_net_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Service the network interface.
 *
 * The Linux kernel drives the whole stack; this is a no-op kept for interface
 * symmetry (iio calls it unconditionally from its poll loop).
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_net_step(struct no_os_net_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	return 0;
}

/**
 * @brief Resolve a hostname to an address.
 * @param desc - the network interface descriptor.
 * @param host - hostname to resolve.
 * @param addr - destination for the resolved address; addr->addr must point to
 *		 a buffer of at least INET_ADDRSTRLEN bytes.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_net_resolve(struct no_os_net_desc *desc, const char *host,
				 struct no_os_sockaddr *addr)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	struct sockaddr_in *sin;
	int ret;

	if (!desc || !host || !addr || !addr->addr)
		return -EINVAL;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(host, NULL, &hints, &res);
	if (ret || !res)
		return -EADDRNOTAVAIL;

	sin = (struct sockaddr_in *)res->ai_addr;
	if (!inet_ntop(AF_INET, &sin->sin_addr, addr->addr, INET_ADDRSTRLEN)) {
		freeaddrinfo(res);
		return -EADDRNOTAVAIL;
	}

	freeaddrinfo(res);

	return 0;
}

/**
 * @brief Open (create) a socket on the network interface.
 *
 * The socket is created non-blocking. Its file descriptor is stored directly in
 * no_os_socket_desc::id, so the Linux backend needs no socket pool.
 * @param net - the owning network interface descriptor.
 * @param sock - destination for the newly created socket descriptor.
 * @param proto - Layer 4 protocol (TCP or UDP).
 * @param buff_size - unused (the kernel manages socket buffers).
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_net_socket_open(struct no_os_net_desc *net,
				     struct no_os_socket_desc **sock,
				     enum no_os_socket_protocol proto,
				     uint32_t buff_size)
{
	struct no_os_socket_desc *s;
	int type, ipproto;
	int32_t ret;
	int flags;
	int fd;

	NO_OS_UNUSED_PARAM(buff_size);

	if (!net || !net->extra || !sock)
		return -EINVAL;

	ret = _proto_to_linux(proto, &type, &ipproto);
	if (ret)
		return ret;

	fd = socket(AF_INET, type, ipproto);
	if (fd < 0)
		return -errno;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		ret = -errno;
		close(fd);
		return ret;
	}

	s = no_os_socket_alloc(net, &linux_socket_ops, (uint32_t)fd);
	if (!s) {
		close(fd);
		return -ENOMEM;
	}

	*sock = s;

	return 0;
}

/**
 * @brief Close a socket connection and free its descriptor.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_sock_close(struct no_os_socket_desc *sock)
{
	int32_t ret = 0;

	if (!sock)
		return -EINVAL;

	if (close((int)sock->id) < 0)
		ret = -errno;

	no_os_free(sock);

	return ret;
}

/**
 * @brief Connect to a remote host.
 * @param sock - the socket descriptor.
 * @param addr - IP/port (or hostname/port) of the remote host.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_sock_connect(struct no_os_socket_desc *sock,
				  struct no_os_sockaddr *addr)
{
	struct sockaddr_in saddr = {0};
	int32_t ret;

	if (!sock || !addr)
		return -EINVAL;

	ret = _resolve_sockaddr(addr, &saddr);
	if (ret)
		return ret;

	if (connect((int)sock->id, (struct sockaddr *)&saddr,
		    sizeof(saddr)) < 0) {
		/* Non-blocking connect completes asynchronously. */
		if (errno == EINPROGRESS || errno == EALREADY)
			return -EAGAIN;
		if (errno == EISCONN)
			return 0;
		return -errno;
	}

	return 0;
}

/**
 * @brief Close a connection from the client side.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_sock_disconnect(struct no_os_socket_desc *sock)
{
	if (!sock)
		return -EINVAL;

	if (shutdown((int)sock->id, SHUT_RDWR) < 0) {
		/* Peer may have already closed the connection. */
		if (errno != ENOTCONN)
			return -errno;
	}

	return 0;
}

/**
 * @brief Send data over a connected socket.
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of the data array.
 * @return Number of bytes sent, negative error code otherwise.
 */
static int32_t linux_sock_send(struct no_os_socket_desc *sock,
			       const void *data, uint32_t size)
{
	ssize_t ret;

	if (!sock || !data)
		return -EINVAL;

	ret = send((int)sock->id, data, size, MSG_NOSIGNAL);
	if (ret < 0)
		return -errno;

	return (int32_t)ret;
}

/**
 * @brief Receive data over a connected socket (non-blocking).
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of data to be read.
 * @return Number of bytes received, -EAGAIN if none pending, error otherwise.
 */
static int32_t linux_sock_recv(struct no_os_socket_desc *sock, void *data,
			       uint32_t size)
{
	ssize_t ret;

	if (!sock || !data)
		return -EINVAL;

	if (!size)
		return 0;

	ret = recv((int)sock->id, data, size, MSG_DONTWAIT);
	if (ret < 0)
		return -errno;

	/* A stream socket peer has performed an orderly shutdown. */
	if (ret == 0)
		return -ENOTCONN;

	return (int32_t)ret;
}

/**
 * @brief Send a datagram to a remote host (UDP).
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of the data array.
 * @param to - destination address.
 * @return Number of bytes sent, negative error code otherwise.
 */
static int32_t linux_sock_sendto(struct no_os_socket_desc *sock,
				 const void *data, uint32_t size,
				 const struct no_os_sockaddr *to)
{
	struct sockaddr_in saddr = {0};
	ssize_t ret;

	if (!sock || !data || !to)
		return -EINVAL;

	ret = _resolve_sockaddr(to, &saddr);
	if (ret)
		return (int32_t)ret;

	ret = sendto((int)sock->id, data, size, MSG_NOSIGNAL,
		     (struct sockaddr *)&saddr, sizeof(saddr));
	if (ret < 0)
		return -errno;

	return (int32_t)ret;
}

/**
 * @brief Receive a datagram, optionally reporting the source (UDP).
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of data to be read.
 * @param from - destination for the source address, or NULL.
 * @return Number of bytes received, -EAGAIN if none pending, error otherwise.
 */
static int32_t linux_sock_recvfrom(struct no_os_socket_desc *sock, void *data,
				   uint32_t size, struct no_os_sockaddr *from)
{
	struct sockaddr_in saddr = {0};
	socklen_t len = sizeof(saddr);
	ssize_t ret;

	if (!sock || !data)
		return -EINVAL;

	ret = recvfrom((int)sock->id, data, size, MSG_DONTWAIT,
		       (struct sockaddr *)&saddr, &len);
	if (ret < 0)
		return -errno;

	if (from && from->addr) {
		inet_ntop(AF_INET, &saddr.sin_addr, from->addr,
			  INET_ADDRSTRLEN);
		from->port = ntohs(saddr.sin_port);
	}

	return (int32_t)ret;
}

/**
 * @brief Bind a socket to a local port.
 * @param sock - the socket descriptor.
 * @param port - port number to bind the socket to.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_sock_bind(struct no_os_socket_desc *sock, uint16_t port)
{
	struct sockaddr_in saddr = {0};
	int opt = 1;

	if (!sock)
		return -EINVAL;

	setsockopt((int)sock->id, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind((int)sock->id, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
		return -errno;

	return 0;
}

/**
 * @brief Configure a socket to listen for connections (TCP server).
 * @param sock - the socket descriptor.
 * @param backlog - connections queue length.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t linux_sock_listen(struct no_os_socket_desc *sock,
				 uint32_t backlog)
{
	if (!sock)
		return -EINVAL;

	/* The kernel silently clamps oversized backlogs (e.g. SOMAXCONN). */
	if (listen((int)sock->id, (int)backlog) < 0)
		return -errno;

	return 0;
}

/**
 * @brief Accept an incoming connection, returning a new socket (non-blocking).
 * @param sock - the listening socket descriptor.
 * @param client - destination for the newly accepted socket descriptor.
 * @return 0 on success, -EAGAIN if no pending connection, error otherwise.
 */
static int32_t linux_sock_accept(struct no_os_socket_desc *sock,
				 struct no_os_socket_desc **client)
{
	struct no_os_socket_desc *cli;
	int fd;

	if (!sock || !client)
		return -EINVAL;

	fd = accept4((int)sock->id, NULL, NULL, SOCK_NONBLOCK);
	if (fd < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return -EAGAIN;
		return -errno;
	}

	cli = no_os_socket_alloc(sock->net, &linux_socket_ops, (uint32_t)fd);
	if (!cli) {
		close(fd);
		return -ENOMEM;
	}

	*client = cli;

	return 0;
}

static const struct no_os_socket_ops linux_socket_ops = {
	.close = linux_sock_close,
	.connect = linux_sock_connect,
	.disconnect = linux_sock_disconnect,
	.send = linux_sock_send,
	.recv = linux_sock_recv,
	.sendto = linux_sock_sendto,
	.recvfrom = linux_sock_recvfrom,
	.bind = linux_sock_bind,
	.listen = linux_sock_listen,
	.accept = linux_sock_accept,
};

const struct no_os_net_platform_ops linux_net_ops = {
	.init = linux_net_init,
	.remove = linux_net_remove,
	.step = linux_net_step,
	.resolve = linux_net_resolve,
	.socket_open = linux_net_socket_open,
};

#endif /* NO_OS_NET && LINUX_PLATFORM */
