/***************************************************************************//**
 *   @file   w5500_net.c
 *   @brief  W5500 adapter for the no_os_net / no_os_socket interfaces.
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

#ifdef NO_OS_NET

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

#include "w5500_net.h"
#include "no_os_net.h"
#include "no_os_socket.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_delay.h"

static const struct no_os_socket_ops w5500_socket_ops;

/**
 * @brief Get a socket pool slot based on its physical id.
 * @param wd - W5500 backend descriptor.
 * @param id - index in the sockets array.
 * @return a reference to the socket slot, NULL in case of an error.
 */
static struct w5500_socket_desc *_get_sock(struct w5500_net_desc *wd,
		uint32_t id)
{
	if (id > W5500_MAX_SOCK_NUMBER)
		return NULL;

	return &wd->sockets[id];
}

/**
 * @brief Get the id of a socket in the W5500_SOCK_CLOSED state (ready to use).
 * @param wd - W5500 backend descriptor.
 * @param id - index in the sockets array.
 * @return 0 on success, -ECONNREFUSED if all the connections are in use.
 */
static int32_t _get_closed_socket(struct w5500_net_desc *wd, uint32_t *id)
{
	uint32_t i;

	for (i = 0; i <= W5500_MAX_SOCK_NUMBER; i++)
		if (wd->sockets[i].state == W5500_SOCK_CLOSED) {
			*id = i;

			return 0;
		}

	/* All the available connections are used */
	return -ECONNREFUSED;
}

/**
 * @brief Map a no_os_socket_protocol to the W5500 socket mode register value.
 * @param proto - transport protocol.
 * @return W5500_Sn_MR_TCP / W5500_Sn_MR_UDP, or W5500_Sn_MR_CLOSE if unknown.
 */
static uint8_t _proto_to_w5500(enum no_os_socket_protocol proto)
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

/**
 * @brief Convert a no_os_sockaddr to a w5500_socket_address.
 * @param src - source address (numeric string + port).
 * @param dst - destination W5500 address (raw bytes).
 * @return 0 on success, negative error code otherwise.
 */
static int32_t _addr_net_to_w5500(const struct no_os_sockaddr *src,
				  struct w5500_socket_address *dst)
{
	int32_t ret;

	if (!src || !src->addr || !dst)
		return -EINVAL;

	ret = no_os_net_ipv4_to_bytes(src->addr, dst->ip);
	if (ret)
		return ret;

	dst->port[0] = W5500_BYTE_HIGH(src->port);
	dst->port[1] = W5500_BYTE_LOW(src->port);

	return 0;
}

/**
 * @brief Convert a w5500_socket_address to a no_os_sockaddr.
 * @param src - source W5500 address (raw bytes).
 * @param dst - destination address; dst->addr must point to a buffer.
 * @param size - size of the dst->addr buffer.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t _addr_w5500_to_net(const struct w5500_socket_address *src,
				  struct no_os_sockaddr *dst, uint32_t size)
{
	int32_t ret;

	if (!src || !dst || !dst->addr)
		return -EINVAL;

	ret = no_os_net_ipv4_to_str(src->ip, dst->addr, size);
	if (ret)
		return ret;

	dst->port = (uint16_t)src->port[0] << 8 | src->port[1];

	return 0;
}

/**
 * @brief Configure the W5500 and the network interface.
 * @param desc - the network interface descriptor to be returned.
 * @param param - initialization parameter (extra holds a w5500_net_param).
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_net_init(struct no_os_net_desc **desc,
			      const struct no_os_net_init_param *param)
{
	struct w5500_net_param *w5500_param;
	struct no_os_net_desc *nd;
	struct w5500_net_desc *wd;
	int32_t ret;
	uint32_t i;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	w5500_param = param->extra;
	if (!w5500_param->mac_dev && !w5500_param->w5500_ip)
		return -EINVAL;

	nd = no_os_calloc(1, sizeof(*nd));
	if (!nd)
		return -ENOMEM;

	wd = no_os_calloc(1, sizeof(*wd));
	if (!wd) {
		ret = -ENOMEM;
		goto free_nd;
	}

	if (w5500_param->mac_dev) {
		wd->mac_dev = w5500_param->mac_dev;
		wd->mac_owned = false;
	} else {
		ret = w5500_init(&wd->mac_dev, w5500_param->w5500_ip);
		if (ret)
			goto free_wd;
		wd->mac_owned = true;
	}

#ifdef CONFIG_NO_OS_IP
	ret = no_os_net_ipv4_to_bytes(CONFIG_NO_OS_IP, wd->ip);
	if (ret)
		goto free_mac;
	ret = w5500_set_ip(wd->mac_dev, wd->ip);
	if (ret)
		goto free_mac;
#endif
#ifdef CONFIG_NO_OS_NETMASK
	ret = no_os_net_ipv4_to_bytes(CONFIG_NO_OS_NETMASK, wd->netmask);
	if (ret)
		goto free_mac;
	ret = w5500_set_subnet(wd->mac_dev, wd->netmask);
	if (ret)
		goto free_mac;
#endif
#ifdef CONFIG_NO_OS_GATEWAY
	ret = no_os_net_ipv4_to_bytes(CONFIG_NO_OS_GATEWAY, wd->gateway);
	if (ret)
		goto free_mac;
	ret = w5500_set_gateway(wd->mac_dev, wd->gateway);
	if (ret)
		goto free_mac;
#endif

	for (i = 0; i <= W5500_MAX_SOCK_NUMBER; i++) {
		wd->sockets[i].state = W5500_SOCK_CLOSED;
		wd->sockets[i].net = wd;
		wd->sockets[i].id = i;
	}

	nd->extra = wd;
	*desc = nd;

	return 0;

#if defined(CONFIG_NO_OS_IP) || defined(CONFIG_NO_OS_NETMASK) || \
	defined(CONFIG_NO_OS_GATEWAY)
free_mac:
	if (wd->mac_owned)
		w5500_remove(wd->mac_dev);
#endif
free_wd:
	no_os_free(wd);
free_nd:
	no_os_free(nd);

	return ret;
}

/**
 * @brief Free the resources allocated by w5500_net_init().
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_net_remove(struct no_os_net_desc *desc)
{
	struct w5500_net_desc *wd;
	uint32_t i;

	if (!desc || !desc->extra)
		return -EINVAL;

	wd = desc->extra;

	for (i = 0; i <= W5500_MAX_SOCK_NUMBER; i++)
		if (wd->sockets[i].state != W5500_SOCK_CLOSED)
			w5500_socket_close(wd->mac_dev, i);

	if (wd->mac_owned)
		w5500_remove(wd->mac_dev);

	no_os_free(wd);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Service the network interface.
 *
 * The W5500 runs its whole TCP/IP stack in hardware, so there are no software
 * timers to drive; this is a no-op kept for interface symmetry (iio calls it
 * unconditionally from its poll loop).
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_net_step(struct no_os_net_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	return 0;
}

/**
 * @brief Get the current IP address as a dotted-decimal string.
 * @param desc - the network interface descriptor.
 * @param buf - destination buffer.
 * @param size - size of the destination buffer.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_net_get_ip(struct no_os_net_desc *desc, char *buf,
				uint32_t size)
{
	struct w5500_net_desc *wd;
	uint8_t ip[4];
	int32_t ret;

	if (!desc || !desc->extra || !buf)
		return -EINVAL;

	wd = desc->extra;

	ret = w5500_get_ip(wd->mac_dev, ip);
	if (ret)
		return ret;

	return no_os_net_ipv4_to_str(ip, buf, size);
}

/**
 * @brief Open (create) a socket on the network interface.
 * @param net - the owning network interface descriptor.
 * @param sock - destination for the newly created socket descriptor.
 * @param proto - Layer 4 protocol (TCP or UDP).
 * @param buff_size - requested socket buffer size in bytes (0 = default 2KB).
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_net_socket_open(struct no_os_net_desc *net,
				     struct no_os_socket_desc **sock,
				     enum no_os_socket_protocol proto,
				     uint32_t buff_size)
{
	struct w5500_net_desc *wd;
	struct w5500_socket_desc *slot;
	struct no_os_socket_desc *s;
	uint8_t w5500_proto;
	uint8_t buff_kb;
	uint32_t id;
	int32_t ret;

	if (!net || !net->extra || !sock)
		return -EINVAL;

	w5500_proto = _proto_to_w5500(proto);
	if (w5500_proto == W5500_Sn_MR_CLOSE)
		return -EPROTONOSUPPORT;

	wd = net->extra;

	ret = _get_closed_socket(wd, &id);
	if (ret)
		return ret;

	if (buff_size == 0 || buff_size <= 2048)
		buff_kb = 2;
	else if (buff_size <= 4096)
		buff_kb = 4;
	else if (buff_size <= 8192)
		buff_kb = 8;
	else
		buff_kb = 16;

	ret = w5500_socket_open(wd->mac_dev, id, w5500_proto, buff_kb);
	if (ret)
		return ret;

	slot = &wd->sockets[id];
	slot->id = id;
	slot->net = wd;
	slot->proto = proto;
	slot->buff_kb = buff_kb;
	slot->local_port = 0;
	/* UDP is connectionless: the socket is immediately usable. */
	slot->state = (proto == NO_OS_SOCKET_UDP) ? W5500_SOCK_CONNECTED
		      : W5500_SOCK_CLOSED;

	s = no_os_socket_alloc(net, &w5500_socket_ops, id);
	if (!s) {
		w5500_socket_close(wd->mac_dev, id);
		slot->state = W5500_SOCK_CLOSED;
		return -ENOMEM;
	}
	s->extra = slot;

	*sock = s;

	return 0;
}

/**
 * @brief Close a socket connection and free its descriptor.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_sock_close(struct no_os_socket_desc *sock)
{
	struct w5500_socket_desc *slot;
	int32_t ret;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;

	ret = w5500_socket_close(slot->net->mac_dev, slot->id);
	slot->state = W5500_SOCK_CLOSED;
	no_os_free(sock);

	return ret;
}

/**
 * @brief Connect to a remote listening socket.
 *
 * The W5500 driver only issues the CONNECT command; the new socket interface
 * has no connection-status query, so the handshake is completed synchronously
 * here by polling the socket status register.
 * @param sock - the socket descriptor.
 * @param addr - IP/port of the remote socket.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_sock_connect(struct no_os_socket_desc *sock,
				  struct no_os_sockaddr *addr)
{
	struct w5500_socket_desc *slot;
	struct w5500_socket_address w5500_addr;
	uint32_t elapsed;
	uint8_t status;
	int32_t ret;

	if (!sock || !sock->extra || !addr)
		return -EINVAL;

	slot = sock->extra;

	ret = _addr_net_to_w5500(addr, &w5500_addr);
	if (ret)
		return ret;

	ret = w5500_socket_connect(slot->net->mac_dev, slot->id, &w5500_addr);
	if (ret)
		return ret;

	for (elapsed = 0; elapsed < W5500_NET_CONNECT_TIMEOUT_MS; elapsed++) {
		ret = w5500_socket_read_status(slot->net->mac_dev, slot->id,
					       &status);
		if (ret)
			return ret;

		if (status == W5500_Sn_SR_ESTABLISHED) {
			slot->state = W5500_SOCK_CONNECTED;
			return 0;
		}

		if (status == W5500_Sn_SR_CLOSED)
			return -ECONNREFUSED;

		no_os_mdelay(1);
	}

	return -ETIMEDOUT;
}

/**
 * @brief Close a connection from the client side.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_sock_disconnect(struct no_os_socket_desc *sock)
{
	struct w5500_socket_desc *slot;
	int32_t ret;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;

	ret = w5500_socket_disconnect(slot->net->mac_dev, slot->id);
	slot->state = W5500_SOCK_CLOSED;

	return ret;
}

/**
 * @brief Send data over a connected socket.
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of the data array.
 * @return Number of bytes sent, negative error code otherwise.
 */
static int32_t w5500_sock_send(struct no_os_socket_desc *sock, const void *data,
			       uint32_t size)
{
	struct w5500_socket_desc *slot;

	if (!sock || !sock->extra || !data)
		return -EINVAL;

	slot = sock->extra;

	if (slot->state != W5500_SOCK_CONNECTED)
		return -ENOTCONN;

	return w5500_socket_send(slot->net->mac_dev, slot->id, data, size);
}

/**
 * @brief Receive data over a connected socket (non-blocking).
 *
 * The underlying driver busy-waits on an empty RX FIFO. To fit the polled,
 * non-blocking model expected by iiod and the TLS BIO, the RX received-size
 * register is checked first: -EAGAIN is returned when connected but no data is
 * pending, and the driver read is clamped to the available bytes so its wait
 * loop never actually blocks.
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of data to be read.
 * @return Number of bytes received, -EAGAIN if none pending, error otherwise.
 */
static int32_t w5500_sock_recv(struct no_os_socket_desc *sock, void *data,
			       uint32_t size)
{
	struct w5500_socket_desc *slot;
	uint16_t rx_size = 0;
	uint8_t status;
	int32_t ret;

	if (!sock || !sock->extra || !data)
		return -EINVAL;

	slot = sock->extra;

	if (slot->state != W5500_SOCK_CONNECTED)
		return -ENOTCONN;

	ret = w5500_read_16bit_reg(slot->net->mac_dev,
				   W5500_SOCKET_REG_BLOCK(slot->id),
				   W5500_Sn_RX_RSR, &rx_size);
	if (ret)
		return ret;

	if (rx_size == 0) {
		ret = w5500_socket_read_status(slot->net->mac_dev, slot->id,
					       &status);
		if (ret)
			return ret;

		if (status != W5500_Sn_SR_ESTABLISHED &&
		    status != W5500_Sn_SR_UDP) {
			slot->state = W5500_SOCK_CLOSED;
			return -ENOTCONN;
		}

		return -EAGAIN;
	}

	return w5500_socket_recv(slot->net->mac_dev, slot->id, data,
				 no_os_min(size, rx_size));
}

/**
 * @brief Send a datagram to a remote host (UDP).
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of the data array.
 * @param to - destination address.
 * @return Number of bytes sent, negative error code otherwise.
 */
static int32_t w5500_sock_sendto(struct no_os_socket_desc *sock,
				 const void *data, uint32_t size,
				 const struct no_os_sockaddr *to)
{
	struct w5500_socket_desc *slot;
	struct w5500_socket_address w5500_addr;
	int32_t ret;

	if (!sock || !sock->extra || !data || !to)
		return -EINVAL;

	slot = sock->extra;

	ret = _addr_net_to_w5500(to, &w5500_addr);
	if (ret)
		return ret;

	ret = w5500_socket_sendto(slot->net->mac_dev, slot->id, data, size,
				  &w5500_addr);
	if (ret)
		return ret;

	return size;
}

/**
 * @brief Receive a datagram, optionally reporting the source (UDP).
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of data to be read.
 * @param from - destination for the source address, or NULL.
 * @return Number of bytes received, -EAGAIN if none pending, error otherwise.
 */
static int32_t w5500_sock_recvfrom(struct no_os_socket_desc *sock, void *data,
				   uint32_t size, struct no_os_sockaddr *from)
{
	struct w5500_socket_desc *slot;
	struct w5500_socket_address w5500_from;
	uint16_t rx_size = 0;
	int32_t ret;

	if (!sock || !sock->extra || !data)
		return -EINVAL;

	slot = sock->extra;

	ret = w5500_read_16bit_reg(slot->net->mac_dev,
				   W5500_SOCKET_REG_BLOCK(slot->id),
				   W5500_Sn_RX_RSR, &rx_size);
	if (ret)
		return ret;

	/* A UDP packet carries an 8-byte W5500 header ahead of the payload. */
	if (rx_size < 8)
		return -EAGAIN;

	ret = w5500_socket_recvfrom(slot->net->mac_dev, slot->id, data, size,
				    &w5500_from);
	if (ret < 0)
		return ret;

	if (from && from->addr)
		_addr_w5500_to_net(&w5500_from, from, 16);

	return ret;
}

/**
 * @brief Bind a socket to a local port.
 * @param sock - the socket descriptor.
 * @param port - port number to bind the socket to.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_sock_bind(struct no_os_socket_desc *sock, uint16_t port)
{
	struct w5500_socket_desc *slot;
	int32_t ret;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;

	ret = w5500_socket_bind(slot->net->mac_dev, slot->id, port);
	if (ret)
		return ret;

	slot->local_port = port;

	return 0;
}

/**
 * @brief Configure a socket to listen for connections (TCP server).
 * @param sock - the socket descriptor.
 * @param backlog - unused (the W5500 accepts one connection per socket).
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t w5500_sock_listen(struct no_os_socket_desc *sock,
				 uint32_t backlog)
{
	struct w5500_socket_desc *slot;
	int32_t ret;

	NO_OS_UNUSED_PARAM(backlog);

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;

	ret = w5500_socket_listen(slot->net->mac_dev, slot->id);
	if (ret)
		return ret;

	slot->state = W5500_SOCK_LISTENING;

	return 0;
}

/**
 * @brief Accept an incoming connection, returning a new socket.
 *
 * The W5500 has no separate listen/accept sockets: when a client connects, the
 * listening socket itself transitions to ESTABLISHED. To preserve the BSD model
 * (the server keeps listening and a fresh socket represents the client), a new
 * hardware socket is opened and set listening on the same local port to become
 * the new listener, while the original (now-connected) socket is handed back as
 * the client.
 * @param sock - the listening socket descriptor.
 * @param client - destination for the newly accepted socket descriptor.
 * @return 0 on success, -EAGAIN if no pending connection, error otherwise.
 */
static int32_t w5500_sock_accept(struct no_os_socket_desc *sock,
				 struct no_os_socket_desc **client)
{
	struct w5500_socket_desc *serv_slot;
	struct w5500_socket_desc *new_slot;
	struct no_os_socket_desc *cli;
	struct w5500_net_desc *wd;
	uint8_t status;
	uint32_t new_id;
	int32_t ret;

	if (!sock || !sock->extra || !client)
		return -EINVAL;

	serv_slot = sock->extra;
	wd = serv_slot->net;

	if (serv_slot->state != W5500_SOCK_LISTENING)
		return -EINVAL;

	ret = w5500_socket_read_status(wd->mac_dev, serv_slot->id, &status);
	if (ret)
		return ret;

	if (status != W5500_Sn_SR_ESTABLISHED)
		return -EAGAIN;

	/* Bring up a replacement listener on the same port. */
	ret = _get_closed_socket(wd, &new_id);
	if (ret)
		return ret;

	ret = w5500_socket_open(wd->mac_dev, new_id, W5500_Sn_MR_TCP,
				serv_slot->buff_kb);
	if (ret)
		return ret;

	ret = w5500_socket_bind(wd->mac_dev, new_id, serv_slot->local_port);
	if (ret)
		goto close_new;

	ret = w5500_socket_listen(wd->mac_dev, new_id);
	if (ret)
		goto close_new;

	/* The client descriptor reuses the original (connected) socket. */
	cli = no_os_socket_alloc(sock->net, &w5500_socket_ops, serv_slot->id);
	if (!cli) {
		ret = -ENOMEM;
		goto close_new;
	}
	cli->extra = serv_slot;
	serv_slot->state = W5500_SOCK_CONNECTED;

	/* The new hardware socket takes over the listening role. */
	new_slot = &wd->sockets[new_id];
	new_slot->id = new_id;
	new_slot->net = wd;
	new_slot->proto = NO_OS_SOCKET_TCP;
	new_slot->buff_kb = serv_slot->buff_kb;
	new_slot->local_port = serv_slot->local_port;
	new_slot->state = W5500_SOCK_LISTENING;
	sock->extra = new_slot;
	sock->id = new_id;

	*client = cli;

	return 0;

close_new:
	w5500_socket_close(wd->mac_dev, new_id);
	wd->sockets[new_id].state = W5500_SOCK_CLOSED;

	return ret;
}

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

const struct no_os_net_platform_ops w5500_net_ops = {
	.init = w5500_net_init,
	.remove = w5500_net_remove,
	.step = w5500_net_step,
	.get_ip = w5500_net_get_ip,
	.socket_open = w5500_net_socket_open,
};

#endif /* NO_OS_NET */
