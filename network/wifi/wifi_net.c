/***************************************************************************//**
 *   @file   wifi_net.c
 *   @brief  WiFi (ESP8266) adapter for the no_os_net / no_os_socket interfaces.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "wifi_net.h"
#include "at_parser.h"
#include "no_os_net.h"
#include "no_os_socket.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_circular_buffer.h"

#define WIFI_INVALID_ID		0xffffffff

static const struct no_os_socket_ops wifi_socket_ops;

/**
 * @brief Get the index of a socket in the WIFI_SOCKET_UNUSED state.
 * @param wd - WiFi backend descriptor.
 * @param idx - destination for the found index.
 * @return 0 on success, -EMLINK if all the connections are in use.
 */
static inline int32_t _wifi_get_unused_socket(struct wifi_net_desc *wd,
		uint32_t *idx)
{
	uint32_t i;

	for (i = 0; i < WIFI_NB_SOCKETS; i++)
		if (wd->sockets[i].state == WIFI_SOCKET_UNUSED) {
			wd->sockets[i].state = WIFI_SOCKET_DISCONNECTED;
			*idx = i;

			return 0;
		}

	/* All the available connections are used */
	return -EMLINK;
}

/**
 * @brief Mark the socket at the given index as WIFI_SOCKET_UNUSED.
 * @param wd - WiFi backend descriptor.
 * @param id - socket pool index.
 */
static inline void _wifi_release_socket(struct wifi_net_desc *wd, uint32_t id)
{
	wd->sockets[id].state = WIFI_SOCKET_UNUSED;
}

/**
 * @brief Assign an unused ESP8266 connection id to a socket.
 * @param wd - WiFi backend descriptor.
 * @param sock_id - socket pool index.
 * @return 0 on success, -EMLINK if no connection id is available.
 */
static inline uint32_t _wifi_get_unused_conn(struct wifi_net_desc *wd,
		uint32_t sock_id)
{
	uint32_t i;

	for (i = 0; i < WIFI_NB_CLI_SOCKETS; i++)
		if (wd->conn_id_to_sock_id[i] == WIFI_INVALID_ID) {
			wd->conn_id_to_sock_id[i] = sock_id;
			wd->sockets[sock_id].conn_id = i;

			return 0;
		}

	return -EMLINK;
}

/**
 * @brief Release the ESP8266 connection id bound to a socket.
 * @param wd - WiFi backend descriptor.
 * @param sock_id - socket pool index.
 */
static inline void _wifi_release_conn(struct wifi_net_desc *wd,
				      uint32_t sock_id)
{
	uint32_t conn_id;

	conn_id = wd->sockets[sock_id].conn_id;
	if (conn_id != WIFI_INVALID_ID) {
		/*
		 * Connection can be released both from the callback or a
		 * stop_connection command, and it shouldn't be released twice.
		 */
		wd->sockets[sock_id].conn_id = WIFI_INVALID_ID;
		wd->conn_id_to_sock_id[conn_id] = WIFI_INVALID_ID;
	}
}

/**
 * @brief Find a pre-allocated client socket ready to receive a new connection.
 * @param wd - WiFi backend descriptor.
 * @return the socket pool index, or WIFI_INVALID_ID if none are available.
 */
static inline int32_t _get_initialized_client_id(struct wifi_net_desc *wd)
{
	uint32_t i;
	uint32_t id;

	for (i = 0; i < wd->server.back_log_clients; i++) {
		id = wd->server.client_ids[i];
		if (wd->sockets[id].state == WIFI_SOCKET_DISCONNECTED) {
			wd->sockets[id].state = WIFI_SOCKET_WAITING_ACCEPT;

			return id;
		}
	}

	return WIFI_INVALID_ID;
}

/**
 * @brief AT parser callback: notified when a connection is created or closed.
 * @param ctx - WiFi backend descriptor.
 * @param event - connection event.
 * @param conn_id - ESP8266 connection id.
 * @param cb - destination for the circular buffer the parser should fill.
 */
static void _wifi_connection_callback(void *ctx, enum at_event event,
				      uint32_t conn_id,
				      struct no_os_circular_buffer **cb)
{
	struct wifi_net_desc	*wd = ctx;
	struct wifi_socket_desc	*sock;
	int32_t			sock_id;

	sock_id = wd->conn_id_to_sock_id[conn_id];
	if (event == AT_NEW_CONNECTION) {
		if (sock_id != WIFI_INVALID_ID) {
			*cb = wd->sockets[sock_id].cb;
		} else {
			sock_id = _get_initialized_client_id(wd);
			if (sock_id == WIFI_INVALID_ID) {
				*cb = NULL;
			} else {
				sock = &wd->sockets[sock_id];
				/* Link new client connection to id */
				wd->conn_id_to_sock_id[conn_id] = sock_id;
				sock->conn_id = conn_id;
				sock->state = WIFI_SOCKET_WAITING_ACCEPT;
				*cb = sock->cb;
			}
		}
	} else if (event == AT_CLOSED_CONNECTION) {
		if (sock_id != WIFI_INVALID_ID) {
			wd->sockets[sock_id].state = WIFI_SOCKET_DISCONNECTED;
			_wifi_release_conn(wd, sock_id);
		}
	}
}

/**
 * @brief Check whether a socket is one of the server's pre-allocated clients.
 * @param wd - WiFi backend descriptor.
 * @param sock_id - socket pool index.
 * @return true if it is a server-owned client socket, false otherwise.
 */
static inline bool _is_server_socket(struct wifi_net_desc *wd, uint32_t sock_id)
{
	uint32_t i;

	for (i = 0; i < wd->server.back_log_clients; i++)
		if (wd->server.client_ids[i] == sock_id)
			return true;

	return false;
}

/* Forward declarations for the internal, pool-index based socket helpers. */
static int32_t _wifi_socket_disconnect(struct wifi_net_desc *wd,
				       uint32_t sock_id);
static int32_t _wifi_socket_close(struct wifi_net_desc *wd, uint32_t sock_id);

/**
 * @brief Get the current IP address as a dotted-decimal string.
 * @param desc - the network interface descriptor.
 * @param buf - destination buffer.
 * @param size - size of the destination buffer.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_net_get_ip(struct no_os_net_desc *desc, char *buf,
			       uint32_t size)
{
	struct wifi_net_desc	*wd;
	union in_out_param	result;
	uint8_t			*ip;
	int32_t			ret;

	if (!desc || !desc->extra || !buf)
		return -EINVAL;

	wd = desc->extra;

	ret = at_run_cmd(wd->at, AT_GET_IP, AT_EXECUTE_OP, &result);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	at_to_str(&ip, &result.out.result);
	if (strlen((char *)ip) + 1 > size)
		return -ENOMEM;

	strcpy(buf, (char *)ip);

	return 0;
}

/**
 * @brief Configure the ESP8266 and join the access point.
 * @param desc - the network interface descriptor to be returned.
 * @param param - initialization parameter (extra holds a wifi_net_param).
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_net_init(struct no_os_net_desc **desc,
			     const struct no_os_net_init_param *param)
{
	struct wifi_net_param	*wifi_param;
	struct no_os_net_desc	*nd;
	struct wifi_net_desc	*wd;
	struct at_init_param	at_param;
	union in_out_param	par;
	int32_t			ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	wifi_param = param->extra;
	if (!wifi_param->ssid)
		return -EINVAL;

	nd = no_os_calloc(1, sizeof(*nd));
	if (!nd)
		return -ENOMEM;

	wd = no_os_calloc(1, sizeof(*wd));
	if (!wd) {
		ret = -ENOMEM;
		goto free_nd;
	}

	memset(wd->conn_id_to_sock_id, (int8_t)WIFI_INVALID_ID,
	       sizeof(wd->conn_id_to_sock_id));
	wd->server.id = WIFI_INVALID_ID;

	at_param.irq_desc = wifi_param->irq_desc;
	at_param.uart_desc = wifi_param->uart_desc;
	at_param.uart_irq_conf = wifi_param->uart_irq_conf;
	at_param.uart_irq_id = wifi_param->uart_irq_id;
	at_param.connection_callback = _wifi_connection_callback;
	at_param.callback_ctx = wd;
	at_param.sw_reset_en = wifi_param->sw_reset_en;

	ret = at_init(&wd->at, &at_param);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_wd;

	par.in.wifi_mode = CLIENT;
	ret = at_run_cmd(wd->at, AT_SET_OPERATION_MODE, AT_SET_OP, &par);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_at;

	par.in.conn_type = MULTIPLE_CONNECTION;
	ret = at_run_cmd(wd->at, AT_SET_CONNECTION_TYPE, AT_SET_OP, &par);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_at;

	str_to_at(&par.in.network.ssid, (uint8_t *)wifi_param->ssid);
	str_to_at(&par.in.network.pwd, (uint8_t *)wifi_param->pwd);
	ret = at_run_cmd(wd->at, AT_CONNECT_NETWORK, AT_SET_OP, &par);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_at;

	nd->extra = wd;
	*desc = nd;

	return 0;

free_at:
	at_remove(wd->at);
free_wd:
	no_os_free(wd);
free_nd:
	no_os_free(nd);

	return ret;
}

/**
 * @brief Disconnect from the access point and free all resources.
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_net_remove(struct no_os_net_desc *desc)
{
	struct wifi_net_desc	*wd;
	uint32_t		i;

	if (!desc || !desc->extra)
		return -EINVAL;

	wd = desc->extra;

	for (i = 0; i < WIFI_NB_SOCKETS; i++)
		_wifi_socket_close(wd, i);

	at_run_cmd(wd->at, AT_DISCONNECT_NETWORK, AT_EXECUTE_OP, NULL);
	at_remove(wd->at);
	no_os_free(wd);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Service the network interface.
 *
 * The AT parser is driven by the UART interrupt, filling per-socket circular
 * buffers as data arrives, so there is nothing to pump here; this is a no-op
 * kept for interface symmetry.
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_net_step(struct no_os_net_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	return 0;
}

/**
 * @brief Open a pool socket slot (internal, no no_os_socket_desc).
 *
 * Used to pre-allocate the server's back-log client slots without minting a
 * user-facing socket descriptor; accept() stamps one later.
 * @param wd - WiFi backend descriptor.
 * @param sock_id - destination for the allocated pool index.
 * @param proto - Layer 4 protocol.
 * @param buff_size - size of the socket's receive circular buffer.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t _wifi_net_open_client(struct wifi_net_desc *wd,
				     uint32_t *sock_id,
				     enum no_os_socket_protocol proto,
				     uint32_t buff_size)
{
	struct wifi_socket_desc	*slot;
	uint32_t		id;
	int32_t			ret;

	ret = _wifi_get_unused_socket(wd, &id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	slot = &wd->sockets[id];

	ret = no_os_cb_init(&slot->cb, buff_size);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		_wifi_release_socket(wd, id);
		return ret;
	}

	slot->proto = proto;
	slot->cb_size = buff_size;
	slot->id = id;
	slot->net = wd;

	*sock_id = id;

	return 0;
}

/**
 * @brief Open (create) a socket on the network interface.
 * @param net - the owning network interface descriptor.
 * @param sock - destination for the newly created socket descriptor.
 * @param proto - Layer 4 protocol (TCP or UDP).
 * @param buff_size - size of the socket's receive circular buffer.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_net_socket_open(struct no_os_net_desc *net,
				    struct no_os_socket_desc **sock,
				    enum no_os_socket_protocol proto,
				    uint32_t buff_size)
{
	struct wifi_net_desc	*wd;
	struct wifi_socket_desc	*slot;
	struct no_os_socket_desc *s;
	uint32_t		id;
	int32_t			ret;

	if (!net || !net->extra || !sock)
		return -EINVAL;

	wd = net->extra;

	ret = _wifi_net_open_client(wd, &id, proto, buff_size);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	slot = &wd->sockets[id];

	s = no_os_socket_alloc(net, &wifi_socket_ops, id);
	if (!s) {
		no_os_cb_remove(slot->cb);
		slot->cb = NULL;
		_wifi_release_socket(wd, id);
		return -ENOMEM;
	}
	s->extra = slot;

	*sock = s;

	return 0;
}

/**
 * @brief Disconnect a socket (pool-index based internal helper).
 * @param wd - WiFi backend descriptor.
 * @param sock_id - socket pool index.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t _wifi_socket_disconnect(struct wifi_net_desc *wd,
				       uint32_t sock_id)
{
	union in_out_param	param;
	int32_t			ret;
	struct wifi_socket_desc	*sock;
	uint32_t		i;
	uint32_t		id;

	if (!wd || sock_id >= WIFI_NB_SOCKETS)
		return -EINVAL;

	sock = &wd->sockets[sock_id];
	if (sock->state == WIFI_SOCKET_UNUSED)
		return -ENOENT;

	if (sock->state == WIFI_SOCKET_DISCONNECTED)
		/* A socket can be disconnected by the peer */
		return 0;

	if (sock_id == wd->server.id) {
		/* Disable ESP8266 server mode */
		param.in.server.action = DELETE_SERVER;
		ret = at_run_cmd(wd->at, AT_SET_SERVER, AT_SET_OP, &param);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		for (i = 0; i < wd->server.back_log_clients; i++) {
			id = wd->server.client_ids[i];
			no_os_cb_remove(wd->sockets[id].cb);
			wd->sockets[id].cb = NULL;
			_wifi_release_socket(wd, id);
		}

		/* Remove server reference */
		wd->server.id = WIFI_INVALID_ID;
	} else {
		param.in.conn_id = sock->conn_id;
		ret = at_run_cmd(wd->at, AT_STOP_CONNECTION, AT_SET_OP, &param);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
		_wifi_release_conn(wd, sock_id);
	}

	sock->state = WIFI_SOCKET_DISCONNECTED;

	return 0;
}

/**
 * @brief Close a socket (pool-index based internal helper).
 * @param wd - WiFi backend descriptor.
 * @param sock_id - socket pool index.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t _wifi_socket_close(struct wifi_net_desc *wd, uint32_t sock_id)
{
	struct wifi_socket_desc	*sock;
	int32_t			ret;

	if (!wd || sock_id >= WIFI_NB_SOCKETS)
		return -EINVAL;

	sock = &wd->sockets[sock_id];
	if (sock->state == WIFI_SOCKET_UNUSED)
		return 0;

	ret = _wifi_socket_disconnect(wd, sock_id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	/*
	 * The server's client circular buffers are released together with the
	 * server socket (in _wifi_socket_disconnect), so only release standalone
	 * sockets here.
	 */
	if (!_is_server_socket(wd, sock_id)) {
		no_os_cb_remove(sock->cb);
		sock->cb = NULL;
	}
	_wifi_release_socket(wd, sock_id);

	return 0;
}

/**
 * @brief Close a socket connection and free its descriptor.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_sock_close(struct no_os_socket_desc *sock)
{
	struct wifi_socket_desc	*slot;
	int32_t			ret;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;
	ret = _wifi_socket_close(slot->net, slot->id);
	no_os_free(sock);

	return ret;
}

/**
 * @brief Connect to a remote listening socket.
 * @param sock - the socket descriptor.
 * @param addr - IP/port of the remote socket.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_sock_connect(struct no_os_socket_desc *sock,
				 struct no_os_sockaddr *addr)
{
	union in_out_param	param;
	int32_t			ret;
	struct wifi_socket_desc	*slot;
	struct wifi_net_desc	*wd;

	if (!sock || !sock->extra || !addr)
		return -EINVAL;

	slot = sock->extra;
	wd = slot->net;

	if (slot->id == wd->server.id)
		return -EINVAL;

	if (slot->state == WIFI_SOCKET_UNUSED)
		return -ENOENT;

	if (slot->state == WIFI_SOCKET_CONNECTED)
		return -EISCONN;

	ret = _wifi_get_unused_conn(wd, slot->id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	str_to_at(&param.in.connection.addr, (uint8_t *)addr->addr);
	param.in.connection.port = addr->port;
	param.in.connection.id = slot->conn_id;
	param.in.connection.soket_type = slot->proto;

	ret = at_run_cmd(wd->at, AT_START_CONNECTION, AT_SET_OP, &param);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		_wifi_release_conn(wd, slot->id);
		return ret;
	}

	slot->state = WIFI_SOCKET_CONNECTED;

	return 0;
}

/**
 * @brief Close a connection from the client side.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_sock_disconnect(struct no_os_socket_desc *sock)
{
	struct wifi_socket_desc	*slot;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;

	return _wifi_socket_disconnect(slot->net, slot->id);
}

/**
 * @brief Send data over a connected socket.
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of the data array.
 * @return Number of bytes sent, negative error code otherwise.
 */
static int32_t wifi_sock_send(struct no_os_socket_desc *sock, const void *data,
			      uint32_t size)
{
	union in_out_param	param;
	int32_t			ret;
	struct wifi_socket_desc	*slot;
	struct wifi_net_desc	*wd;
	uint32_t		to_send;
	uint32_t		i;

	if (!sock || !sock->extra || !data)
		return -EINVAL;

	slot = sock->extra;
	wd = slot->net;

	if (slot->id == wd->server.id)
		return -EINVAL;

	if (slot->state != WIFI_SOCKET_CONNECTED)
		return -ENOTCONN;

	i = 0;
	do {
		to_send = no_os_min(size - i, MAX_CIPSEND_DATA);
		param.in.send_data.id = slot->conn_id;
		param.in.send_data.data.buff = ((uint8_t *)data) + i;
		param.in.send_data.data.len = to_send;
		ret = at_run_cmd(wd->at, AT_SEND, AT_SET_OP, &param);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		i += to_send;
	} while (i < size);

	return (int32_t)size;
}

/**
 * @brief Receive data over a connected socket (non-blocking).
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of data to be read.
 * @return Number of bytes received, -EAGAIN if none pending, error otherwise.
 */
static int32_t wifi_sock_recv(struct no_os_socket_desc *sock, void *data,
			      uint32_t size)
{
	struct wifi_socket_desc	*slot;
	struct wifi_net_desc	*wd;
	uint32_t		available_size;
	int32_t			ret;

	if (!sock || !sock->extra || !data || !size)
		return -EINVAL;

	slot = sock->extra;
	wd = slot->net;

	if (slot->id == wd->server.id)
		return -EINVAL;

	if (slot->state != WIFI_SOCKET_CONNECTED)
		return -ENOTCONN;

	no_os_cb_size(slot->cb, &available_size);
	if (available_size == 0)
		return -EAGAIN;

	size = no_os_min(available_size, size);
	ret = no_os_cb_read(slot->cb, data, size);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return size;
}

/**
 * @brief Not implemented (TCP only).
 * @return -ENOSYS
 */
static int32_t wifi_sock_sendto(struct no_os_socket_desc *sock,
				const void *data, uint32_t size,
				const struct no_os_sockaddr *to)
{
	NO_OS_UNUSED_PARAM(sock);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(size);
	NO_OS_UNUSED_PARAM(to);

	/* TODO: Implement for UDP */
	return -ENOSYS;
}

/**
 * @brief Not implemented (TCP only).
 * @return -ENOSYS
 */
static int32_t wifi_sock_recvfrom(struct no_os_socket_desc *sock, void *data,
				  uint32_t size, struct no_os_sockaddr *from)
{
	NO_OS_UNUSED_PARAM(sock);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(size);
	NO_OS_UNUSED_PARAM(from);

	/* TODO: Implement for UDP */
	return -ENOSYS;
}

/**
 * @brief Bind a socket to a local port (marks it as the server socket).
 * @param sock - the socket descriptor.
 * @param port - port number to bind the socket to.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_sock_bind(struct no_os_socket_desc *sock, uint16_t port)
{
	struct wifi_socket_desc	*slot;
	struct wifi_net_desc	*wd;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;
	wd = slot->net;

	if (wd->server.id != WIFI_INVALID_ID)
		return -EMLINK;

	if (slot->state == WIFI_SOCKET_UNUSED)
		return -ENODEV;

	/* Configure the current socket as server */
	wd->server.id = slot->id;
	wd->server.port = port;

	return 0;
}

/**
 * @brief Configure a socket to listen for connections (TCP server).
 * @param sock - the socket descriptor.
 * @param backlog - number of connections in the socket's listen queue.
 * @return 0 in the case of success, negative error code otherwise.
 */
static int32_t wifi_sock_listen(struct no_os_socket_desc *sock,
				uint32_t backlog)
{
	struct wifi_socket_desc	*server_sock;
	struct wifi_socket_desc	*cli_sock;
	struct wifi_net_desc	*wd;
	union in_out_param	param;
	int32_t			ret = 0;
	uint32_t		*i;
	uint32_t		id;

	if (!sock || !sock->extra)
		return -EINVAL;

	server_sock = sock->extra;
	wd = server_sock->net;

	/* Socket bind must be done before listen */
	if (wd->server.id == WIFI_INVALID_ID || server_sock->id != wd->server.id)
		return -ENOTCONN;

	/* Initialize sockets in order not to allocate memory in interrupts */
	i = &wd->server.back_log_clients;
	if (backlog == 0)
		backlog = MAX_CONNECTIONS;
	else
		backlog = no_os_min(backlog, MAX_CONNECTIONS);

	for (*i = 0; *i < backlog; (*i)++) {
		ret = _wifi_net_open_client(wd, &id, server_sock->proto,
					    server_sock->cb_size);
		if (NO_OS_IS_ERR_VALUE(ret))
			break;

		cli_sock = &wd->sockets[id];
		if (*i == 0 && server_sock->cb) {
			/* Reuse the server socket's own buffer for the first client. */
			no_os_cb_remove(cli_sock->cb);
			cli_sock->cb = server_sock->cb;
			server_sock->cb = NULL;
		}
		cli_sock->state = WIFI_SOCKET_DISCONNECTED;
		wd->server.client_ids[*i] = id;
	}
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_resources;

	param.in.server.action = CREATE_SERVER;
	param.in.server.port = wd->server.port;

	ret = at_run_cmd(wd->at, AT_SET_SERVER, AT_SET_OP, &param);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_resources;

	server_sock->state = WIFI_SOCKET_LISTENING;

	return 0;

free_resources:
	for (id = 0; id < *i; id++)
		_wifi_socket_close(wd, wd->server.client_ids[id]);
	*i = 0;

	return ret;
}

/**
 * @brief Accept an incoming connection, returning a new socket (non-blocking).
 * @param sock - the listening socket descriptor.
 * @param client - destination for the newly accepted socket descriptor.
 * @return 0 on success, -EAGAIN if no pending connection, error otherwise.
 */
static int32_t wifi_sock_accept(struct no_os_socket_desc *sock,
				struct no_os_socket_desc **client)
{
	struct wifi_socket_desc	*serv_slot;
	struct wifi_socket_desc	*cli_slot;
	struct no_os_socket_desc *cli;
	struct wifi_net_desc	*wd;
	uint32_t		i;

	if (!sock || !sock->extra || !client)
		return -EINVAL;

	serv_slot = sock->extra;
	wd = serv_slot->net;

	if (serv_slot->id != wd->server.id)
		return -EINVAL;

	/* Bind should be called first */
	if (serv_slot->state != WIFI_SOCKET_LISTENING)
		return -ENOTCONN;

	for (i = 0; i < WIFI_NB_SOCKETS; i++)
		if (wd->sockets[i].state == WIFI_SOCKET_WAITING_ACCEPT) {
			cli_slot = &wd->sockets[i];
			cli = no_os_socket_alloc(sock->net, &wifi_socket_ops, i);
			if (!cli)
				return -ENOMEM;
			cli->extra = cli_slot;
			cli_slot->state = WIFI_SOCKET_CONNECTED;
			*client = cli;

			return 0;
		}

	return -EAGAIN;
}

static const struct no_os_socket_ops wifi_socket_ops = {
	.close = wifi_sock_close,
	.connect = wifi_sock_connect,
	.disconnect = wifi_sock_disconnect,
	.send = wifi_sock_send,
	.recv = wifi_sock_recv,
	.sendto = wifi_sock_sendto,
	.recvfrom = wifi_sock_recvfrom,
	.bind = wifi_sock_bind,
	.listen = wifi_sock_listen,
	.accept = wifi_sock_accept,
};

const struct no_os_net_platform_ops wifi_net_ops = {
	.init = wifi_net_init,
	.remove = wifi_net_remove,
	.step = wifi_net_step,
	.get_ip = wifi_net_get_ip,
	.socket_open = wifi_net_socket_open,
};

#endif /* NO_OS_NET */
