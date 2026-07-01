/***************************************************************************//**
 * @file linux_socket.c
 * @brief Implementation of socket functions for linux sockets.
 * @author Ramona Nechita (ramona.nechita@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#ifdef LINUX_PLATFORM
#define _GNU_SOURCE

#include "linux_socket.h"

#include <stdlib.h>
#include <assert.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_net.h"
#include "no_os_socket.h"
#include "no_os_alloc.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>

/* ---- no_os_net / no_os_socket backend --------------------------------- */
/*
 * The Linux backend is stateless: a socket is just a POSIX fd, stored directly
 * in no_os_socket_desc.id. There is no per-interface state, so no_os_net_desc
 * only carries the (const) ops table.
 */

static const struct no_os_socket_ops linux_socket_ops;

static int32_t lnx_net_init(struct no_os_net_desc **desc,
			    const struct no_os_net_init_param *param)
{
	struct no_os_net_desc *nd;

	nd = no_os_calloc(1, sizeof(*nd));
	if (!nd)
		return -ENOMEM;

	*desc = nd;

	return 0;
}

static int32_t lnx_net_remove(struct no_os_net_desc *desc)
{
	no_os_free(desc);

	return 0;
}

static int32_t lnx_net_step(struct no_os_net_desc *desc)
{
	/* POSIX sockets need no periodic servicing. */
	return 0;
}

static int32_t lnx_net_resolve(struct no_os_net_desc *desc, const char *host,
			       struct no_os_sockaddr *addr)
{
	struct hostent *hptr;

	hptr = gethostbyname(host);
	if (!hptr || !hptr->h_addr_list[0])
		return -EINVAL;

	/*
	 * Return the address as-is; connect()/sendto() resolve the string again.
	 * Kept simple since the Linux backend is host-side test tooling.
	 */
	addr->addr = (char *)host;

	return 0;
}

/* Close the underlying fd without freeing the descriptor. */
static int32_t lnx_close_fd(struct no_os_socket_desc *sock)
{
	if (close(sock->id) < 0)
		return -errno;

	return 0;
}

static int32_t lnx_socket_open(struct no_os_net_desc *net,
			       struct no_os_socket_desc **sock,
			       enum no_os_socket_protocol proto,
			       uint32_t buff_size)
{
	struct no_os_socket_desc *s;
	int32_t flags;
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0)
		return -errno;

	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	s = no_os_socket_alloc(net, &linux_socket_ops, fd);
	if (!s) {
		close(fd);
		return -ENOMEM;
	}

	*sock = s;

	return 0;
}

static int32_t lnx_socket_close(struct no_os_socket_desc *sock)
{
	int32_t ret;

	ret = lnx_close_fd(sock);
	no_os_free(sock);

	return ret;
}

static int32_t lnx_socket_connect(struct no_os_socket_desc *sock,
				  struct no_os_sockaddr *addr)
{
	struct sockaddr_in saddr = {0};
	struct hostent *hptr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(addr->port);
	hptr = gethostbyname(addr->addr);
	saddr.sin_addr.s_addr = ((struct in_addr *) hptr->h_addr_list[0])->s_addr;

	if (connect(sock->id, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
		return -errno;

	return 0;
}

static int32_t lnx_socket_disconnect(struct no_os_socket_desc *sock)
{
	return lnx_close_fd(sock);
}

static int32_t lnx_socket_send(struct no_os_socket_desc *sock,
			       const void *data, uint32_t size)
{
	if (send(sock->id, data, size, 0) < 0)
		return -errno;

	return size;
}

static int32_t lnx_socket_recv(struct no_os_socket_desc *sock, void *data,
			       uint32_t size)
{
	int32_t ret;

	if (!size)
		return size;

	ret = recv(sock->id, data, size, MSG_DONTWAIT);
	if (ret < 0)
		return -errno;

	/* A stream socket peer has performed an orderly shutdown */
	if (ret == 0)
		return -ENOTCONN;

	return ret;
}

static int32_t lnx_socket_sendto(struct no_os_socket_desc *sock,
				 const void *data, uint32_t size,
				 const struct no_os_sockaddr *to)
{
	struct sockaddr_in saddr_to = {0};
	struct hostent *hptr;
	int32_t ret;

	saddr_to.sin_family = AF_INET;
	saddr_to.sin_port = htons(to->port);
	hptr = gethostbyname(to->addr);
	saddr_to.sin_addr.s_addr = ((struct in_addr *) hptr->h_addr_list[0])->s_addr;

	ret = sendto(sock->id, data, size, 0, (struct sockaddr *) &saddr_to,
		     sizeof(saddr_to));
	if (ret != 0)
		return ret;

	return 0;
}

static int32_t lnx_socket_recvfrom(struct no_os_socket_desc *sock, void *data,
				   uint32_t size, struct no_os_sockaddr *from)
{
	struct sockaddr_in saddr_from = {0};
	struct hostent *hptr;
	socklen_t len;
	int32_t ret;

	saddr_from.sin_family = AF_INET;
	saddr_from.sin_port = htons(from->port);
	hptr = gethostbyname(from->addr);
	saddr_from.sin_addr.s_addr = ((struct in_addr *) hptr->h_addr_list[0])->s_addr;
	len = sizeof(saddr_from);

	ret = recvfrom(sock->id, data, size, MSG_DONTWAIT,
		       (struct sockaddr *) &saddr_from, &len);
	if (ret != 0)
		return ret;

	return 0;
}

static int32_t lnx_socket_bind(struct no_os_socket_desc *sock, uint16_t port)
{
	struct sockaddr_in saddr = {0};

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock->id, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
		return -errno;

	return 0;
}

static int32_t lnx_socket_listen(struct no_os_socket_desc *sock,
				 uint32_t backlog)
{
	if (listen(sock->id, backlog) < 0)
		return -errno;

	return 0;
}

static int32_t lnx_socket_accept(struct no_os_socket_desc *sock,
				 struct no_os_socket_desc **client)
{
	struct no_os_socket_desc *c;
	int fd;

	fd = accept4(sock->id, NULL, NULL, SOCK_NONBLOCK);
	if (fd < 0)
		return -errno;

	c = no_os_socket_alloc(sock->net, &linux_socket_ops, fd);
	if (!c) {
		close(fd);
		return -ENOMEM;
	}

	*client = c;

	return 0;
}

const struct no_os_net_platform_ops linux_net_platform_ops = {
	.init = lnx_net_init,
	.remove = lnx_net_remove,
	.step = lnx_net_step,
	.resolve = lnx_net_resolve,
	.socket_open = lnx_socket_open,
};

static const struct no_os_socket_ops linux_socket_ops = {
	.close = lnx_socket_close,
	.connect = lnx_socket_connect,
	.disconnect = lnx_socket_disconnect,
	.send = lnx_socket_send,
	.recv = lnx_socket_recv,
	.sendto = lnx_socket_sendto,
	.recvfrom = lnx_socket_recvfrom,
	.bind = lnx_socket_bind,
	.listen = lnx_socket_listen,
	.accept = lnx_socket_accept,
};

#endif
