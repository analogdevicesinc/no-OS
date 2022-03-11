/***************************************************************************//**
 * @file linux_socket.c
 * @brief Implementation of socket functions for linux sockets.
 * @author Ramona Nechita (ramona.nechita@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#ifdef LINUX_PLATFORM
#define _GNU_SOURCE

#include "linux_socket.h"

#include <stdlib.h>
#include <assert.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>

/******************************************************************************/
/*************************** FUnctions Declarations *******************************/
/******************************************************************************/

/** @brief See \ref network_interface.socket_open */
static int32_t linux_socket_open(void *desc, uint32_t *sock_id,
				 enum socket_protocol prot, uint32_t buff_size)
{
	int32_t flags;
	int err;

	err = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(err < 0)
		return err;

	*sock_id = err;
	flags = fcntl(*sock_id, F_GETFL);
	fcntl(*sock_id, F_SETFL, flags | O_NONBLOCK);

	return 0;
}

/** @brief See \ref network_interface.socket_close */
static int32_t linux_socket_close(void *desc, uint32_t sock_id)
{
	int32_t ret;

	ret = close(sock_id);
	if(ret < 0)
		return -errno;

	return ret;
}

/** @brief See \ref network_interface.socket_connect */
static int32_t linux_socket_connect(void *desc, uint32_t sock_id,
				    struct socket_address *addr)
{
	int32_t ret;
	struct hostent* hptr;

	struct sockaddr_in saddr = {0};
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(addr->port);
	hptr = gethostbyname(addr->addr);
	saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	socklen_t len = sizeof(saddr);

	ret = connect(sock_id,(struct sockaddr*) &saddr,len);

	if(ret < 0)
		return -errno;

	return ret;

}

/** @brief See \ref network_interface.socket_disconnect */
static int32_t linux_socket_disconnect(void *desc,
				       uint32_t sock_id)
{
	int32_t ret;

	ret = linux_socket_close(desc, sock_id);

	if(ret < 0)
		return -errno;

	return 0;
}

/** @brief See \ref network_interface.socket_send */
static int32_t linux_socket_send(void *desc, uint32_t sock_id,
				 const void *data, uint32_t size)
{
	int32_t ret;

	ret = send(sock_id, data, size, 0);

	if(ret < 0)
		return -errno;

	return size;
}

/** @brief See \ref network_interface.socket_recv */
static int32_t linux_socket_recv(void *desc, uint32_t sock_id,
				 void *data, uint32_t size)
{
	int32_t ret;

	if (!size)
		return size;

	ret = recv(sock_id, data, size, MSG_DONTWAIT);
	if(ret < 0)
		return -errno;

	/* A stream socket peer has performed an orderly shutdown */
	if(ret == 0)
		return -ENOTCONN;

	return ret;
}

/** @brief See \ref network_interface.socket_sendto */
static int32_t linux_socket_sendto(void *desc, uint32_t sock_id,
				   const void *data, uint32_t size,
				   const struct socket_address* to)
{
	int32_t ret;
	struct sockaddr_in saddr_to = {0};
	socklen_t len;
	struct hostent* hptr;

	saddr_to.sin_family = AF_INET;
	saddr_to.sin_port = htons(to->port);
	hptr = gethostbyname(to->addr);
	saddr_to.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	len = sizeof(saddr_to);

	ret = sendto(sock_id, data, size, 0, (struct sockaddr*) &saddr_to, len);

	if(ret != 0)
		return ret;

	return 0;
}

/** @brief See \ref network_interface.socket_recvfrom */
static int32_t linux_socket_recvfrom(void *desc, uint32_t sock_id,
				     void *data, uint32_t size,
				     struct socket_address *from)
{
	int32_t ret;
	struct sockaddr_in saddr_from = {0};
	socklen_t len;
	struct hostent* hptr;

	saddr_from.sin_family = AF_INET;
	saddr_from.sin_port = htons(from->port);
	hptr = gethostbyname(from->addr);
	saddr_from.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	len = sizeof(saddr_from);

	ret = recvfrom(sock_id, data, size, MSG_DONTWAIT,(struct sockaddr*) &saddr_from,
		       &len);

	if(ret != 0)
		return ret;

	return 0;
}

/** @brief See \ref network_interface.socket_bind */
static int32_t linux_socket_bind(void *desc, uint32_t sock_id,
				 uint16_t port)
{
	int32_t ret;
	struct sockaddr_in saddr = {0};
	socklen_t len;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	len = sizeof(saddr);

	ret = bind(sock_id, (struct sockaddr*) &saddr, len);

	if(ret < 0)
		return -errno;

	return ret;
}

/** @brief See \ref network_interface.socket_listen */
static int32_t linux_socket_listen(void *desc, uint32_t sock_id,
				   uint32_t back_log)
{
	int32_t ret;

	ret = listen(sock_id, back_log);

	if(ret < 0)
		return -errno;

	return ret;
}

/** @brief See \ref network_interface.socket_accept */
static int32_t linux_socket_accept(void *desc, uint32_t sock_id,
				   uint32_t *client_socket_id)
{
	int32_t ret;

	ret = accept4(sock_id, NULL, NULL, SOCK_NONBLOCK);

	if(ret < 0)
		return -errno;

	*client_socket_id = ret;

	return 0;
}

struct network_interface linux_net = {
	.socket_open = (int32_t (*)(void *, uint32_t *, enum socket_protocol,
				    uint32_t)) linux_socket_open,
	.socket_close = (int32_t (*)(void *, uint32_t)) linux_socket_close,
	.socket_connect = (int32_t (*)(void *, uint32_t,struct socket_address *))linux_socket_connect,
	.socket_disconnect = (int32_t (*)(void *, uint32_t))linux_socket_disconnect,
	.socket_send = (int32_t (*)(void *, uint32_t, const void *, uint32_t))linux_socket_send,
	.socket_recv = (int32_t (*)(void *, uint32_t, void *, uint32_t))linux_socket_recv,
	.socket_sendto = (int32_t (*)(void *, uint32_t, const void *, uint32_t, const struct socket_address* to))linux_socket_sendto,
	.socket_recvfrom = (int32_t (*)(void *, uint32_t, void *, uint32_t, struct socket_address* from))linux_socket_recvfrom,
	.socket_bind = (int32_t (*)(void *, uint32_t, uint16_t))linux_socket_bind,
	.socket_listen = (int32_t (*)(void *, uint32_t, uint32_t))linux_socket_listen,
	.socket_accept= (int32_t (*)(void *, uint32_t, uint32_t*))linux_socket_accept
};

#endif
