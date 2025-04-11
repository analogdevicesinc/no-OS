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
#include "tcp_socket.h"

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
static uint8_t w5500_net_translate_protocol(enum socket_protocol proto)
{
	switch (proto) {
	case PROTOCOL_TCP:
		return W5500_Sn_MR_TCP;
	case PROTOCOL_UDP:
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
static int w5500_net_addr_net_to_w5500(const struct socket_address *src,
				       struct w5500_socket_address *dst)
{
	int result;

	if (!src || !src->addr || !dst)
		return -EINVAL;

	result = sscanf(src->addr, "%hhu.%hhu.%hhu.%hhu",
			&dst->ip[0], &dst->ip[1], &dst->ip[2], &dst->ip[3]);

	if (result != 4)
		return -EINVAL;

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
static int w5500_net_addr_w5500_to_net(struct w5500_socket_address *src,
				       struct socket_address *dst)
{
	static char ip_str[16];

	if (!src || !dst)
		return -EINVAL;

	snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
		 src->ip[0], src->ip[1], src->ip[2], src->ip[3]);

	dst->addr = ip_str;
	dst->port = src->port[0] << 8 | src->port[1];

	return 0;
}

/***************************************************************************//**
 * @brief Open a socket through the network interface
 *
 * @param net       - The network interface
 * @param sock_id   - Pointer to store the assigned socket ID
 * @param proto     - Protocol to use (TCP, UDP)
 * @param buff_size - Buffer size for the socket
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_open(void *net, uint32_t *sock_id,
				     enum socket_protocol proto,
				     uint32_t buff_size)
{
	struct w5500_network_dev *net_dev = net;
	uint8_t buff_kb;
	uint8_t physical_id;
	struct w5500_socket_map *socket;
	uint8_t w5500_protocol;
	int ret;

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

	w5500_protocol = w5500_net_translate_protocol(proto);

	ret = w5500_socket_open(net_dev->mac_dev, physical_id, w5500_protocol, buff_kb);
	if (ret)
		return ret;

	socket = &net_dev->sockets[physical_id];

	socket->in_use = 1;
	socket->sock_id = net_dev->next_virtual_id;

	net_dev->next_virtual_id++;

	*sock_id = socket->sock_id;

	return 0;
}

/***************************************************************************//**
 * @brief Close a socket
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to close
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_close(void *net, uint32_t sock_id)
{
	struct w5500_network_dev *net_dev = net;
	struct w5500_socket_map *socket;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_close(net_dev->mac_dev, physical_id);
	if (ret) {
		return ret;
	}

	socket = &net_dev->sockets[physical_id];
	socket->in_use = 0;
	socket->role = W5500_ROLE_UNUSED;

	return 0;

}

/***************************************************************************//**
 * @brief Connect a socket to a remote host
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to connect
 * @param addr    - The remote address to connect to
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_connect(void *net, uint32_t sock_id,
					struct socket_address *addr)
{
	struct w5500_network_dev *net_dev = net;
	struct w5500_socket_address w5500_addr;
	uint8_t physical_id;
	int ret;

	if (!addr || !addr->addr)
		return -EINVAL;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	ret = w5500_net_addr_net_to_w5500(addr, &w5500_addr);
	if (ret)
		return ret;

	return w5500_socket_connect(net_dev->mac_dev, physical_id, &w5500_addr);
}

/***************************************************************************//**
 * @brief Disconnect a connected socket
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to disconnect
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_disconnect(void *net, uint32_t sock_id)
{
	struct w5500_network_dev *net_dev = net;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	return w5500_socket_disconnect(net_dev->mac_dev, physical_id);
}

/***************************************************************************//**
 * @brief Send data through a socket
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to send data through
 * @param data    - Pointer to the data buffer to send
 * @param size    - Size of the data to send
 *
 * @return Number of bytes sent on success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_send(void *net, uint32_t sock_id,
				     const void *data, uint32_t size)
{
	struct w5500_network_dev *net_dev = net;
	uint8_t physical_id;
	int ret;

	if (!data)
		return -EINVAL;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	return w5500_socket_send(net_dev->mac_dev, physical_id, data, size);
}

/***************************************************************************//**
 * @brief Receive data from a socket
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to receive data from
 * @param data    - Buffer to store received data
 * @param size    - Maximum size of data to receive
 *
 * @return Number of bytes received on success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_recv(void *net, uint32_t sock_id,
				     void *data, uint32_t size)
{
	struct w5500_network_dev *net_dev = net;
	uint8_t physical_id;
	int ret;

	if (!data)
		return -EINVAL;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	return w5500_socket_recv(net_dev->mac_dev, physical_id, data, size);
}

/***************************************************************************//**
 * @brief Send data to a specific destination (UDP)
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to send data through
 * @param data    - Pointer to the data buffer to send
 * @param size    - Size of the data to send
 * @param to      - Destination address information
 *
 * @return Number of bytes sent on success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_sendto(void *net, uint32_t sock_id,
				       const void *data, uint32_t size,
				       const struct socket_address *to)
{
	struct w5500_network_dev *net_dev = net;
	struct w5500_socket_address w5500_addr;
	uint8_t physical_id;
	int ret;

	if (!data || !to || !to->addr)
		return -EINVAL;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	ret = w5500_net_addr_net_to_w5500(to, &w5500_addr);
	if (ret)
		return ret;

	return w5500_socket_sendto(net_dev->mac_dev, physical_id, data, size,
				   &w5500_addr);
}

/***************************************************************************//**
 * @brief Receive data from a socket with source address information (UDP)
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to receive data from
 * @param data    - Buffer to store received data
 * @param size    - Maximum size of data to receive
 * @param from    - Buffer to store source address information
 *
 * @return Number of bytes received on success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_recvfrom(void *net, uint32_t sock_id,
		void *data, uint32_t size,
		struct socket_address *from)
{
	struct w5500_network_dev *net_dev = net;
	uint8_t physical_id;
	int ret;
	struct w5500_socket_address w5500_from;

	if (!data)
		return -EINVAL;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_recvfrom(net_dev->mac_dev, physical_id, data, size,
				    &w5500_from);
	if (ret < 0)
		return ret;

	if (from) {
		ret = w5500_net_addr_w5500_to_net(&w5500_from, from);
		if (ret)
			return ret;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Bind a socket to a specific port
 *
 * @param net     - The network interface
 * @param sock_id - The socket ID to bind
 * @param port    - The port number to bind to
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_bind(void *net, uint32_t sock_id, uint16_t port)
{
	struct w5500_network_dev *net_dev = net;
	struct w5500_socket_map *socket;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_bind(net_dev->mac_dev, physical_id, port);
	if (ret)
		return ret;

	socket = &net_dev->sockets[physical_id];
	socket->local_port = port;

	return 0;
}

/***************************************************************************//**
 * @brief Set a socket to listen mode (TCP server)
 *
 * @param net      - The network interface
 * @param sock_id  - The socket ID to set in listen mode
 * @param back_log - Maximum connection backlog (not used by W5500)
 *
 * @return 0 in case of success, negative error code otherwise
*******************************************************************************/
static int32_t w5500_net_socket_listen(void *net, uint32_t sock_id,
				       uint32_t back_log)
{
	struct w5500_network_dev *net_dev = net;
	struct w5500_socket_map *socket;
	uint8_t physical_id;
	int ret;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_listen(net_dev->mac_dev, physical_id);
	if (ret)
		return ret;

	socket = &net_dev->sockets[physical_id];
	socket->role = W5500_ROLE_SERVER;

	return 0;
}

/***************************************************************************//**
 * @brief Accept an incoming connection on a listening socket
 *
 * @param net            - The network interface
 * @param sock_id        - The listening socket ID
 * @param client_sock_id - Pointer to store the new client socket ID
 *
 * @return 0 in case of success, -EAGAIN if no connection pending, other negative error code otherwise
 *
 * @note W5500 Socket Behavior:
 * Unlike traditional socket implementations where accept() creates a new socket
 * for the client while the original socket continues listening, the W5500 hardware
 * changes the listening socket's state to ESTABLISHED when a connection is accepted.
 *
 * This implementation:
 * 1. Checks if the server socket has an established connection
 * 2. Creates a new socket to replace the original server socket (for future connections)
 * 3. Configures the new socket with the same parameters as the original
 * 4. Updates the socket mapping:
 *    - Original socket becomes a client socket (with the established connection)
 *    - New socket becomes the server socket (continues listening)
 * 5. Returns the original socket's new ID as the client socket ID
*******************************************************************************/
static int32_t w5500_net_socket_accept(void *net, uint32_t sock_id,
				       uint32_t *client_sock_id)
{
	struct w5500_network_dev *net_dev = net;
	struct w5500_socket_map *old_server_sock;
	struct w5500_socket_map *new_sock;
	uint8_t status;
	uint8_t physical_id;
	uint8_t new_physical_id;
	int ret;

	if (!client_sock_id)
		return -EINVAL;

	ret = w5500_net_map(net_dev, sock_id, &physical_id);
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

	/* Find and configure a new socket to take over server role */
	ret = w5500_net_find_free_socket(net_dev, &new_physical_id);
	if (ret)
		return ret;

	ret = w5500_socket_open(net_dev->mac_dev, new_physical_id, W5500_Sn_MR_TCP, 2);
	if (ret)
		return ret;

	ret = w5500_socket_bind(net_dev->mac_dev, new_physical_id,
				old_server_sock->local_port);
	if (ret)
		return ret;

	ret = w5500_socket_listen(net_dev->mac_dev, new_physical_id);
	if (ret)
		return ret;

	/* Configure new socket to take the place of the old server */
	new_sock = &net_dev->sockets[new_physical_id];
	new_sock->sock_id = sock_id;
	new_sock->local_port = old_server_sock->local_port;
	new_sock->in_use = 1;
	new_sock->role = W5500_ROLE_SERVER;

	/* Old server socket becomes new client socket */
	old_server_sock->sock_id = net_dev->next_virtual_id++;
	old_server_sock->role = W5500_ROLE_CLIENT;

	*client_sock_id = old_server_sock->sock_id;

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

#ifdef NO_OS_IP
	sscanf(NO_OS_IP, "%hhu.%hhu.%hhu.%hhu", &dev->ip[0], &dev->ip[1], &dev->ip[2],
	       &dev->ip[3]);
	ret = w5500_set_ip(dev->mac_dev, dev->ip);
	if (ret)
		goto free_mac_dev;
#endif

#ifdef NO_OS_NETMASK
	sscanf(NO_OS_NETMASK, "%hhu.%hhu.%hhu.%hhu", &dev->netmask[0], &dev->netmask[1],
	       &dev->netmask[2], &dev->netmask[3]);
	ret = w5500_set_subnet(dev->mac_dev, dev->netmask);
	if (ret)
		goto free_mac_dev;
#endif

#ifdef NO_OS_GATEWAY
	sscanf(NO_OS_GATEWAY, "%hhu.%hhu.%hhu.%hhu", &dev->gateway[0], &dev->gateway[1],
	       &dev->gateway[2], &dev->gateway[3]);
	ret = w5500_set_gateway(dev->mac_dev, dev->gateway);
	if (ret)
		goto free_mac_dev;
#endif

	dev->net_if.net = dev;

	dev->net_if.socket_open = w5500_net_socket_open;
	dev->net_if.socket_close = w5500_net_socket_close;
	dev->net_if.socket_connect = w5500_net_socket_connect;
	dev->net_if.socket_disconnect = w5500_net_socket_disconnect;
	dev->net_if.socket_send = w5500_net_socket_send;
	dev->net_if.socket_recv = w5500_net_socket_recv;
	dev->net_if.socket_sendto = w5500_net_socket_sendto;
	dev->net_if.socket_recvfrom = w5500_net_socket_recvfrom;
	dev->net_if.socket_bind = w5500_net_socket_bind;
	dev->net_if.socket_listen = w5500_net_socket_listen;
	dev->net_if.socket_accept = w5500_net_socket_accept;

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

#endif /* NO_OS_W5500_NETWORKING */
