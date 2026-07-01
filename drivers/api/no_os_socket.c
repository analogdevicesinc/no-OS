/***************************************************************************//**
 *   @file   no_os_socket.c
 *   @brief  Implementation of the socket interface.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include "no_os_net.h"
#include "no_os_socket.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * @brief Allocate and stamp a socket descriptor.
 *
 * Backend socket_open()/accept() helper: allocates a no_os_socket_desc and
 * fills in the owning network interface, the backend socket operations and the
 * backend socket identifier.
 * @param net - Network interface that owns the socket.
 * @param ops - Backend specific socket operations.
 * @param id - Backend socket identifier.
 * @return Pointer to the new descriptor, or NULL on allocation failure.
 */
struct no_os_socket_desc *no_os_socket_alloc(struct no_os_net_desc *net,
		const struct no_os_socket_ops *ops, uint32_t id)
{
	struct no_os_socket_desc *desc;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return NULL;

	desc->net = net;
	desc->ops = ops;
	desc->id = id;

	return desc;
}

/**
 * @brief Open (create) a socket on a network interface.
 *
 * The socket is manufactured by the owning network interface, which stamps it
 * with the backend specific socket operations.
 * @param desc - The socket descriptor.
 * @param param - The structure that contains the socket parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_init(struct no_os_socket_desc **desc,
			  const struct no_os_socket_init_param *param)
{
	int32_t ret;
	struct no_os_net_desc *net;

	if (!desc || !param || !param->net || !param->net->platform_ops)
		return -EINVAL;

	net = param->net;

	if (!net->platform_ops->socket_open)
		return -ENOSYS;

	ret = net->platform_ops->socket_open(net, desc, param->proto,
					     param->buff_size);
	if (ret)
		return ret;

	(*desc)->net = net;

	return 0;
}

/**
 * @brief Close a socket and free the resources allocated by no_os_socket_init().
 * @param desc - The socket descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_remove(struct no_os_socket_desc *desc)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->close)
		return -ENOSYS;

	return desc->ops->close(desc);
}

/**
 * @brief Connect a socket to a remote host.
 * @param desc - The socket descriptor.
 * @param addr - The remote host address.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_connect(struct no_os_socket_desc *desc,
			     struct no_os_sockaddr *addr)
{
	if (!desc || !desc->ops || !addr)
		return -EINVAL;

	if (!desc->ops->connect)
		return -ENOSYS;

	return desc->ops->connect(desc, addr);
}

/**
 * @brief Disconnect a socket from a remote host.
 * @param desc - The socket descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_disconnect(struct no_os_socket_desc *desc)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->disconnect)
		return -ENOSYS;

	return desc->ops->disconnect(desc);
}

/**
 * @brief Send data over a connected socket.
 * @param desc - The socket descriptor.
 * @param data - Buffer of data to send.
 * @param size - Size of the buffer in bytes.
 * @return Number of bytes sent in case of success, error code otherwise.
 */
int32_t no_os_socket_send(struct no_os_socket_desc *desc, const void *data,
			  uint32_t size)
{
	if (!desc || !desc->ops || !data)
		return -EINVAL;

	if (!desc->ops->send)
		return -ENOSYS;

	return desc->ops->send(desc, data, size);
}

/**
 * @brief Receive data over a connected socket.
 * @param desc - The socket descriptor.
 * @param data - Destination buffer for received data.
 * @param size - Maximum number of bytes to read.
 * @return Number of bytes received in case of success, error code otherwise.
 */
int32_t no_os_socket_recv(struct no_os_socket_desc *desc, void *data,
			  uint32_t size)
{
	if (!desc || !desc->ops || !data)
		return -EINVAL;

	if (!desc->ops->recv)
		return -ENOSYS;

	return desc->ops->recv(desc, data, size);
}

/**
 * @brief Send a datagram to a remote host.
 * @param desc - The socket descriptor.
 * @param data - Buffer of data to send.
 * @param size - Size of the buffer in bytes.
 * @param to - Address of the remote host.
 * @return Number of bytes sent in case of success, error code otherwise.
 */
int32_t no_os_socket_sendto(struct no_os_socket_desc *desc, const void *data,
			    uint32_t size, const struct no_os_sockaddr *to)
{
	if (!desc || !desc->ops || !data || !to)
		return -EINVAL;

	if (!desc->ops->sendto)
		return -ENOSYS;

	return desc->ops->sendto(desc, data, size, to);
}

/**
 * @brief Receive a datagram, optionally reporting the source address.
 * @param desc - The socket descriptor.
 * @param data - Destination buffer for received data.
 * @param size - Maximum number of bytes to read.
 * @param from - Destination for the source address, or NULL.
 * @return Number of bytes received in case of success, error code otherwise.
 */
int32_t no_os_socket_recvfrom(struct no_os_socket_desc *desc, void *data,
			      uint32_t size, struct no_os_sockaddr *from)
{
	if (!desc || !desc->ops || !data)
		return -EINVAL;

	if (!desc->ops->recvfrom)
		return -ENOSYS;

	return desc->ops->recvfrom(desc, data, size, from);
}

/**
 * @brief Bind a socket to a local port.
 * @param desc - The socket descriptor.
 * @param port - Port to bind.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_bind(struct no_os_socket_desc *desc, uint16_t port)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->bind)
		return -ENOSYS;

	return desc->ops->bind(desc, port);
}

/**
 * @brief Start listening for incoming connections.
 * @param desc - The socket descriptor.
 * @param backlog - Number of connections in the socket's listen queue.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_listen(struct no_os_socket_desc *desc, uint32_t backlog)
{
	if (!desc || !desc->ops)
		return -EINVAL;

	if (!desc->ops->listen)
		return -ENOSYS;

	return desc->ops->listen(desc, backlog);
}

/**
 * @brief Accept an incoming connection, returning a new socket.
 * @param desc - The listening socket descriptor.
 * @param client - Destination for the newly accepted socket descriptor.
 * @return 0 in case of success, error code otherwise (-EAGAIN if no pending
 *         connection is available).
 */
int32_t no_os_socket_accept(struct no_os_socket_desc *desc,
			    struct no_os_socket_desc **client)
{
	if (!desc || !desc->ops || !client)
		return -EINVAL;

	if (!desc->ops->accept)
		return -ENOSYS;

	return desc->ops->accept(desc, client);
}
