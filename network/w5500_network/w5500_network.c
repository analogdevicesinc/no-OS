/***************************************************************************/ /**
*   @file   w5500_network.c
*   @brief  W5500 Network Interface Implementation
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifdef NO_OS_W5500_NETWORKING

#include "w5500_network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "no_os_alloc.h"

/***************************************************************************//**
 * @brief Initialize the socket mapping table
 *
 * @param dev - The W5500 network device descriptor
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int w5500_net_sockets_init(struct w5500_network_dev *dev)
{
	uint8_t i;

	for (i = 0; i <= W5500_MAX_SOCK_NUMBER; i++) {
		dev->sockets[i].in_use = 0;
		dev->sockets[i].physical_id = i;
		dev->sockets[i].local_port = 0;
		dev->sockets[i].remote_port = 0;
		memset(dev->sockets[i].remote_ip, 0, 4);
	}

	return 0;
}

/***************************************************************************//**
 * @brief Find an available socket in the socket mapping table
 *
 * @param dev         - The W5500 network device descriptor
 * @param physical_id - Pointer to store the found physical socket ID
 *
 * @return 0 in case of success, -ENOENT if no free socket found
*******************************************************************************/
static int w5500_net_find_free_socket(struct w5500_network_dev *dev,
				      uint8_t *physical_id)
{
	uint8_t i;

	for (i = 0; i <= W5500_MAX_SOCK_NUMBER; i++) {
		if (!dev->sockets[i].in_use) {
			*physical_id = i;
			return 0;
		}
	}
	return -ENOENT;
}

/***************************************************************************//**
 * @brief Map a virtual socket ID to its physical counterpart
 *
 * @param dev         - The W5500 network device descriptor
 * @param sock_id     - The virtual socket ID to map
 * @param physical_id - Pointer to store the mapped physical socket ID
 *
 * @return 0 in case of success, -ENOENT if mapping not found
*******************************************************************************/
static int w5500_net_map(struct w5500_network_dev *dev, uint32_t sock_id,
			 uint8_t *physical_id)
{
	uint8_t i;

	for (i = 0; i <= W5500_MAX_SOCK_NUMBER; i++) {
		if (dev->sockets[i].sock_id == sock_id) {
			*physical_id = dev->sockets[i].physical_id;
			return 0;
		}
	}
	return -ENOENT;
}

/***************************************************************************//**
 * @brief Convert network interface protocol to W5500 protocol
 *
 * @param proto - Network interface protocol
 *
 * @return W5500 protocol value
*******************************************************************************/
static uint8_t w5500_net_translate_protocol(enum no_os_socket_protocol proto)
{
	switch (proto) {
	case NO_OS_SOCKET_TCP:
		return W5500_Sn_MR_TCP;
	case NO_OS_SOCKET_UDP:
		return W5500_Sn_MR_UDP;
	default:
		return W5500_Sn_MR_CLOSE;
	}
}

/***************************************************************************//**
 * @brief Convert socket_address to w5500_socket_address
 *
 * @param src - Source socket address (network interface)
 * @param dst - Destination socket address (W5500 specific)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
static int w5500_net_addr_net_to_w5500(const struct no_os_sockaddr *src,
				       struct w5500_socket_address *dst)
{
	int ret;

	if (!src || !src->addr || !dst)
		return -EINVAL;

	ret = no_os_net_ipv4_to_bytes(src->addr, dst->ip);
	if (ret)
		return ret;

	dst->port[0] = W5500_BYTE_HIGH(src->port);
	dst->port[1] = W5500_BYTE_LOW(src->port);

	return 0;
}

/***************************************************************************//**
 * @brief Convert w5500_socket_address to socket_address
 *
 * @param src - Source socket address (W5500 specific)
 * @param dst - Destination socket address (network interface)
 *
 * @return 0 on success, negative error code otherwise
*******************************************************************************/
static int w5500_net_addr_w5500_to_net(struct w5500_network_dev *net_dev,
				       struct w5500_socket_address *src,
				       struct no_os_sockaddr *dst)
{
	int ret;

	if (!net_dev || !src || !dst)
		return -EINVAL;

	ret = no_os_net_ipv4_to_str(src->ip, net_dev->addr_str,
				    sizeof(net_dev->addr_str));
	if (ret)
		return ret;

	dst->addr = net_dev->addr_str;
	dst->port = src->port[0] << 8 | src->port[1];

	return 0;
}

/***************************************************************************//**
 * @brief Resolve a socket descriptor to its network device and physical id.
 *
 * @param sock        - The generic socket descriptor
 * @param net_dev     - Pointer to store the owning network device
 * @param physical_id - Pointer to store the mapped physical socket id
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int w5500_net_resolve(struct no_os_socket_desc *sock,
			     struct w5500_network_dev **net_dev,
			     uint8_t *physical_id)
{
	*net_dev = sock->net->extra;

	return w5500_net_map(*net_dev, sock->id, physical_id);
}

/***************************************************************************//**
 * @brief Close a socket identified by its virtual id and release its slot.
 *
 * Shared by the close op and the accept() error path.
 *
 * @param net_dev - The W5500 network device descriptor
 * @param sock_id - The virtual socket id to close
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_close_id(struct w5500_network_dev *net_dev,
			      uint32_t sock_id)
{
	struct w5500_socket_map *socket;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_close(net_dev->mac_dev, physical_id);
	if (ret)
		return ret;

	socket = &net_dev->sockets[physical_id];
	socket->in_use = 0;
	socket->role = W5500_ROLE_UNUSED;

	return 0;
}

/***************************************************************************//**
 * @brief Initialize the W5500 network interface
 *
 * @param net_dev    - Double pointer to store the created network device
 * @param init_param - Initialization parameters
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_network_init(struct w5500_network_dev **net_dev,
		       struct w5500_network_init_param *init_param)
{
	struct w5500_network_dev *dev;
	int32_t ret;

	dev = (struct w5500_network_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	if (init_param->mac_dev == NULL) {
		if (init_param->w5500_ip == NULL) {
			ret = -EINVAL;
			goto free_dev;
		}

		ret = w5500_init(&dev->mac_dev, init_param->w5500_ip);
		if (ret)
			goto free_dev;
	}

#ifdef CONFIG_NO_OS_IP
	sscanf(CONFIG_NO_OS_IP, "%hhu.%hhu.%hhu.%hhu", &dev->ip[0], &dev->ip[1],
	       &dev->ip[2],
	       &dev->ip[3]);
	ret = w5500_set_ip(dev->mac_dev, dev->ip);
	if (ret)
		goto free_mac_dev;
#endif

#ifdef CONFIG_NO_OS_NETMASK
	sscanf(CONFIG_NO_OS_NETMASK, "%hhu.%hhu.%hhu.%hhu", &dev->netmask[0],
	       &dev->netmask[1],
	       &dev->netmask[2], &dev->netmask[3]);
	ret = w5500_set_subnet(dev->mac_dev, dev->netmask);
	if (ret)
		goto free_mac_dev;
#endif

#ifdef CONFIG_NO_OS_GATEWAY
	sscanf(CONFIG_NO_OS_GATEWAY, "%hhu.%hhu.%hhu.%hhu", &dev->gateway[0],
	       &dev->gateway[1],
	       &dev->gateway[2], &dev->gateway[3]);
	ret = w5500_set_gateway(dev->mac_dev, dev->gateway);
	if (ret)
		goto free_mac_dev;
#endif

	ret = w5500_net_sockets_init(dev);
	if (ret)
		goto free_mac_dev;

	dev->next_virtual_id = W5500_MAX_SOCK_NUMBER + 1;

	*net_dev = dev;

	return 0;

free_mac_dev:
	no_os_free(dev->mac_dev);
free_dev:
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Remove a W5500 network device and free resources
 *
 * @param dev - The device descriptor to remove
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
int w5500_network_remove(struct w5500_network_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_free(dev);

	return 0;
}

/* ---- no_os_net / no_os_socket backend --------------------------------- */
/*
 * The W5500 backend keeps its virtual->physical socket map inside
 * w5500_network_dev (including the accept() hardware-socket swap). In the new
 * API, no_os_net_desc.extra holds the w5500_network_dev, and each
 * no_os_socket_desc wraps a virtual socket id in its "id" field. The chip runs
 * its own TCP/IP state machine, so step() is a no-op.
 */

#include "no_os_net.h"
#include "no_os_socket.h"

static const struct no_os_socket_ops w5500_socket_ops;

static int32_t w5500_net_init(struct no_os_net_desc **desc,
			      const struct no_os_net_init_param *param)
{
	struct no_os_net_desc *nd;
	struct w5500_network_dev *dev;
	int ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	nd = no_os_calloc(1, sizeof(*nd));
	if (!nd)
		return -ENOMEM;

	ret = w5500_network_init(&dev, param->extra);
	if (ret) {
		no_os_free(nd);
		return ret;
	}

	nd->extra = dev;
	*desc = nd;

	return 0;
}

static int32_t w5500_net_remove(struct no_os_net_desc *desc)
{
	int ret;

	ret = w5500_network_remove(desc->extra);
	no_os_free(desc);

	return ret;
}

static int32_t w5500_net_step(struct no_os_net_desc *desc)
{
	/* The W5500 runs its TCP/IP stack in hardware; nothing to pump. */
	return 0;
}

static int32_t w5500_net_get_ip(struct no_os_net_desc *desc, char *buf,
				uint32_t size)
{
	struct w5500_network_dev *dev = desc->extra;
	uint8_t ip[4];
	int ret;

	ret = w5500_get_ip(dev->mac_dev, ip);
	if (ret)
		return ret;

	return no_os_net_ipv4_to_str(ip, buf, size);
}

/**
 * @brief Open a socket and wrap it in a generic socket descriptor.
 *
 * The buffer size is rounded up to the nearest W5500 socket buffer bank size
 * (in KiB, capped at 16).
 */
static int32_t w5500_net_open(struct no_os_net_desc *net,
			      struct no_os_socket_desc **sock,
			      enum no_os_socket_protocol proto,
			      uint32_t buff_size)
{
	struct w5500_network_dev *net_dev = net->extra;
	struct w5500_socket_map *socket;
	struct no_os_socket_desc *s;
	uint8_t physical_id;
	uint8_t buff_kb;
	int32_t ret;

	if (buff_size <= 1024)
		buff_kb = 1;
	else if (buff_size <= 2048)
		buff_kb = 2;
	else if (buff_size <= 4096)
		buff_kb = 4;
	else if (buff_size <= 8192)
		buff_kb = 8;
	else
		buff_kb = 16;

	ret = w5500_net_find_free_socket(net_dev, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_open(net_dev->mac_dev, physical_id,
				w5500_net_translate_protocol(proto), buff_kb);
	if (ret)
		return ret;

	socket = &net_dev->sockets[physical_id];
	socket->in_use = 1;
	socket->sock_id = net_dev->next_virtual_id++;

	s = no_os_socket_alloc(net, &w5500_socket_ops, socket->sock_id);
	if (!s) {
		w5500_close_id(net_dev, socket->sock_id);
		return -ENOMEM;
	}

	*sock = s;

	return 0;
}

static int32_t w5500_sock_close(struct no_os_socket_desc *sock)
{
	int32_t ret;

	ret = w5500_close_id(sock->net->extra, sock->id);
	no_os_free(sock);

	return ret;
}

static int32_t w5500_sock_connect(struct no_os_socket_desc *sock,
				  struct no_os_sockaddr *addr)
{
	struct w5500_network_dev *net_dev;
	struct w5500_socket_address w5500_addr;
	uint8_t physical_id;
	int ret;

	if (!addr || !addr->addr)
		return -EINVAL;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	ret = w5500_net_addr_net_to_w5500(addr, &w5500_addr);
	if (ret)
		return ret;

	return w5500_socket_connect(net_dev->mac_dev, physical_id, &w5500_addr);
}

static int32_t w5500_sock_disconnect(struct no_os_socket_desc *sock)
{
	struct w5500_network_dev *net_dev;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	return w5500_socket_disconnect(net_dev->mac_dev, physical_id);
}

static int32_t w5500_sock_send(struct no_os_socket_desc *sock, const void *data,
			       uint32_t size)
{
	struct w5500_network_dev *net_dev;
	uint8_t physical_id;
	int ret;

	if (!data)
		return -EINVAL;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	return w5500_socket_send(net_dev->mac_dev, physical_id, data, size);
}

static int32_t w5500_sock_recv(struct no_os_socket_desc *sock, void *data,
			       uint32_t size)
{
	struct w5500_network_dev *net_dev;
	uint8_t physical_id;
	int ret;

	if (!data)
		return -EINVAL;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	return w5500_socket_recv(net_dev->mac_dev, physical_id, data, size);
}

static int32_t w5500_sock_sendto(struct no_os_socket_desc *sock,
				 const void *data, uint32_t size,
				 const struct no_os_sockaddr *to)
{
	struct w5500_network_dev *net_dev;
	struct w5500_socket_address w5500_addr;
	uint8_t physical_id;
	int ret;

	if (!data || !to || !to->addr)
		return -EINVAL;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	ret = w5500_net_addr_net_to_w5500(to, &w5500_addr);
	if (ret)
		return ret;

	return w5500_socket_sendto(net_dev->mac_dev, physical_id, data, size,
				   &w5500_addr);
}

static int32_t w5500_sock_recvfrom(struct no_os_socket_desc *sock, void *data,
				   uint32_t size, struct no_os_sockaddr *from)
{
	struct w5500_network_dev *net_dev;
	struct w5500_socket_address w5500_from;
	uint8_t physical_id;
	int ret;

	if (!data)
		return -EINVAL;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_recvfrom(net_dev->mac_dev, physical_id, data, size,
				    &w5500_from);
	if (ret < 0)
		return ret;

	if (from)
		return w5500_net_addr_w5500_to_net(net_dev, &w5500_from, from) ?
		       -EINVAL : ret;

	return ret;
}

static int32_t w5500_sock_bind(struct no_os_socket_desc *sock, uint16_t port)
{
	struct w5500_network_dev *net_dev;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_bind(net_dev->mac_dev, physical_id, port);
	if (ret)
		return ret;

	net_dev->sockets[physical_id].local_port = port;

	return 0;
}

static int32_t w5500_sock_listen(struct no_os_socket_desc *sock,
				 uint32_t backlog)
{
	struct w5500_network_dev *net_dev;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_listen(net_dev->mac_dev, physical_id);
	if (ret)
		return ret;

	net_dev->sockets[physical_id].role = W5500_ROLE_SERVER;

	return 0;
}

/**
 * @brief Accept an incoming connection on a listening socket.
 *
 * @note W5500 Socket Behavior:
 * Unlike traditional socket implementations where accept() creates a new socket
 * for the client while the original socket continues listening, the W5500
 * hardware changes the listening socket's state to ESTABLISHED when a
 * connection is accepted. This implementation therefore hands the established
 * (original) hardware socket to the client and spins up a fresh hardware socket
 * to keep listening in its place.
 *
 * @return 0 on success, -EAGAIN if no connection is pending, negative error
 *         code otherwise.
 */
static int32_t w5500_sock_accept(struct no_os_socket_desc *sock,
				 struct no_os_socket_desc **client)
{
	struct w5500_network_dev *net_dev;
	struct w5500_socket_map *old_server_sock;
	struct w5500_socket_map *new_sock;
	struct no_os_socket_desc *c;
	uint8_t status;
	uint8_t physical_id;
	uint8_t new_physical_id;
	int ret;

	ret = w5500_net_resolve(sock, &net_dev, &physical_id);
	if (ret)
		return ret;

	old_server_sock = &net_dev->sockets[physical_id];

	if (old_server_sock->role != W5500_ROLE_SERVER)
		return -EINVAL;

	ret = w5500_socket_read_status(net_dev->mac_dev, physical_id, &status);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_ESTABLISHED)
		return -EAGAIN;

	/* Find and configure a new socket to take over the server role */
	ret = w5500_net_find_free_socket(net_dev, &new_physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_open(net_dev->mac_dev, new_physical_id,
				W5500_Sn_MR_TCP, 2);
	if (ret)
		return ret;

	ret = w5500_socket_bind(net_dev->mac_dev, new_physical_id,
				old_server_sock->local_port);
	if (ret)
		return ret;

	ret = w5500_socket_listen(net_dev->mac_dev, new_physical_id);
	if (ret)
		return ret;

	/* New socket takes the place of the old server */
	new_sock = &net_dev->sockets[new_physical_id];
	new_sock->sock_id = sock->id;
	new_sock->local_port = old_server_sock->local_port;
	new_sock->in_use = 1;
	new_sock->role = W5500_ROLE_SERVER;

	/* Old server socket becomes the new client socket */
	old_server_sock->sock_id = net_dev->next_virtual_id++;
	old_server_sock->role = W5500_ROLE_CLIENT;

	c = no_os_socket_alloc(sock->net, &w5500_socket_ops,
			       old_server_sock->sock_id);
	if (!c) {
		w5500_close_id(net_dev, old_server_sock->sock_id);
		return -ENOMEM;
	}

	*client = c;

	return 0;
}

const struct no_os_net_platform_ops w5500_net_platform_ops = {
	.init = w5500_net_init,
	.remove = w5500_net_remove,
	.step = w5500_net_step,
	.get_ip = w5500_net_get_ip,
	.socket_open = w5500_net_open,
};

static const struct no_os_socket_ops w5500_socket_ops = {
	.close = w5500_sock_close,
	.connect = w5500_sock_connect,
	.disconnect = w5500_sock_disconnect,
	.send = w5500_sock_send,
	.recv = w5500_sock_recv,
	.sendto = w5500_sock_sendto,
	.recvfrom = w5500_sock_recvfrom,
	.bind = w5500_sock_bind,
	.listen = w5500_sock_listen,
	.accept = w5500_sock_accept,
};

#endif /* NO_OS_W5500_NETWORKING */
