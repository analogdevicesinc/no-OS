/***************************************************************************//**
 *   @file   no_os_socket.c
 *   @brief  Implementation of the Socket Interface
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

#include <inttypes.h>
#include <stdlib.h>

#include "no_os_error.h"
#include "no_os_socket.h"
#include "no_os_util.h"

/**
 * @brief Initialize the socket.
 * @param desc - The socket descriptor.
 * @param param - The structure that contains the socket parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_init(struct no_os_socket_desc **desc,
			  const struct no_os_socket_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	ret = param->platform_ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;
	(*desc)->net_desc = param->net_desc;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_socket_init().
 * @param desc - The socket descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_remove(struct no_os_socket_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Connect to a remote host.
 * @param desc - The socket descriptor.
 * @param addr - Remote address (IP + port).
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_connect(struct no_os_socket_desc *desc,
			     struct no_os_socket_address *addr)
{
	if (!desc || !desc->platform_ops || !addr)
		return -EINVAL;

	if (!desc->platform_ops->connect)
		return -ENOSYS;

	return desc->platform_ops->connect(desc, addr);
}

/**
 * @brief Disconnect from the remote host.
 * @param desc - The socket descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_disconnect(struct no_os_socket_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->disconnect)
		return -ENOSYS;

	return desc->platform_ops->disconnect(desc);
}

/**
 * @brief Send data over a connected socket.
 * @param desc - The socket descriptor.
 * @param data - Buffer containing data to send.
 * @param len - Number of bytes to send.
 * @return Number of bytes sent on success, negative error code otherwise.
 */
int32_t no_os_socket_send(struct no_os_socket_desc *desc, const void *data,
			  uint32_t len)
{
	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->send)
		return -ENOSYS;

	return desc->platform_ops->send(desc, data, len);
}

/**
 * @brief Receive data from a connected socket (non-blocking).
 * @param desc - The socket descriptor.
 * @param data - Destination buffer for received data.
 * @param len - Maximum number of bytes to read.
 * @return Number of bytes received on success, -EAGAIN if no data available,
 *         negative error code otherwise.
 */
int32_t no_os_socket_recv(struct no_os_socket_desc *desc, void *data,
			  uint32_t len)
{
	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->recv)
		return -ENOSYS;

	return desc->platform_ops->recv(desc, data, len);
}

/**
 * @brief Send a datagram to a specific address (UDP).
 * @param desc - The socket descriptor.
 * @param data - Buffer containing data to send.
 * @param len - Number of bytes to send.
 * @param to - Destination address.
 * @return Number of bytes sent on success, negative error code otherwise.
 */
int32_t no_os_socket_sendto(struct no_os_socket_desc *desc, const void *data,
			    uint32_t len,
			    const struct no_os_socket_address *to)
{
	if (!desc || !desc->platform_ops || !data || !to)
		return -EINVAL;

	if (!desc->platform_ops->sendto)
		return -ENOSYS;

	return desc->platform_ops->sendto(desc, data, len, to);
}

/**
 * @brief Receive a datagram and the sender's address (UDP).
 * @param desc - The socket descriptor.
 * @param data - Destination buffer for received data.
 * @param len - Maximum number of bytes to read.
 * @param from - Filled with the sender's address (may be NULL).
 * @return Number of bytes received on success, -EAGAIN if no data available,
 *         negative error code otherwise.
 */
int32_t no_os_socket_recvfrom(struct no_os_socket_desc *desc, void *data,
			      uint32_t len,
			      struct no_os_socket_address *from)
{
	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->recvfrom)
		return -ENOSYS;

	return desc->platform_ops->recvfrom(desc, data, len, from);
}

/**
 * @brief Bind a socket to a local port.
 * @param desc - The socket descriptor.
 * @param port - Local port number.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_bind(struct no_os_socket_desc *desc, uint16_t port)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->bind)
		return -ENOSYS;

	return desc->platform_ops->bind(desc, port);
}

/**
 * @brief Start listening for incoming connections.
 * @param desc - The socket descriptor (must be bound first).
 * @param back_log - Maximum number of pending connections.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_socket_listen(struct no_os_socket_desc *desc, uint32_t back_log)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->listen)
		return -ENOSYS;

	return desc->platform_ops->listen(desc, back_log);
}

/**
 * @brief Accept an incoming connection.
 * @param desc - The listening socket descriptor.
 * @param new_client - Address where the new client descriptor is stored.
 * @return 0 in case of success, -EAGAIN if no pending connection,
 *         negative error code otherwise.
 */
int32_t no_os_socket_accept(struct no_os_socket_desc *desc,
			    struct no_os_socket_desc **new_client)
{
	int32_t ret;

	if (!desc || !desc->platform_ops || !new_client)
		return -EINVAL;

	if (!desc->platform_ops->accept)
		return -ENOSYS;

	ret = desc->platform_ops->accept(desc, new_client);
	if (ret)
		return ret;

	(*new_client)->platform_ops = desc->platform_ops;
	(*new_client)->net_desc = desc->net_desc;

	return 0;
}
